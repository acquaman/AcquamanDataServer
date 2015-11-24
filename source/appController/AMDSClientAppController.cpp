#include "AMDSClientAppController.h"

#include <QtNetwork>

#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientRequestSupport.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientStatisticsRequest.h"
#include "ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "Connection/AMDSClientTCPSocket.h"
#include "Connection/AMDSServer.h"
#include "util/AMDSRunTimeSupport.h"

AMDSClientAppController *AMDSClientAppController::clientAppController()
{
	if (!appController_) {
		appController_ = new AMDSClientAppController();

		AMDSClientAppController *asClientAppController = qobject_cast<AMDSClientAppController *>(appController_);
		QTimer::singleShot(0, asClientAppController, SLOT(openNetworkSession()));
	}

	return qobject_cast<AMDSClientAppController *>(appController_);
}

AMDSClientAppController::AMDSClientAppController(QObject *parent) :
	AMDSAppController(AMDSAppController::Client, parent)
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
	return (networkSession_ && networkSession_->isOpen());
}

QStringList AMDSClientAppController::getBufferNamesByServer(const QString &serverIdentifier)
{
	QStringList bufferNames;

	AMDSServer *server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		bufferNames = server->bufferNames();
	}

	return bufferNames;
}

QStringList AMDSClientAppController::getActiveSocketKeysByServer(const QString &serverIdentifier)
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
			QSettings settings(QSettings::UserScope, QLatin1String("AMDSClient"));
			settings.beginGroup(QLatin1String("Network"));
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
		else{
			emit networkSessionOpened();
		}
	}
}

void AMDSClientAppController::connectToServer(const AMDSServerConfiguration &serverConfiguration)
{
	connectToServer(serverConfiguration.ipAddress(), serverConfiguration.port());
}

void AMDSClientAppController::connectToServer(const QString &hostName, quint16 portNumber)
{
	QString serverIdentifier = AMDSServer::generateServerIdentifier(hostName, portNumber);
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (!server) {
		server = new AMDSServer(hostName, portNumber);
		activeServers_.insert(serverIdentifier, server);

		connect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SIGNAL(requestDataReady(AMDSClientRequest*)));
		connect(server, SIGNAL(AMDSServerError(QString,int,QString,QString)), this, SLOT(onAMDSServerError(QString,int,QString,QString)));
		emit newServerConnected(server->serverIdentifier());

		// request the introspection data for all the buffers defined in this server
		QString bufferName = "All";
		requestClientData(hostName, portNumber, bufferName);
	}
}

void AMDSClientAppController::disconnectWithServer(const QString &serverIdentifier)
{
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		disconnect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SIGNAL(requestDataReady(AMDSClientRequest*)));
		disconnect(server, SIGNAL(AMDSServerError(QString,int,QString,QString)), this, SLOT(onAMDSServerError(QString,int,QString,QString)));

		activeServers_.remove(serverIdentifier);
		server->deleteLater();
		emit serverError(AMDS_CLIENT_ERR_SERVER_DISCONNECTED, true, serverIdentifier, "Server is disconnected");
	}
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Statistics);
		if (clientRequest) {
			clientTCPSocket->sendClientRequest(clientRequest);

			return true;
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Introspection);
		if (clientRequest) {
			AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if(clientIntrospectionRequest) {
				clientIntrospectionRequest->setBufferName(bufferName);

				clientTCPSocket->sendClientRequest(clientIntrospectionRequest);
				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &startTime, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::StartTimePlusCount);
		if (clientRequest) {
			AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
			if(clientStartTimePlusCountDataRequest){
				clientStartTimePlusCountDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, startTime, count);

				clientTCPSocket->sendClientRequest(clientStartTimePlusCountDataRequest);
				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, quint64 relativeCount, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::RelativeCountPlusCount);
		if (clientRequest) {
			AMDSClientRelativeCountPlusCountDataRequest *clientRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
			if(clientRelativeCountPlusCountDataRequest){
				clientRelativeCountPlusCountDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, relativeCount, count);

				clientTCPSocket->sendClientRequest(clientRelativeCountPlusCountDataRequest);
				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &startTime, const QDateTime &endTime, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::StartTimeToEndTime);
		if (clientRequest) {
			AMDSClientStartTimeToEndTimeDataRequest *clientStartTimeToEndTimeDataRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
			if(clientStartTimeToEndTimeDataRequest){
				clientStartTimeToEndTimeDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, startTime, endTime);

				clientTCPSocket->sendClientRequest(clientStartTimeToEndTimeDataRequest);
				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &middleTime, quint64 countBefore, quint64 countAfter, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter);
		if (clientRequest) {
			AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest *clientMiddleTimePlusCountBeforeAndAfterDataRequest = qobject_cast<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest*>(clientRequest);
			if(clientMiddleTimePlusCountBeforeAndAfterDataRequest){
				clientMiddleTimePlusCountBeforeAndAfterDataRequest->setAttributesValues(bufferName, includeStatus, enableFlattening, middleTime, countBefore, countAfter);

				clientTCPSocket->sendClientRequest(clientMiddleTimePlusCountBeforeAndAfterDataRequest);
				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QStringList &bufferNames, quint64 updateInterval, bool includeStatus, bool enableFlattening, QString handShakeSocketKey)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		if (bufferNames.length() == 0 && handShakeSocketKey.length() == 0) {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_CLIENT_ERR_FAILED_TO_PARSE_CONTINUOUS_MSG, QString("Failed to parse continuousDataRequest without interested buffer name(s) and handShakeSocketKey"));
			return false;
		}

		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Continuous);
		if (clientRequest) {
			AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
			if(clientContinuousDataRequest){
				clientContinuousDataRequest->setAttributesValues(includeStatus, enableFlattening, bufferNames, updateInterval, handShakeSocketKey);

				clientTCPSocket->sendClientRequest(clientContinuousDataRequest);

				return true;
			}
		}
	}

	return false;
}

bool AMDSClientAppController::requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, int commandId, const QString &value)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		AMDSClientRequest *clientRequest = instantiateClientRequest(AMDSClientRequestDefinitions::Configuration);
		if (clientRequest) {
			AMDSClientConfigurationRequest *clientConfigurationRequest = qobject_cast<AMDSClientConfigurationRequest*>(clientRequest);
			if(clientConfigurationRequest){
				clientConfigurationRequest->setBufferName(bufferName);

				if (commandId > 0) { // this is the command to ask for the list of configuration command for this buffergroup
					clientConfigurationRequest->appendCommand(commandId, value);
				}
				clientTCPSocket->sendClientRequest(clientConfigurationRequest);

				return true;
			}
		}
	}

	return false;
}

void AMDSClientAppController::onAMDSServerError(const QString &serverIdentifier, int errorCode, const QString &socketKey, const QString &errorMessage)
{
	Q_UNUSED(socketKey)

	bool disconnectServer = false;
	if (errorCode != QAbstractSocket::RemoteHostClosedError) {
		disconnectWithServer(serverIdentifier);
		disconnectServer = true;
	}
	emit serverError(errorCode, disconnectServer, serverIdentifier, errorMessage);
}

void AMDSClientAppController::onNetworkSessionOpened()
{
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_CLIENT_INFO_NETWORK_SESSION_STARTED, "Network session has been opened");

	// Save the used configuration
	QNetworkConfiguration config = networkSession_->configuration();
	QString id;
	if (config.type() == QNetworkConfiguration::UserChoice)
		id = networkSession_->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
	else
		id = config.identifier();

	QSettings settings(QSettings::UserScope, QLatin1String("AMDSClient"));
	settings.beginGroup(QLatin1String("Network"));
	settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
	settings.endGroup();

	emit networkSessionOpened();
}

AMDSClientTCPSocket * AMDSClientAppController::establishSocketConnection(const QString &hostName, quint16 portNumber)
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
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_CLIENT_ERR_FAILED_TO_PARSE_CLIENT_MSG, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return 0;
	}

	clientRequest->setClientLocalTime(QDateTime::currentDateTimeUtc());
	return clientRequest;
}
