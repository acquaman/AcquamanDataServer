#ifndef AMDSCENTRALSERVERSGMSCALER_H
#define AMDSCENTRALSERVERSGMSCALER_H

#include <QObject>

#include "application/AMDSCentralServer.h"
#include "DataHolder/AMDSDataHolder.h"

class AMDSScalerConfigurationMap;
class AMDSThreadedScalerDetector;
class AMDSDetectorServerManager;

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
	void onNewScalerScanDataReceived(const AMDSDataHolderList &scalerScanCountsDataHolder);

	/// Called when the scaler requests flattened data. Sets the latches for internalRequestActive_ and dwellSecondsRequested
	void onScalerDetectorRequestFlattenedData(double seconds);
	/// Handles the internal requests that are processed and send data back to detector
	void onInternalRequestProcessed(AMDSClientRequest *clientRequest);

protected:
	/// function to initialize the system configurations
	void initializeConfiguration();
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup();
	/// function to initialize the detector manager
	virtual void initializeDetectorManager();
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDetectorServer();
	/// function to finalize the initialization
	virtual void wrappingUpInitialization();

	/// helper function to fill the configuration commands for the given bufferName
	void fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest);

protected:
	/// the scaler detector configruation map
	AMDSScalerConfigurationMap *scalerConfigurationMap_;
	/// the Scaler detector manager
	AMDSThreadedScalerDetector *threadedScalerDetector_;
	/// the scaler server manager
	AMDSDetectorServerManager *scalerDetectorServerManager_;

	/// Latches that we have an internal request in processing and handles it differently
	bool internalRequestActive_;
	/// Latches the flattening time in seconds
	double dwellSecondsRequested_;
};

#endif // AMDSCENTRALSERVERSGMSCALER_H
