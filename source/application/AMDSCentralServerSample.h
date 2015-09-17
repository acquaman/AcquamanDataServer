#ifndef AMDSCENTRALSERVER_SAMPLE_H
#define AMDSCENTRALSERVER_SAMPLE_H

#include <QObject>
#include <QMap>

#include "source/application/AMDSCentralServer.h"

class AMDSCentralServerSample : public AMDSCentralServer
{
Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSample(QObject *parent = 0);

protected slots:
	/// slot to handle the 50 ms timer, to fetch and update the scaler data buffer
	void onFiftyMillisecondTimerUpdate();
	/// slot to handle the 100 ms timer, to fetch and update the amptek data buffer
	void onHundredMillisecondTimerUpdate();

protected:
	/// function to initialize the buffer groups, with the given buffer size
	virtual void initializeBufferGroup(quint64 maxCountSize);
	/// function to initialize the timer to update the buffer groups
	virtual void initializeTimer();
	/// function to start the timer of data buffer update
	virtual void startTimer();

protected:
	AMDSBufferGroup *energyBufferGroup_;
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;

	AMDSBufferGroup *amptek1BufferGroup_;
	quint64 spectralCounter_;
	QTimer *hundredMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_SAMPLE_H
