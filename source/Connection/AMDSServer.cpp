#include "AMDSServer.h"

#include <QtNetwork>

#include "source/Connection/AMDSClientTCPSocket.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"

AMDSServer::AMDSServer(QString hostName, quint16 portNumber, QObject *parent) :
    QObject(parent)
{
	hostName_ = hostName;
	portNumber_ = portNumber;
}

AMDSServer::~AMDSServer()
{
	// release the resources of the active sockets
	foreach (QString key,  activeTCPSockets_.keys()) {
		removeTCPSocket(activeTCPSockets_.value(key));
	}
}

void AMDSServer::setBufferNames(QStringList activeBufferNames) {
	bufferNames_.clear();
	bufferNames_.append(activeBufferNames);
}

AMDSClientTCPSocket * AMDSServer::establishSocketConnection()
{
	AMDSClientTCPSocket * clientTCPSocket = new AMDSClientTCPSocket(hostName_, portNumber_);
	connect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	connect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));

	return clientTCPSocket;
}

void AMDSServer::onSocketDataReady(AMDSClientTCPSocket* clientTCPSocket, AMDSClientRequest *clientRequest)
{
	if (clientRequest->isContinuousMessage()) {
		activeTCPSockets_.insert(clientRequest->socketKey(), clientTCPSocket);
	} else {
		if (clientRequest->isIntrospectionMessage()) {
			AMDSClientIntrospectionRequest *introspectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
			if (introspectionRequest && introspectionRequest->checkAllBuffer() ) {
				QStringList bufferNames = introspectionRequest->getAllBufferNames();
				setBufferNames(bufferNames);
			}
		}

		// the connection is done, the socket should be released
		removeTCPSocket(clientTCPSocket);
	}

	emit requestDataReady(clientRequest);
}

void AMDSServer::onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketErrorCode)
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
		errorString = "The connection was refused by the peer. Make sure the Acquaman Data Server is running, and check that "
					"the host name and port settings are correct.";
		break;
	default:
		errorString = QString("The following error occurred: %1.").arg(clientTCPSocket->errorString());
	}

	emit socketError(socketErrorCode, clientTCPSocket->socketKey(), errorString);
	removeTCPSocket(clientTCPSocket);
}

void AMDSServer::removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket)
{
	// try to remove from the active socket list
	if (activeTCPSockets_.contains(clientTCPSocket->socketKey()))
		activeTCPSockets_.remove(clientTCPSocket->socketKey());

	disconnect(clientTCPSocket, SIGNAL(newRequestDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)), this, SLOT(onSocketDataReady(AMDSClientTCPSocket*, AMDSClientRequest*)));
	disconnect(clientTCPSocket, SIGNAL(socketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)), this, SLOT(onSocketError(AMDSClientTCPSocket*, QAbstractSocket::SocketError)));

	clientTCPSocket->deleteLater();
}
