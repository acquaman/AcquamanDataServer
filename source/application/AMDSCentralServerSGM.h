#ifndef AMDSCENTRALSERVERSGM_H
#define AMDSCENTRALSERVERSGM_H

#include <QObject>
#include <QMap>

#include "application/AMDSCentralServer.h"

class AmptekSDD123ConfigurationMap;
class AmptekSDD123ServerGroup;

class AMDSCentralServerSGM : public AMDSCentralServer
{
Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGM(QObject *parent = 0);
	~AMDSCentralServerSGM();

signals:
	void serverChangedToConfigurationState(int index);
	void serverChangedToDwellState(int index);

protected slots:
	void onServerChangedToConfigurationState(int index);
	void onServerChangedToDwellState(int index);

//	/// slot to handle the 50 ms timer, to fetch and update the scaler data buffer
//	void onFiftyMillisecondTimerUpdate();
//	/// slot to handle the 100 ms timer, to fetch and update the amptek data buffer
//	void onHundredMillisecondTimerUpdate();

protected:
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup(quint64 maxCountSize = 1000*60*60*10);
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDataServer();

	/// helper function to connect the qEvent between BufferGroup and DataServer to update data
	void connectBufferGroupAndDataServer();

protected:
	/// the list of configuration map of SGM amptek
	QList<AmptekSDD123ConfigurationMap*> configurationMaps_;

	/// the QThread of amptek data server, which will fetch the UDP data from the amptek host to grab the data
	QThread *amptekDataServerGroupThread_;
	/// the serverGroup to manage the AmptekServer
	AmptekSDD123ServerGroup *serverGroup_;

	/// the AmptekDetector group, which will manange and hold the Amptek data and manage the communications with the PVs
//	AmptekSDD123DetectorGroup *detectorGroup_;



//	AMDSBufferGroup *energyBufferGroup_;
//	quint64 simpleCounter_;
//	QTimer *fiftyMillisecondTimer_;

//	AMDSBufferGroup *amptek1BufferGroup_;
//	quint64 spectralCounter_;
//	QTimer *hundredMillisecondTimer_;
};

#endif // AMDSCENTRALSERVERSGM_H
