#include "AMDSCentralServer.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "appController/AMDSServerAppController.h"
#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "DataElement/AMDSDwellStatusData.h"
#include "DataElement/AMDSBufferGroup.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "DataHolder/AMDSDataHolder.h"
#include "util/AMDSRunTimeSupport.h"

AMDSCentralServer::AMDSCentralServer(QObject *parent) :
	QObject(parent)
{
	// initialize the app controller
	AMDSServerAppController::serverAppController();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, "Starting Acquaman Data Server application ...");

	qRegisterMetaType<AMDSDwellStatusData>();
	qRegisterMetaType<AMDSDataHolderList>();


	maxBufferSize_ = 1000*60*60*10;
}

AMDSCentralServer::~AMDSCentralServer()
{
	foreach (AMDSThreadedBufferGroup *bufferGroupManager, bufferGroupManagers_) {
		bufferGroupManager->deleteLater();
	}
	bufferGroupManagers_.clear();

	tcpDataServer_->deleteLater();

	AMDSServerAppController::releaseAppController();
}

void AMDSCentralServer::initializeAndStartServices()
{
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, " ... initialize and start the TCP server ...");
	initializeAndStartTCPServer();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, " ... initialize the bufferGroup ...");
	initializeBufferGroup();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, " ... initialize the detector managers ...");
	initializeDetectorManager();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, " ... initialize and start the Data server...");
	initializeAndStartDetectorServer();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_START_SERVER_APP, " ... wrap up initialization...");
	wrappingUpInitialization();
}

void AMDSCentralServer::initializeAndStartTCPServer()
{
	tcpDataServer_ = new AMDSThreadedTCPDataServer(this);

	connect(tcpDataServer_->server(), SIGNAL(error(quint8,quint16,QString)), this, SLOT(onDataServerErrorHandler(quint8,quint16,QString)));
	connect(tcpDataServer_->server(), SIGNAL(clientRequestRead(AMDSClientRequest*)), this, SLOT(onDataServerClientRequestReady(AMDSClientRequest*)));
	connect(this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
}

void AMDSCentralServer::onDataServerErrorHandler(quint8 errorLevel, quint16 errorCode, const QString &errorMessage)
{
	switch(errorLevel) {
	case AMDSRunTimeSupport::Error:
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, errorCode, errorMessage);
		QCoreApplication::quit();
		break;
	default:
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, errorCode, errorMessage);
		break;
	}
}

void AMDSCentralServer::onDataServerClientRequestReady(AMDSClientRequest *clientRequest)
{
	switch ( clientRequest->requestType() ) {
	case AMDSClientRequestDefinitions::Introspection:
		processIntrospectionClientRequest(clientRequest);
		break;
	case AMDSClientRequestDefinitions::Configuration:
		processConfigurationClientRequest(clientRequest);
		break;
	default:
		processClientDataRequest(clientRequest);
		break;
	}
}

void AMDSCentralServer::onDetectorServerStartDwelling(const QString &bufferName)
{
	AMDSThreadedBufferGroup* threadedBufferGroup = bufferGroupManagers_.value(bufferName);
	if (threadedBufferGroup) {
		threadedBufferGroup->clearBufferGroup();
	}
}

void AMDSCentralServer::processIntrospectionClientRequest(AMDSClientRequest *clientRequest)
{
	AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
	if(clientIntrospectionRequest){
		if(clientIntrospectionRequest->bufferName() == "All"){
			QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroupManagers_.constBegin();
			while (i != bufferGroupManagers_.constEnd()) {
				AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
				AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_BUFFER_DEF, QString("%1 definition is %2 %3 %4 %5 %6 %7").arg(i.key()).arg(oneInfo.name()).arg(oneInfo.description()).arg(oneInfo.units()).arg(oneInfo.size().toString()).arg(oneInfo.isFlattenEnabled()?"true":"false").arg(oneInfo.flattenMethod()));
				clientIntrospectionRequest->appendBufferGroupInfo(oneInfo);
				++i;
			}
		}
		else if(bufferGroupManagers_.contains(clientIntrospectionRequest->bufferName())) {
			clientIntrospectionRequest->appendBufferGroupInfo(bufferGroupManagers_.value(clientIntrospectionRequest->bufferName())->bufferGroupInfo());
		} else {
			clientIntrospectionRequest->setErrorMessage(QString("No buffer named %1, cannot introspect").arg(clientIntrospectionRequest->bufferName()));
		}
	} else {
		clientIntrospectionRequest->setErrorMessage(QString("The message (%1) is NOT an instrospection message.").arg(clientRequest->socketKey()));
	}

	emit clientRequestProcessed(clientIntrospectionRequest);
}

void AMDSCentralServer::processConfigurationClientRequest(AMDSClientRequest *clientRequest)
{
	QString errorMessage = "Succeeded to process the data request";
	AMDSClientConfigurationRequest *clientConfigurationRequest = qobject_cast<AMDSClientConfigurationRequest*>(clientRequest);

	QString bufferGroup = clientConfigurationRequest->bufferName();
	if (clientConfigurationRequest->configurationCommands().size() == 0) {
		//request the list of available commands
		fillConfigurationCommandForClientRequest(bufferGroup, clientConfigurationRequest);
		clientConfigurationRequest->setErrorMessage(errorMessage);
		emit clientRequestProcessed(clientConfigurationRequest);
	} else {
		emit configurationRequestReceived(clientConfigurationRequest);
	}
}

void AMDSCentralServer::processClientDataRequest(AMDSClientRequest *clientRequest)
{
	QString errorMessage = "Succeeded to process the data request";

	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest*>(clientRequest);
	if(clientDataRequest){
		if ( clientDataRequest->isContinuousMessage()) {
			AMDSClientContinuousDataRequest *continuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientDataRequest);
			if (!continuousDataRequest || !continuousDataRequest->startContinuousRequestTimer()) {
				return;
			}

			QStringList requestedBufferNames = continuousDataRequest->bufferNames();
			foreach (QString bufferName, requestedBufferNames) {
				AMDSClientDataRequest *continuousBufferDataRequest = continuousDataRequest->bufferDataRequest(bufferName);
				AMDSThreadedBufferGroup *threadedBufferGroup = bufferGroupManagers_.value(bufferName, 0);
				if (threadedBufferGroup) {
					threadedBufferGroup->forwardClientRequest(continuousBufferDataRequest);
				} else {
					AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_INVALID_REQUEST, QString("Invalid client data request with buffer name: %1").arg(continuousBufferDataRequest->bufferName()));
					emit clientRequestProcessed(continuousBufferDataRequest);
				}
			}

		} else {
			AMDSThreadedBufferGroup *threadedBufferGroup = bufferGroupManagers_.value(clientDataRequest->bufferName(), 0);
			if (threadedBufferGroup) {
				threadedBufferGroup->forwardClientRequest(clientDataRequest);
			} else {
				errorMessage = QString("ERROR: %1 - Invalid client data request with buffer name: %2").arg(AMDS_SERVER_ALT_INVALID_REQUEST).arg(clientDataRequest->bufferName());
				clientDataRequest->setErrorMessage(errorMessage);
				AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_INVALID_REQUEST, errorMessage);
				emit clientRequestProcessed(clientDataRequest);
			}
		}
	} else {
		errorMessage = QString("ERROR: %1 - The message (%2) is not a client data request.").arg(AMDS_SERVER_ALT_INVALID_REQUEST).arg(clientRequest->socketKey());
		clientRequest->setErrorMessage(errorMessage);
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_INVALID_REQUEST, errorMessage);
		emit clientRequestProcessed(clientRequest);
	}
}
