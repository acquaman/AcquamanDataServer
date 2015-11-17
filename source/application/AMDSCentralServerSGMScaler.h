#ifndef AMDSCENTRALSERVERSGM_H
#define AMDSCENTRALSERVERSGM_H

#include <QObject>

#include "application/AMDSCentralServer.h"
#include "DataHolder/AMDSDataHolder.h"

class AMDSScalerConfigurationMap;
class AMDSScalerDetectorManager;

#define AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME 30101

class AMDSCentralServerSGMScaler : public AMDSCentralServer
{
	Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGMScaler(QObject *parent = 0);
	~AMDSCentralServerSGMScaler();

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


	/// slot to handle new scaler data request to add the data to buffergroup
	void onNewScalerScanDataReceivedd(const AMDSDataHolderList &scalerScanCountsDataHolder);

protected:
	/// function to initialize the system configurations
	void initializeConfiguration();
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup();
	/// function to initialize the detector manager
	virtual void initializeDetectorManager();
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDataServer();
	/// function to finalize the initialization
	virtual void wrappingUpInitialization();

protected:
	/// the scaler detector configruation map
	AMDSScalerConfigurationMap *scalerConfigurationMap_;

	/// the Scaler detector manager
	AMDSScalerDetectorManager *scalerDetectorManager_;
};

#endif // AMDSCENTRALSERVERSGM_H
