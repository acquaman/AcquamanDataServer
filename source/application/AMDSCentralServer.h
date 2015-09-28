#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class QTimer;

class AMDSBufferGroupManager;
class AMDSThreadedTCPDataServer;
class AMDSBufferGroup;
class AMDSClientRequest;

#define AMDS_SERVER_INFO_START_SERVER_APP 20100
#define AMDS_SERVER_INFO_START_BUFFER_TIMER 20101
#define AMDS_SERVER_INFO_BUFFER_DEF 20102
#define AMDS_SERVER_ALT_INVALID_REQUEST 20103

class AMDSCentralServer : public QObject
{
Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServer(QObject *parent = 0);
	~AMDSCentralServer();

public:
	/// interface to initialize and start the services
	void initializeAndStartServices();

signals:
	/// The signal when the AMDSCenterServer finished processing a client request, then the TCP data suppose to wrap the msg and send back to client
	void clientRequestProcessed(AMDSClientRequest *processedRequest);

protected slots:
	/// slot to handle the errors information from the TCP Data server
	void onDataServerErrorHandler(quint8 errorLevel, quint16 errorCode, const QString &errorMessage);
	/// slot to handle the client request from the TCP Data server
	void onDataServerClientRequestReady(AMDSClientRequest *clientRequest);

protected:
	/// initialize and start the TCPServer to handle clientRequest
	void initializeAndStartTCPServer();

	/// pure virtual function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup(quint64 maxCountSize = 1000*60*60*10) = 0;
	/// pure virtual function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDataServer() = 0;

protected:
	AMDSThreadedTCPDataServer *tcpDataServer_;
	QMap<QString, AMDSBufferGroupManager*> bufferGroupManagers_;
};

Q_DECLARE_INTERFACE(AMDSCentralServer, "AMDS.AMDSCentralServer/1.0")

#endif // AMDSCENTRALSERVER_H
