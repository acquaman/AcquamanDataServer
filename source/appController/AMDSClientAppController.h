#ifndef AMDSCLIENTAPPCONTROLLER_H
#define AMDSCLIENTAPPCONTROLLER_H

#include <QObject>
#include <QHash>
#include <QDateTime>
#include <QTcpSocket>
#include <QNetworkSession>
#include <QFile>

class AMDSServer;
class AMDSClientTCPSocket;
class AMDSClientRequest;

#include "appController/AMDSAppController.h"
#include "ClientRequest/AMDSClientRequestDefinitions.h"
#include "DataElement/AMDSConfigurationDef.h"

#define AMDS_CLIENT_INFO_HAND_SHAKE_MESSAGE 10100
#define AMDS_CLIENT_INFO_NETWORK_SESSION_STARTED 10101
#define AMDS_CLIENT_ERR_NETWORK_SESSION_CLOSED 10102
#define AMDS_CLIENT_ERR_NETWORK_SESSION_INVALID_STATE 10103
#define AMDS_CLIENT_ERR_NETWORK_SESSION_ERROR 10104

#define AMDS_CLIENT_ERR_INVALID_MESSAGE_TYPE 10200

#define AMDS_CLIENT_ERR_SERVER_NOT_CONNECTED 10300
#define AMDS_CLIENT_ERR_SERVER_DISCONNECTED 10301
#define AMDS_CLIENT_ERR_FAILED_TO_PARSE_CLIENT_MSG 10302
#define AMDS_CLIENT_ERR_FAILED_TO_PARSE_CONTINUOUS_MSG 10303
#define AMDS_CLIENT_ERR_FAILED_TO_PARSE_CONFIGURATION_MSG 10304

#define AMDS_CLIENT_ERR_WRITING_FILE 10305
#define AMDS_CLIENT_ERR_TARGET_EXPORT_FILE_NOT_CONFIGURED 10306
#define AMDS_CLIENT_ERR_TARGET_EXPORT_FILE_EXISTED 10307
#define AMDS_CLIENT_ERR_FAILED_TO_OPEN_TARGET_EXPORT_FILE 10308

class AMDSClientAppController : public AMDSAppController
{
    Q_OBJECT

public:
	static AMDSClientAppController* clientAppController();

	~AMDSClientAppController();

	/// helper function to check whether the network session is openned or not
	bool isSessionOpen();

	/// set the path where the client data request will be written to
	inline void setAMDSExportFilePath(const QString filePath) { currentExportedFilePath_ = filePath; }

	/// helper function to return the server by given identifier
	inline AMDSServer *getServerByServerIdentifier(const QString &serverIdentifier) { return activeServers_.value(serverIdentifier, 0);}

	/// helper function to return the list of buffers defined by a give host server
	QStringList getBufferNamesByServer(const QString &serverIdentifier);
	/// helper function to return the list of socketKeys of active connection by a give host server
	QStringList getActiveSocketKeysByServer(const QString &serverIdentifier);

	/// request to establish a connection to a specific hostName and the portNumber
	void connectToServer(const AMDSServerConfiguration &serverConfiguration);
	/// request to establish a connection to a specific hostName and the portNumber
	void connectToServer(const QString &hostName, quint16 portNumber);
	/// request disconnection with a given server identifier
	void disconnectWithServer(const QString &serverIdentifier);


	/// request data from server for Statistics
	bool requestClientData(const QString &hostName, quint16 portNumber);
	/// request data from server for Instrospection
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName);
	/// request data from server for StartTimePlusCount
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &startTime, quint64 count, bool includeStatus=false, bool enableFlattening=false);
	/// request data from server for RelativeCountPlusCount
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, quint64 relativeCount, quint64 count, bool includeStatus=false, bool enableFlattening=false);
	/// request data from server for StartTimeToEndTime
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &startTime, const QDateTime &endTime, bool includeStatus=false, bool enableFlattening=false);
	/// request data from server for MiddleTimePlusCountBeforeAndAfter
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, const QDateTime &middleTime, quint64 countBefore, quint64 countAfter, bool includeStatus=false, bool enableFlattening=false);
	/// request data from server for Continuous
	bool requestClientData(const QString &hostName, quint16 portNumber, const QStringList &bufferNames, quint64 updateInterval, bool includeStatus=false, bool enableFlattening=false, QString handShakeSocketKey="");
	/// request data from server for Configuration
	bool requestClientData(const QString &hostName, quint16 portNumber, const QString &bufferName, int commandId, const QString &value);

signals:
	/// signal to indicate that the manager object is opening a network session
	void networkSessionOpening();

	/// signal to indicate that the AMDS Client App Controller is ready for service
	void AMDSClientControllerConnected();
	/// signal to indicate that the AMDS Client App Controller is disconnected
	void AMDSClientControllerDisconnected();
	/// signal to indicate that the AMDS Client App Controller network session is closed
	void AMDSClientControllerNetworkSessionClosed(const QString &errorMsg);
	/// signal to indicate that the AMDS Client App Controller is in invalid state
	void AMDSClientControllerError(const QString &errorMsg);
	/// signal to indicate that there are server errors
	void serverError(int errorCode, bool disconnectServer, const QString &hostIdentifier, const QString &errorMessage);

	/// signal to indicate that a new server is connected
	void newServerConnected(const QString &hostIdentifier);
	/// signal to indicate that the data is sent from server for read
	void requestDataReady(AMDSClientRequest*);

public slots:
	/// open a new network session
	void openNetworkSession();

	/// slot to handle socket error signal from the server
	void onAMDSServerError(const QString &serverIdentifier, int errorCode, const QString &socketKey, const QString &errorMessage);

	/// slot to handle the signal to start a new scan
	void onScanStarted();
	/// slot to handle the signal to finish a scan
	void onScanStopped();

protected:
	/// make the constructor protected for SINGLETON usage
	explicit AMDSClientAppController(QObject *parent = 0);

private slots:
	/// slot to handle the signal of network session opened
	void onNetworkSessionOpened();
	/// slot to handle the signal of network session closed
	void onNetworkSessionClosed();
	/// slot to handle the signal of network session state changed
	void onNetworkSessionStateChanged(QNetworkSession::State);
	/// slot to handle the signal of network session error
	void onNetworkSessionError(QNetworkSession::SessionError);
	/// slot to handle the new client request received message from a given server
	void onRequestDataReady(AMDSClientRequest* clientRequest);

private:
	/// establish TCP socket connection to a specific hostName and the portNumber
	AMDSClientTCPSocket * establishSocketConnection(const QString &hostName, quint16 portNumber);

	/// helper function to instantiate client request based on the request type
	AMDSClientRequest *instantiateClientRequest(AMDSClientRequestDefinitions::RequestType clientRequestType);

private:
	/// the pointer of network session
	QNetworkSession *networkSession_;

	/// the hash table of active servers
	QHash<QString, AMDSServer *> activeServers_;

	/// the path where the clientDataRequest will be written to
	QString currentExportedFilePath_;
	/// the handler of the file, which we are going to write the data to
	QFile* clientDataRequestFile_;
};

#endif // AMDSCLIENTAPPCONTROLLER_H
