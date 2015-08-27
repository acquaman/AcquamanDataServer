#ifndef AMDSCLIENTAPPCONTROLLER_H
#define AMDSCLIENTAPPCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QTcpSocket>

class QNetworkSession;

class AMDSServer;
class AMDSClientTCPSocket;
class AMDSClientRequest;

#define AMDS_CLIENT_ERR_INVALID_MESSAGE_TYPE 10100
#define AMDS_CLIENT_INFO_HAND_SHAKE_MESSAGE 10101

#define AMDS_CLIENT_ERR_SERVER_NOT_CONNECTED 10201

class AMDSClientAppController : public QObject
{
    Q_OBJECT

public:
	explicit AMDSClientAppController(QObject *parent = 0);
	~AMDSClientAppController();

	/// helper function to check whether the network session is openned or not
	bool isSessionOpen();

	/// helper function to return the server by given identifier
	inline AMDSServer *getServerByServerIdentifier(QString serverIdentifier) { return activeServers_.value(serverIdentifier, 0);}
	/// helper function to return the list of buffers defined by a give host server
	QStringList getBufferNamesByServer(QString serverIdentifier);
	/// helper function to return the list of socketKeys of active connection by a give host server
	QStringList getActiveSocketKeysByServer(QString serverIdentifier);

signals:
	/// signal to indicate that the manager object is opening a network session
	void networkSessionOpening();
	/// signal to indicate that a network session is opened
	void networkSessionOpened();

	/// signal to indicate that a new server is connected
	void newServerConnected(QString hostIdentifier);
	/// signal to indicate that there are server errors
	void serverError(int errorCode, QString hostIdentifier, QString errorMessage);

	/// signal to indicate that something wrong with the connection happened
	void socketError(int errorCode, QString socketKey, QString errorMessage);
	/// signal to indicate that the data is sent from server for read
	void requestDataReady(AMDSClientRequest*);

public slots:
	/// slot to open a network session
	void openNetworkSession();

	/// slot to request a connection to a specific hostName and the portNumber
	void connectToServer(QString hostName, quint16 portNumber);
	/// slot to establish TCP socket connection to a specific hostName and the portNumber
	AMDSClientTCPSocket * establishSocketConnection(QString hostName, quint16 portNumber);

	/// slot to request data from server for Statistics
	void requestClientData(QString &hostName, quint16 portNumber);
	/// slot to request data from server for Instrospection
	void requestClientData(QString &hostName, quint16 portNumber, QString &bufferName);
	/// slot to request data from server for StartTimePlusCount
	void requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, quint64 count, bool includeStatus=false, bool enableFlattening=false);
	/// slot to request data from server for RelativeCountPlusCount
	void requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, quint64 relativeCount, quint64 count, bool includeStatus=false, bool enableFlattening=false);
	/// slot to request data from server for StartTimeToEndTime
	void requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &startTime, QDateTime &endTime, bool includeStatus=false, bool enableFlattening=false);
	/// slot to request data from server for MiddleTimePlusCountBeforeAndAfter
	void requestClientData(QString &hostName, quint16 portNumber, QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter, bool includeStatus=false, bool enableFlattening=false);
	/// slot to request data from server for Continuous
	void requestClientData(QString &hostName, quint16 portNumber, QStringList &bufferNames, quint64 updateInterval, bool includeStatus=false, bool enableFlattening=false, QString handShakeSocketKey="");

private slots:
	/// slot to handle the signal of network session opened
	void onNetworkSessionOpened();

private:
	/// the pointer of network session
	QNetworkSession *networkSession_;

	/// the hash table of active servers
	QHash<QString, AMDSServer *> activeServers_;
};

#endif // AMDSCLIENTAPPCONTROLLER_H
