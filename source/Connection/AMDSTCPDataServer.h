#ifndef AMDSTCPDATASERVER_H
#define AMDSTCPDATASERVER_H

#include <QObject>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QSignalMapper>
#include <QStringList>
#include <QNetworkConfigurationManager>
#include <QSettings>

#include "Connection/AMDSPacketStats.h"

class QTimer;
class AMDSClientRequest;

/**
 *  A class which handles incoming client connections, and handles requests from them for data
 */
#define AMDS_SERVER_ERR_TCP_DATA_SERVER 20400
#define AMDS_SERVER_INFO_STARTING_TCP_DATA_SERVER 20401
#define AMDS_SERVER_INFO_NO_CONNECTED_CLIENT 20402
#define AMDS_SERVER_INFO_ACTIVE_SESSION 20403
#define AMDS_SERVER_INFO_SERVER_STOPPED 20404
#define AMDS_SERVER_INFO_SERVER_NEW_CONNECTION_ESTABLISHED 20405
#define AMDS_SERVER_INFO_SERVER_CONNECTION_DISCONNECTED 20406
#define AMDS_SERVER_INFO_SERVER_HAND_SHAKE_MSG 20407
#define AMDS_SERVER_INFO_SERVER_HAND_SHAKE_MSG_CONNECTION_NOT_FOUND 20408
#define AMDS_SERVER_ALT_ALIVE_CONNECTION 20420
#define AMDS_SERVER_ALT_SERVER_IS_ALIVE 20421
#define AMDS_SERVER_ALT_SERVER_IS_NOT_ALIVE 20422
#define AMDS_SERVER_ERR_FAIL_TO_START_TCP_DATA_SERVER 20440

class AMDSTCPDataServer : public QObject
{
	Q_OBJECT
public:
	/// Creates a new instance of a data server. Server will not start listening until a call to
	/// start() is made
	explicit AMDSTCPDataServer(QObject *parent = 0);
	/// Default destructor for AMDSTcpDataServer. Calls stop
	~AMDSTCPDataServer();

signals:
	/// error signal
	void error(quint8 errorLevel, quint16 errorCode, const QString &errorString);

	/// the signal of new client request read from socket
	void clientRequestRead(AMDSClientRequest*);

public slots:
	/// Outputs to console a list of the currently connected clients' IP and ports. If no clients are connected
	/// then displays "No clients connected"
	void displayClients();
	/// Outputs the current IP and port the server is listening on. If the server is not currently listening
	/// then displays "Server not currently listening"
	void displayDetails();
	/// Starts the server. Server will listen on the given interface and port
	void start(const QString& interfaceName, quint16 port);
	/// Stops the server. Informs current clients that they have been disconnected,
	/// deletes socket objects, and stops the server listening on the given port
	void stop();
	/// Slot which handles a data request being passed back as ready. Uses the information contained
	/// within data to respond to the request
	void onClientRequestProcessed(AMDSClientRequest *processedRequest);

protected slots:
	/// to disconnect the given connection from the host
	void disconnectFromHost(QString &socketKey);

	/// Slot which sets the server to be listening. Automatically called from within the start() function, or
	/// if a session is required, when the session is opened
	void sessionOpened();
	/// Slot which handles a new client connection. Adds the client socket to the socket map,  and wires up
	/// its disconnect signal through, the signal mapper, to onClientDisconnect
	void onNewClientConnected();
	/// Slot which handles a client disconnect. Removes the client socket from the socket map, outputs the
	/// information to console, then marks the socket object for deletion
	void onClientDisconnect(const QString& clientKey);
	/// Slot which handles a client sending a request to the server. Builds a ClientRequest* from the provided data,
	/// if the request is well formed, and emits requestData()
	void onClientSentRequest(const QString& clientKey);
	/// Slot to handle the newly received statics client request message
	void onClientStaticsRequestReceived(AMDSClientRequest *clientRequest);
	/// Slot to handle the newly received continuous client request message, return true if the message is done handle
	void onClientContinuousRequestReceived(AMDSClientRequest *clientRequest);
	/// Slot to handle client request task accomplished, the related resouces can be released
	void onClientRequestTaskAccomplished(AMDSClientRequest *clientRequest);

	/// Slot to handle the timeout signals of statistics timers
	void onTenMillisecondStatsTimerTimeout();
	void onHundredMillisecondStatsTimerTimeout();
	void onOneSecondStatsTimerTimeout();
	void onTenSecondStatsTimerTimeout();

protected:
	/// The name of the interface on which the server will attempt to listen
	QString interfaceName_;
	/// The port number on which the server will attempt to listen
	quint16 port_;
	/// The network session which will be used if the network environment requires one
	QNetworkSession* session_;
	/// The server object which will handle incoming connects
	QTcpServer* server_;
	/// A map storing the currently connected clients, mapped by ip_address:port in the format:
	/// xx.xx.xx.xx:xxxxx
	QHash<QString, QTcpSocket*> clientSockets_;
	/// A map storing the current data requests from a client that have not yet been full processed. Maps
	/// the ip_address:port in the format xx.xx.xx.xx:xxxxx to the size of the request
	QHash<QString, int> clientSocketDataInProgress_;
	/// A map storing continuous data requests from clients
	QHash<QString, AMDSClientRequest*> activeContinuousDataRequestList_;

	/// A signal mapper which allows for us to listen to the disonnect signal from all the client sockets
	/// currently connected. Mapped on the same string as the clientSockets_ hash:
	/// xx.xx.xx.xx:xxxxx
	QSignalMapper* clientDisconnectSignalMapper_;
	/// A signal mapper which allows us to listen to a readyRead signal from all the client sockets currently
	/// connected. Mapped on the same string as the clientSockets_ hash:
	/// xx.xx.xx.xx:xxxxx
	QSignalMapper* clientRequestSignalMapper_;

	QTimer *tenMillisecondStatsTimer_;
	QTimer *hundredMillisecondStatsTimer_;
	QTimer *oneSecondStatsTimer_;
	QTimer *tenSecondStatsTimer_;

	AMDSPacketStats tenMillisecondsStats_;
	AMDSPacketStats hundredMillisecondsStats_;
	AMDSPacketStats oneSecondsStats_;
	AMDSPacketStats tenSecondsStats_;
};

#endif // AMDSTCPDATASERVER_H
