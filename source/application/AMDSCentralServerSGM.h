#ifndef AMDSCENTRALSERVERSGM_H
#define AMDSCENTRALSERVERSGM_H

#include <QObject>

#include "application/AMDSCentralServer.h"

class AmptekSDD123ConfigurationMap;
class AmptekSDD123ThreadedDataServerGroup;
class AmptekSDD123DetectorGroupSGM;
class AMDSDataHolder;

class AMDSCentralServerSGM : public AMDSCentralServer
{
	Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGM(QObject *parent = 0);
	~AMDSCentralServerSGM();

signals:
	/// signal to indicate that the server (at index) switched to configuration state
	void serverChangedToConfigurationState(int index);
	/// signal to indicate that the server (at index) switched to dwell state
	void serverChangedToDwellState(int index);

protected slots:
	/// slot to handle the signal that server switch to configuration state
	void onServerChangedToConfigurationState(int index);
	/// slot to handle the signal that server switch to dwell state
	void onServerChangedToDwellState(int index);

	void onClearHistrogramData(QString detectorName);
	void onClearDwellHistrogramData(QString detectorName);
	void onNewHistrogramReceived(QString detectorName, AMDSDataHolder *);
	void onNewDwellHistrogramReceived(QString detectorName, AMDSDataHolder *);

protected:
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup();
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDataServer();
	/// function to finalize the initialization
	virtual void wrappingUpInitialization();

protected:
	/// the list of configuration map of SGM amptek
	QList<AmptekSDD123ConfigurationMap*> configurationMaps_;

	QMap<QString, AMDSThreadedBufferGroup*> dwellBufferGroupManagers_;


	/// the threaded serverGroup to manage the AmptekServers, which will fetch the UDP data from the amptek host to grab the data
	AmptekSDD123ThreadedDataServerGroup *amptekThreadedDataServerGroup_;

	/// the AmptekDetector group, which will manange and hold the Amptek data and manage the communications with the PVs
	AmptekSDD123DetectorGroupSGM *detectorGroup_;
};

#endif // AMDSCENTRALSERVERSGM_H
