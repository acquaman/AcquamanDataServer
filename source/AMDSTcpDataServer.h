#ifndef AMDSTCPDATASERVER_H
#define AMDSTCPDATASERVER_H

#include <QObject>
#include <QNetworkSession>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QSignalMapper>
#include <QDebug>
#include <QStringList>
#include <QNetworkConfigurationManager>
#include <QSettings>

#include "source/AMDSPacketStats.h"

class QTimer;
class AMDSClientRequest;

/**
 *  A class which handles incoming client connections, and handles requests from them for data
 */
class AMDSTcpDataServer : public QObject
{
	Q_OBJECT
public:
	/// Creates a new instance of a data server. Server will not start listening until a call to
	/// start() is made
	explicit AMDSTcpDataServer(QObject *parent = 0);
	/// Default destructor for AMDSTcpDataServer. Calls stop
	~AMDSTcpDataServer();

signals:
	void requestInfo();
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
	/// Slot which handles the timeout which signals a continuous data request needs a new aggregate of data.
	void onContinuousDataRequestTimer(const QString& clientKey);

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
//	QHash<QString, ClientDataRequest*> continuousDataRequests_;
	/// A signal mapper which allows for us to listen to the disonnect signal from all the client sockets
	/// currently connected. Mapped on the same string as the clientSockets_ hash:
	/// xx.xx.xx.xx:xxxxx
	QSignalMapper* clientDisconnectSignalMapper_;
	/// A signal mapper which allows us to listen to a readyRead signal from all the client sockets currently
	/// connected. Mapped on the same string as the clientSockets_ hash:
	/// xx.xx.xx.xx:xxxxx
	QSignalMapper* clientRequestSignalMapper_;

	QSignalMapper* continuousDataRequestSignalMapper_;

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
