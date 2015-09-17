#include "AMDSCentralServer.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "source/Connection/AMDSThreadedTCPDataServer.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/DataElement/AMDSThreadedBufferGroup.h"
#include "source/DataElement/AMDSBufferGroup.h"
#include "source/DataElement/AMDSBufferGroupInfo.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServer::AMDSCentralServer(QObject *parent) :
	QObject(parent)
{
	AMErrorMon::information(this, AMDS_SERVER_INFO_START_SERVER_APP, "Starting Acquaman Data Server application ...");

	dataServer_ = new AMDSThreadedTCPDataServer(this);
	connect(dataServer_->server(), SIGNAL(error(quint8,quint16,QString)), this, SLOT(onDataServerErrorHandler(quint8,quint16,QString)));
	connect(dataServer_->server(), SIGNAL(clientRequestRead(AMDSClientRequest*)), this, SLOT(onDataServerClientRequestReady(AMDSClientRequest*)));
	connect(this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
}

void AMDSCentralServer::startDataServerUpdate()
{
	initializeBufferGroup(1000*60*60*10); // 10 hours of 1kHz signal
	initializeTimer();
	startTimer();
}

void AMDSCentralServer::onDataServerErrorHandler(quint8 errorLevel, quint16 errorCode, const QString &errorMessage)
{
	switch(errorLevel) {
	case AMErrorReport::Serious:
		AMErrorMon::error(this, errorCode, errorMessage);
		QCoreApplication::quit();
		break;
	default:
		AMErrorMon::alert(this, errorCode, errorMessage);
		break;
	}
}

void AMDSCentralServer::onDataServerClientRequestReady(AMDSClientRequest *clientRequest){
	if(clientRequest->requestType() == AMDSClientRequestDefinitions::Introspection){
		AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
		if(clientIntrospectionRequest){
			if(clientIntrospectionRequest->bufferName() == "All"){
				QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroups_.constBegin();
				while (i != bufferGroups_.constEnd()) {
					AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
					AMErrorMon::information(this, AMDS_SERVER_INFO_BUFFER_DEF, QString("%1 definition is %2 %3 %4 %5 %6 %7").arg(i.key()).arg(oneInfo.name()).arg(oneInfo.description()).arg(oneInfo.units()).arg(oneInfo.size().toString()).arg(oneInfo.isFlattenEnabled()?"true":"false").arg(oneInfo.flattenMethod()));
					clientIntrospectionRequest->appendBufferGroupInfo(oneInfo);
					++i;
				}
			}
			else if(bufferGroups_.contains(clientIntrospectionRequest->bufferName()))
				clientIntrospectionRequest->appendBufferGroupInfo(bufferGroups_.value(clientIntrospectionRequest->bufferName())->bufferGroupInfo());
			else
				clientIntrospectionRequest->setErrorMessage(QString("No buffer named %1, cannot introspect").arg(clientIntrospectionRequest->bufferName()));

			emit clientRequestProcessed(clientIntrospectionRequest);
		}
	}
	else{
		AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest*>(clientRequest);
		if(clientDataRequest){
			if ( clientDataRequest->isContinuousMessage()) {
				AMDSClientContinuousDataRequest *continuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientDataRequest);
				if (!continuousDataRequest->startContinuousRequestTimer()) {
					return;
				}

				QStringList requestedBufferNames = continuousDataRequest->bufferNames();
				foreach (QString bufferName, requestedBufferNames) {
					AMDSClientDataRequest *dataRequest = continuousDataRequest->bufferDataRequest(bufferName);
					AMDSThreadedBufferGroup *threadedBufferGroup = bufferGroups_.value(bufferName, 0);
					if (threadedBufferGroup) {
						AMDSBufferGroup * bufferGroup = threadedBufferGroup->bufferGroup();
						dataRequest->setBufferGroupInfo(threadedBufferGroup->bufferGroupInfo());
						bufferGroup->processClientRequest(dataRequest);
					} else {
						AMErrorMon::alert(this, AMDS_SERVER_ALT_INVALID_REQUEST, QString("Invalid client data request with buffer name: %1").arg(dataRequest->bufferName()));
						emit clientRequestProcessed(dataRequest);
					}
				}

			} else {
				AMDSThreadedBufferGroup *threadedBufferGroup = bufferGroups_.value(clientDataRequest->bufferName(), 0);
				if (threadedBufferGroup) {
					AMDSBufferGroup * bufferGroup = threadedBufferGroup->bufferGroup();
					clientDataRequest->setBufferGroupInfo(threadedBufferGroup->bufferGroupInfo());
					bufferGroup->processClientRequest(clientRequest);
				} else {
					AMErrorMon::alert(this, AMDS_SERVER_ALT_INVALID_REQUEST, QString("Invalid client data request with buffer name: %1").arg(clientDataRequest->bufferName()));
					emit clientRequestProcessed(clientRequest);
				}
			}
		}
	}
}
