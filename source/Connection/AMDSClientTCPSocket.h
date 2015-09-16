#ifndef AMDSCLIENTTCPSOCKET_H
#define AMDSCLIENTTCPSOCKET_H

#include <QObject>
#include <QDateTime>
#include <QTcpSocket>

class AMDSClientRequest;

#define AMDS_SERVER_ERR_TCP_SOCKET 20600
#define AMDS_SERVER_INFO_SOCKET_SEND_DATA 20601
#define AMDS_SERVER_ALT_VALIDATE_RESPONSE_NOT_IMPLEMENTED 20621

class AMDSClientTCPSocket: public QObject
{
	Q_OBJECT
public:
	explicit AMDSClientTCPSocket(const QString host, const quint16 port, QObject *parent = 0);
	virtual ~AMDSClientTCPSocket();

	/// Returns the socket connect identifier
	inline QString hostIdentifier() { return QString("%1:%2").arg(hostName_).arg(port_); }
	/// Returns the socket key of this connection
	inline QString socketKey() { return socketKey_; }
	/// Returns the errorStrong of the encapsulated tcpSocket_
	inline QString errorString() { return tcpSocket_->errorString(); }

signals:
	/// emit signal when there are errors
	void socketError(AMDSClientTCPSocket* socket, QAbstractSocket::SocketError error);
	/// new data is ready
	void newRequestDataReady(AMDSClientTCPSocket* socket, AMDSClientRequest *clientRequest);

public slots:
	/// slot to send client request to server
	void sendClientRequest(AMDSClientRequest *clientRequest);

protected slots:
	/// slot to handle the socket state changed signal
	void onSocketStateChanged(QAbstractSocket::SocketState);
	/// slot to handle the socket error signals
	void onSocketError(QAbstractSocket::SocketError error);
	/// slot to handle the data from server
	void readClientRequestMessage();

private:
	/// write request data to dataStream and socket
	void sendData(AMDSClientRequest *clientRequest);

protected:
	/// the host name of the connection
	QString hostName_;
	/// the port of the connection
	quint16 port_;

	/// the handler of the socket connection
	QTcpSocket *tcpSocket_;
	/// the socket key
	QString socketKey_;

	/// the messages to be sent
	QList<AMDSClientRequest *> clientRequestsToSend;

	/// flag to identify whether we are waiting for the second data package or not
	bool waitingMorePackages_;
	/// variable to remember how many bytes we already read
	quint32 readedBufferSize_;
	/// variable to remember how many bytes we suppose to read
	quint32 expectedBufferSize_;
	/// byte array to save the readed bytes temporarily
	QByteArray incomeDataBuffer_;
};

#endif // AMDSCLIENTTCPSOCKET_H
