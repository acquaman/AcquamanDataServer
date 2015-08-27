#ifndef AMDSCLIENTMANAGER_H
#define AMDSCLIENTMANAGER_H

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QTcpSocket>

class QNetworkSession;

class AMDSClientTCPSocket;
class AMDSClientRequest;

#define AMDS_CLIENT_ERR_INVALID_MESSAGE_TYPE 10100
#define AMDS_CLIENT_INFO_HAND_SHAKE_MESSAGE 10101

class AMDSClientManager : public QObject
{
    Q_OBJECT

public:
    explicit AMDSClientManager(QObject *parent = 0);
	~AMDSClientManager();

	/// helper function to return the server socket identifier of given host name and port number
	inline QString serverTcpSocketIdentifier(QString hostName, qint16 portNumber) { return QString("%1:%2").arg(hostName).arg(portNumber); }
	/// helper function to check whether the network session is openned or not
	bool isSessionOpen();

signals:
	/// signal to indicate that the manager object is opening a network session
	void networkSessionOpening();
	/// signal to indicate that a network session is opened
	void networkSessionOpened();

	/// signal to indicate that something wrong with the connection happened
	void socketError(QString socketKey, QString errorMessage);
	/// signal to indicate that the data is sent from server for read
	void requestDataReady(AMDSClientRequest*);

public slots:
	/// slot to open a network session
	void openNetworkSession();

	/// slot to request a connection to a specific hostName and the portNumber
	AMDSClientTCPSocket * connectToServer(QString hostName, qint16 portNumber);

	/// slot to request data from server for Statistics
	void requestClientData(QString &hostName, qint16 portNumber);
	/// slot to request data from server for Instrospection
	void requestClientData(QString &hostName, qint16 portNumber, QString &bufferName);
	/// slot to request data from server for StartTimePlusCount
	void requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &startTime, quint64 count);
	/// slot to request data from server for RelativeCountPlusCount
	void requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, quint64 relativeCount, quint64 count);
	/// slot to request data from server for StartTimeToEndTime
	void requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &startTime, QDateTime &endTime);
	/// slot to request data from server for MiddleTimePlusCountBeforeAndAfter
	void requestClientData(QString &hostName, qint16 portNumber, QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter);
	/// slot to request data from server for Continuous
	void requestClientData(QString &hostName, qint16 portNumber, QStringList &bufferNames, quint64 updateInterval, QString handShakeSocketKey="");

private slots:
	/// slot to handle the signal of network session opened
	void onNetworkSessionOpened();
	/// slot to handle Socket data ready signal
	void onSocketDataReady(AMDSClientTCPSocket* socket, AMDSClientRequest *clientRequest);
	/// slot to handle Socket error signal
	void onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError socketError);
	/// slot to handle the request to remove a given signal
	void removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket);

private:
	/// the pointer of network session
	QNetworkSession *networkSession_;

	/// the hash table of active server connections
	QHash<QString, AMDSClientTCPSocket *> activeServerSockets_;
};

#endif // AMDSCLIENTMANAGER_H
