#ifndef ADMSCLIENTTCPSOCKETMANAGER_H
#define ADMSCLIENTTCPSOCKETMANAGER_H

#include <QObject>

#include "source/ClientRequest/AMDSClientRequestDefinitions.h"
#include "source/Connection/AMDSClientTCPSocket.h"

class AMDSClientTCPSocketManager : public QObject
{
    Q_OBJECT
public:
	static AMDSClientTCPSocketManager *clientTCPSocketManager_;
	static AMDSClientTCPSocketManager *socketManager() {
		if (clientTCPSocketManager_ == 0) {
			clientTCPSocketManager_ = new AMDSClientTCPSocketManager();
		}

		return clientTCPSocketManager_;
	}

public:
	explicit AMDSClientTCPSocketManager(QObject *parent = 0);
	~AMDSClientTCPSocketManager();

	/// returns the socket identifier
	inline QString serverSocketIdentifier(QString hostName, qint16 portNumber) { return QString("%1:%2").arg(hostName).arg(portNumber); }

	/// returns the AMDSClientTCPSocket with the given socket key
	inline AMDSClientTCPSocket *tcpSocket(QString socketKey) { return clientTCPSocketsBySocketKey_.value(socketKey); }
	/// appends a client socket to the list
	void appendSocket(AMDSClientTCPSocket *clientTCPSocket) ;
	/// remove a client socket from the list
	void removeSocket(QString socketKey);

	/// returns the AMDSClientTCPSocket with the given hostName
	inline AMDSClientTCPSocket *tcpSocketBySocketIdentifier(QString socketIdentifier) { return clientTCPSocketsByIdentifier_.value(socketIdentifier); }
	/// appends a client socket to the list
	void appendSocket(QString serverSocketIdentifier, AMDSClientTCPSocket *clientTCPSocket) ;
	/// remove a client socket from the list
	void removeSocketByIdentifier(QString identifier);

protected:
	/// the list of the client socket, hashed by the socketKey
	QHash<QString, AMDSClientTCPSocket *> clientTCPSocketsBySocketKey_;

	/// the list of the client socket, hashed by the hostName
	QHash<QString, AMDSClientTCPSocket *> clientTCPSocketsByIdentifier_;
};

#endif // ADMSCLIENTTCPSOCKETMANAGER_H
