#ifndef AMDSCENTRALSERVERSGM_H
#define AMDSCENTRALSERVERSGM_H

#include <QObject>
#include <QMap>

#include "source/AMDSCentralServer.h"

class AMDSCentralServerSGM : public AMDSCentralServer
{
Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGM(QObject *parent = 0);

protected slots:
	/// slot to handle the 50 ms timer, to fetch and update the scaler data buffer
	void onFiftyMillisecondTimerUpdate();
	/// slot to handle the 100 ms timer, to fetch and update the amptek data buffer
	void onHundredMillisecondTimerUpdate();

protected:
	/// function to initialize the buffer groups, with the given buffer size
	virtual void initializeBufferGroup(quint64 maxCountSize);
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

#endif // AMDSCENTRALSERVERSGM_H
