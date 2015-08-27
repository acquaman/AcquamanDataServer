#include "AMDSClientManager.h"

#include <QtNetwork>

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/Connection/AMDSClientTCPSocket.h"
#include "source/util/AMDSErrorMonitor.h"


AMDSClientManager::AMDSClientManager(QObject *parent) :
	QObject(parent)
{
	networkSession_ = 0;
}

AMDSClientManager::~AMDSClientManager()
{
	activeServerSockets_.clear();

	if (networkSession_)
		networkSession_->deleteLater();
}

bool AMDSClientManager::isSessionOpen()
{
	return (!networkSession_ || networkSession_->isOpen());
}

void AMDSClientManager::openNetworkSession()
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

AMDSClientTCPSocket * AMDSClientManager::connectToServer(QString hostName, qint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = new AMDSClientTCPSocket(hostName, portNumber);
	connect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	connect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));

	return clientTCPSocket;
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData();
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QString &bufferName)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName);
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &startTime, quint64 count)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, startTime, count);
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, quint64 relativeCount, quint64 count)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, relativeCount, count);
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &startTime, QDateTime &endTime)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, startTime, endTime);
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferName, middleTime, countBefore, countAfter);
	}
}

void AMDSClientManager::requestClientData(QString &hostName, qint16 portNumber, QStringList &bufferNames, quint64 updateInterval, QString handShakeSocketKey)
{
	AMDSClientTCPSocket * clientTCPSocket = connectToServer(hostName, portNumber);
	if (clientTCPSocket) {
		clientTCPSocket->requestData(bufferNames, updateInterval, handShakeSocketKey);
	}
}


void AMDSClientManager::onNetworkSessionOpened()
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

void AMDSClientManager::onSocketDataReady(AMDSClientTCPSocket* clientTCPSocket, AMDSClientRequest *clientRequest)
{
	if (clientRequest->isContinuousMessage()) {
		activeServerSockets_.insert(clientRequest->socketKey(), clientTCPSocket);
	} else {
		// the connection is done, the socket should be released
		removeTCPSocket(clientTCPSocket);
	}

	emit requestDataReady(clientRequest);
}

void AMDSClientManager::onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketErrorCode)
{

	QString errorString;
	switch (socketErrorCode) {
	case QAbstractSocket::RemoteHostClosedError:
		errorString = "Connection closed by remove host";
		break;
	case QAbstractSocket::HostNotFoundError:
		errorString = "The host was not found. Please check the host name and port settings.";
		break;
	case QAbstractSocket::ConnectionRefusedError:
		errorString = "The connection was refused by the peer. Make sure the fortune server is running, and check that "
					"the host name and port settings are correct.";
		break;
	default:
		errorString = QString("The following error occurred: %1.").arg(clientTCPSocket->errorString());
	}

	emit socketError(clientTCPSocket->serverSocketIdentifier(), errorString);
	removeTCPSocket(clientTCPSocket);
}

void AMDSClientManager::removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket)
{
	// try to remove from the active socket list
	if (activeServerSockets_.contains(clientTCPSocket->socketKey()))
		activeServerSockets_.remove(clientTCPSocket->socketKey());

	disconnect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	disconnect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));

	clientTCPSocket->deleteLater();
}
