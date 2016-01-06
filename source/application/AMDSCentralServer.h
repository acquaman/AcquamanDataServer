#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class AMDSThreadedBufferGroupManager;
class AMDSThreadedTCPDataServer;
class AMDSClientRequest;
class AMDSClientConfigurationRequest;

#define AMDS_SERVER_INFO_START_SERVER_APP 20100
#define AMDS_SERVER_INFO_START_BUFFER_TIMER 20101
#define AMDS_SERVER_INFO_BUFFER_DEF 20102
#define AMDS_SERVER_ALT_INVALID_REQUEST 20103
#define AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME 20104

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
	void configurationRequestReceived(AMDSClientRequest*);

protected slots:
	/// slot to handle the errors information from the TCP Data server
	void onDataServerErrorHandler(quint8 errorLevel, quint16 errorCode, const QString &errorMessage);
	/// slot to handle the client request from the TCP Data server
	void onDataServerClientRequestReady(AMDSClientRequest *clientRequest);

	void onDetectorServerStartDwelling(const QString &bufferName);

protected:
	/// initialize and start the TCPServer to handle clientRequest
	void initializeAndStartTCPServer();

	/// pure virtual function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup() = 0;
	/// pure virtual function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeDetectorManager() = 0;
	/// pure virtual function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDetectorServer() = 0;
	/// pure virtual function to finalize the initialization
	virtual void wrappingUpInitialization() = 0;
	/// pure virtual function to fill the configuration commands for the given bufferName
	virtual void fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest) = 0;

	/// function to process the instrospection client request from the TCP Data server
	void processIntrospectionClientRequest(AMDSClientRequest *clientRequest);
	/// function to process the configuration client request from the TCP Data server
	void processConfigurationClientRequest(AMDSClientRequest *clientRequest);
	/// function to process the client data request from the TCP Data server
	virtual void processClientDataRequest(AMDSClientRequest *clientRequest);


protected:
	/// the max buffer size, default value: 1000*60*60*10 --- 10 hours data collection
	quint64 maxBufferSize_;
	/// the instance of threaded TCP data server
	AMDSThreadedTCPDataServer *tcpDataServer_;

	/// the list of bufferGroup managers
	QMap<QString, AMDSThreadedBufferGroupManager*> bufferGroupManagers_;
};

Q_DECLARE_INTERFACE(AMDSCentralServer, "AMDS.AMDSCentralServer/1.0")

#endif // AMDSCENTRALSERVER_H
