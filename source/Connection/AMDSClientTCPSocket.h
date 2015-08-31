#ifndef AMDSCLIENTTCPSOCKET_H
#define AMDSCLIENTTCPSOCKET_H

#include <QObject>
#include <QDateTime>
#include <QTcpSocket>

class AMDSClientRequest;

class AMDSClientTCPSocket: public QObject
{
	Q_OBJECT
public:
	explicit AMDSClientTCPSocket(const QString host, const quint16 port, QObject *parent = 0);
	virtual ~AMDSClientTCPSocket();

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
	/// slot to request data from server for Statistics
	void requestData();
	/// slot to request data from server for Instrospection
	void requestData(QString &bufferName);
	/// slot to request data from server for StartTimePlusCount
	void requestData(QString &bufferName, QDateTime &startTime, quint64 count);
	/// slot to request data from server for RelativeCountPlusCount
	void requestData(QString &bufferName, quint64 relativeCount, quint64 count);
	/// slot to request data from server for StartTimeToEndTime
	void requestData(QString &bufferName, QDateTime &startTime, QDateTime &endTime);
	/// slot to request data from server for MiddleTimePlusCountBeforeAndAfter
	void requestData(QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter);
	/// slot to request data from server for Continuous
	void requestData(QStringList &bufferNames, quint64 updateInterval, QString handShakeSocketKey="");

protected slots:
	/// slot to handle the socket error signals
	void onSocketError(QAbstractSocket::SocketError error);
	/// slot to handle the data from server
	void readFortune();

private:
	/// write request data to dataStream and socket
	void sendData(AMDSClientRequest *clientRequest);

protected:
	/// the handler of the socket connection
	QTcpSocket *tcpSocket_;
	/// the socket key
	QString socketKey_;

	/// flag to identify whether we are waiting for the second data package or not
	bool waitingMorePackages_;
	/// variable to remember how many bytes we already read
	quint32 readedBufferSize_;
	/// variable to remember how many bytes we suppose to read
	quint32 expectedBufferSize_;
	/// byte array to save the readed bytes temporarily
	QByteArray *incomeDataBuffer_;
};

#endif // AMDSCLIENTTCPSOCKET_H
