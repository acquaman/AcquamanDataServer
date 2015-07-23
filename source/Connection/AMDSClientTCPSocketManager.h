#ifndef ADMSCLIENTTCPSOCKETMANAGER_H
#define ADMSCLIENTTCPSOCKETMANAGER_H

#include <QObject>

#include "source/ClientRequest/AMDSClientRequestDefinitions.h"
#include "source/Connection/AMDSClientTCPSocket.h"

class AMDSClientTCPSocketManager : public QObject
{
    Q_OBJECT
public:
	static AMDSClientTCPSocketManager *clientTCPSocketManager;
	static AMDSClientTCPSocketManager *socketManager() {
		if (clientTCPSocketManager == 0) {
			clientTCPSocketManager = new AMDSClientTCPSocketManager();
		}

		return clientTCPSocketManager;
	}

public:
	explicit AMDSClientTCPSocketManager(QObject *parent = 0);

	AMDSClientTCPSocket *tcpSocket(QString socketKey) { return clientTCPSocketsBySocketKey_.value(socketKey); }
	void appendSocket(AMDSClientTCPSocket *clientTCPSocket) {
		clientTCPSocketsBySocketKey_.insert(clientTCPSocket->socketKey(), clientTCPSocket);
		qDebug() << "active connections: " << clientTCPSocketsBySocketKey_.size();
	}
	void removeSocket(QString socketKey) { clientTCPSocketsBySocketKey_.remove(socketKey); }

signals:

public slots:
	void onTCPSocketConnect(quint8 requestTypeId, QString bufferName);

protected:
	QHash<QString, AMDSClientTCPSocket *> clientTCPSocketsBySocketKey_;
	QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientTCPSocket *> clientTCPSocketsByType_;

};

#endif // ADMSCLIENTTCPSOCKETMANAGER_H
