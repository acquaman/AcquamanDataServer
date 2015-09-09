#include "AMDSClientAppController.h"

#include <QtNetwork>

#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/Connection/AMDSClientTCPSocket.h"
#include "source/Connection/AMDSServer.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientAppController::AMDSClientAppController(QObject *parent) :
	QObject(parent)
{
	networkSession_ = 0;
}

AMDSClientAppController::~AMDSClientAppController()
{
	// release the resouces of the AMDSServers
	foreach (QString key, activeServers_.keys()) {
		AMDSServer *server = activeServers_.value(key);
		if (server)
			server->deleteLater();

		activeServers_.remove(key);
	}

	if (networkSession_) {
		disconnect(networkSession_, SIGNAL(opened()), this, SLOT(onNetworkSessionOpened()));
		networkSession_->deleteLater();
		networkSession_ = 0;
	}
}

bool AMDSClientAppController::isSessionOpen()
{
	return (!networkSession_ || networkSession_->isOpen());
}

QStringList AMDSClientAppController::getBufferNamesByServer(QString serverIdentifier)
{
	QStringList bufferNames;

	AMDSServer *server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		bufferNames = server->bufferNames();
	}

	bufferNames.insert(0, "All");

	return bufferNames;
}

QStringList AMDSClientAppController::getActiveSocketKeysByServer(QString &serverIdentifier)
{
	QStringList activeSocketKeys;

	AMDSServer *server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		activeSocketKeys = server->activeTCPSocketKeys();
	}

	activeSocketKeys.insert(0, "");
	return activeSocketKeys;
}

void AMDSClientAppController::openNetworkSession()
{
	if (!networkSession_) {
		QNetworkConfigurationManager manager;
		if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
			// Get saved network configuration
			QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
			settings.beginGroup(QLatin1String("QtNetwork"));
			const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
			settings.endGroup();

			// If the saved network configuration is not currently discovered use the system default
			QNetworkConfiguration config = manager.configurationFromIdentifier(id);
			if ((config.state() & QNetworkConfiguration::Discovered) !=
				QNetworkConfiguration::Discovered) {
				config = manager.defaultConfiguration();
			}

			networkSession_ = new QNetworkSession(config);
			connect(networkSession_, SIGNAL(opened()), this, SLOT(onNetworkSessionOpened()));

			emit networkSessionOpening();
			networkSession_->open();
		}
	}
}

void AMDSClientAppController::connectToServer(QString &hostName, quint16 portNumber)
{
	QString serverIdentifier = AMDSServer::generateServerIdentifier(hostName, portNumber);
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (!server) {
		server = new AMDSServer(hostName, portNumber);
		activeServers_.insert(serverIdentifier, server);

		connect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SIGNAL(requestDataReady(AMDSClientRequest*)));
		connect(server, SIGNAL(AMDSServerError(AMDSServer*,int,QString,QString)), this, SLOT(onAMDSServerError(AMDSServer*,int,QString,QString)));
		emit newServerConnected(server->serverIdentifier());

		// request the introspection data for all the buffers defined in this server
		QString bufferName = "All";
		requestClientData(hostName, portNumber, bufferName);
	}
}

void AMDSClientAppController::disconnectWithServer(QString serverIdentifier)
{
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		disconnect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SIGNAL(requestDataReady(AMDSClientRequest*)));
		disconnect(server, SIGNAL(AMDSServerError(AMDSServer*,int,QString,QString)), this, SLOT(onAMDSServerError(AMDSServer*,int,QString,QString)));

		activeServers_.remove(serverIdentifier);
		server->deleteLater();
		emit serverError(AMDS_CLIENT_ERR_SERVER_DISCONNECTED, true, serverIdentifier, "Server is disconnected");
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Statistics);
		if (clientRequest) {
			clientTCPSocket->sendClientRequest(clientRequest);
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Introspection);
		if (clientRequest) {
			AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if(clientIntrospectionRequest) {
				clientIntrospectionRequest->setBufferName(bufferName);

				clientTCPSocket->sendClientRequest(clientIntrospectionRequest);
			}
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::StartTimePlusCount);
		if (clientRequest) {
			AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
			if(clientStartTimePlusCountDataRequest){
				clientStartTimePlusCountDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, startTime, count);

				clientTCPSocket->sendClientRequest(clientStartTimePlusCountDataRequest);
			}
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, quint64 relativeCount, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::RelativeCountPlusCount);
		if (clientRequest) {
			AMDSClientRelativeCountPlusCountDataRequest *clientRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
			if(clientRelativeCountPlusCountDataRequest){
				clientRelativeCountPlusCountDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, relativeCount, count);

				clientTCPSocket->sendClientRequest(clientRelativeCountPlusCountDataRequest);
			}
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, QDateTime &endTime, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::StartTimeToEndTime);
		if (clientRequest) {
			AMDSClientStartTimeToEndTimeDataRequest *clientStartTimeToEndTimeDataRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
			if(clientStartTimeToEndTimeDataRequest){
				clientStartTimeToEndTimeDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, startTime, endTime);

				clientTCPSocket->sendClientRequest(clientStartTimeToEndTimeDataRequest);
			}
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter);
		if (clientRequest) {
			AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest *clientMiddleTimePlusCountBeforeAndAfterDataRequest = qobject_cast<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest*>(clientRequest);
			if(clientMiddleTimePlusCountBeforeAndAfterDataRequest){

				clientMiddleTimePlusCountBeforeAndAfterDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, middleTime, countBefore, countAfter);

				clientTCPSocket->sendClientRequest(clientMiddleTimePlusCountBeforeAndAfterDataRequest);
			}
		}
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QStringList &bufferNames, quint64 updateInterval, bool includeStatus, bool enableFlattening, QString handShakeSocketKey)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		if (bufferNames.length() == 0 && handShakeSocketKey.length() == 0) {
			AMDSErrorMon::alert(this, AMDS_CLIENT_ERR_FAILED_TO_PARSE_CONTINUOUS_MSG, QString("Failed to parse continuousDataRequest without interested buffer name(s) and handShakeSocketKey"));
			return;
		}

		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Continuous);
		if (clientRequest) {
			AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
			if(clientContinuousDataRequest){
				clientContinuousDataRequest->setAttributesValues(includeStatus, enableFlattening, bufferNames, updateInterval, handShakeSocketKey);

				clientTCPSocket->sendClientRequest(clientContinuousDataRequest);
			}
		}
	}
}

void AMDSClientAppController::onAMDSServerError(AMDSServer* server, int errorCode, QString socketKey, QString errorMessage)
{
	Q_UNUSED(socketKey)

	bool disconnectServer = false;
	if (errorCode != QAbstractSocket::RemoteHostClosedError) {
		disconnectWithServer(server->serverIdentifier());
		disconnectServer = true;
	}
	emit serverError(errorCode, disconnectServer, server->serverIdentifier(), errorMessage);
}

void AMDSClientAppController::onNetworkSessionOpened()
{
	AMDSErrorMon::information(this, AMDS_CLIENT_INFO_NETWORK_SESSION_STARTED, "Network session has been opened");

	// Save the used configuration
	QNetworkConfiguration config = networkSession_->configuration();
	QString id;
	if (config.type() == QNetworkConfiguration::UserChoice)
		id = networkSession_->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
	else
		id = config.identifier();

	QSettings settings(QSettings::UserScope, QLatin1String("Trolltech"));
	settings.beginGroup(QLatin1String("QtNetwork"));
	settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
	settings.endGroup();

	emit networkSessionOpened();
}

AMDSClientTCPSocket * AMDSClientAppController::establishSocketConnection(QString &hostName, quint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = 0;

	QString serverIdentifier = AMDSServer::generateServerIdentifier(hostName, portNumber);
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		clientTCPSocket = server->establishSocketConnection();
	} else {
		emit serverError(AMDS_CLIENT_ERR_SERVER_NOT_CONNECTED, true, serverIdentifier, "Server is not connected");
	}

	return clientTCPSocket;
}

AMDSClientRequest *AMDSClientAppController::instantiateClientRequest(AMDSClientRequestDefinitions::RequestType clientRequestType)
{
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, AMDS_CLIENT_ERR_FAILED_TO_PARSE_CLIENT_MSG, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
	}

	return clientRequest;
}