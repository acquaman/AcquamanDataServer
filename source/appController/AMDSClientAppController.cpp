#include "AMDSClientAppController.h"

#include <QtNetwork>

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
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

	if (networkSession_)
		networkSession_->deleteLater();
}

bool AMDSClientAppController::isSessionOpen()
{
	return (!networkSession_ || networkSession_->isOpen());
}

QStringList AMDSClientAppController::getBufferNamesByServer(QString serverIdentifier)
{
	QStringList bufferNames;

	AMDSServer *server = activeServers_.value(serverIdentifier, 0);
	if (server) {
		bufferNames = server->bufferNames();
	}

	bufferNames.insert(0, "All");

	return bufferNames;
}

QStringList AMDSClientAppController::getActiveSocketKeysByServer(QString serverIdentifier)
{
	QStringList activeSocketKeys;

	AMDSServer *server = activeServers_.value(serverIdentifier, 0);
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

void AMDSClientAppController::connectToServer(QString hostName, quint16 portNumber)
{
	QString serverIdentifier = AMDSServer::generateServerIdentifier(hostName, portNumber);
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (!server) {
		server = new AMDSServer(hostName, portNumber);
		activeServers_.insert(serverIdentifier, server);

		connect(server, SIGNAL(requestDataReady(AMDSClientRequest*)), this, SIGNAL(requestDataReady(AMDSClientRequest*)));
		connect(server, SIGNAL(socketError(int,QString,QString)), this, SIGNAL(socketError(int,QString,QString)));
		emit newServerConnected(server->serverIdentifier());

		// request the introspection data for all the buffers defined in this server
		QString bufferName = "All";
		requestClientData(hostName, portNumber, bufferName);
	}
}

AMDSClientTCPSocket * AMDSClientAppController::establishSocketConnection(QString hostName, quint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = 0;

	QString serverIdentifier = AMDSServer::generateServerIdentifier(hostName, portNumber);
	AMDSServer * server = getServerByServerIdentifier(serverIdentifier);
	if (server) {
		clientTCPSocket = server->establishSocketConnection();
	} else {
		emit serverError(AMDS_CLIENT_ERR_SERVER_NOT_CONNECTED, serverIdentifier, "Server is not connected");
	}

	return clientTCPSocket;
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData();
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName);
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, startTime, count, includeStatus, enableFlattening);
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, quint64 relativeCount, quint64 count, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, relativeCount, count, includeStatus, enableFlattening);
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, QDateTime &endTime, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, startTime, endTime, includeStatus, enableFlattening);
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter, bool includeStatus, bool enableFlattening)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, middleTime, countBefore, countAfter, includeStatus, enableFlattening);
	}
}

void AMDSClientAppController::requestClientData(QString &hostName, quint16 portNumber, QStringList &bufferNames, quint64 updateInterval, bool includeStatus, bool enableFlattening, QString handShakeSocketKey)
{
	AMDSClientTCPSocket * clientTCPSocket = establishSocketConnection(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferNames, updateInterval, includeStatus, enableFlattening, handShakeSocketKey);
	}
}

void AMDSClientAppController::onNetworkSessionOpened()
{
	AMDSErrorMon::information(this, 0, "Network session has been opened");

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
