#ifndef AMDSCENTRALSERVER_SAMPLE_H
#define AMDSCENTRALSERVER_SAMPLE_H

#include <QObject>
#include <QMap>

#include "application/AMDSCentralServer.h"

class AMDSCentralServerSample : public AMDSCentralServer
{
Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSample(QObject *parent = 0);
	~AMDSCentralServerSample();

protected slots:
	/// slot to handle the 50 ms timer, to fetch and update the scaler data buffer
	void onFiftyMillisecondTimerUpdate();
	/// slot to handle the 100 ms timer, to fetch and update the amptek data buffer
	void onHundredMillisecondTimerUpdate();

protected:
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup();
	/// function to initialize the detector manager
	virtual void initializeDetectorManager();
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDetectorServer();
	/// function to finalize the initialization
	virtual void wrappingUpInitialization();
	/// pure virtual function to fill the configuration commands for the given bufferName
	virtual void fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest);

protected:
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;

	quint64 spectralCounter_;
	QTimer *hundredMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_SAMPLE_H
