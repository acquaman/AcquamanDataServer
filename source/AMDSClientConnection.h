#ifndef AMDSCLIENTCONNECTION_H
#define AMDSCLIENTCONNECTION_H

#include <QTcpSocket>
#include <QVector>

class AMDSClientTCPSocket;
class AMDSClientRequest;

class AMDSClientConnection
{
	Q_OBJECT
public:
    AMDSClientConnection();

private slots:
	void requestNewFortune();
	void onSocketDataReady(AMDSClientTCPSocket* socket, AMDSClientRequest *clientRequest);

	void onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketError);
	void enableGetFortuneButton();
	void sessionOpened();

private:
	void removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket);

private:
	QString currentFortune;
	quint32 blockSize;

	QNetworkSession *networkSession;
};

#endif // AMDSCLIENTCONNECTION_H
