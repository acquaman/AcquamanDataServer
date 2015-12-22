#ifndef AMDSCENTRALSERVERSGMAMPTEK_H
#define AMDSCENTRALSERVERSGMAMPTEK_H

#include <QObject>

#include "application/AMDSCentralServer.h"
#include "DataHolder/AMDSDataHolder.h"

class AmptekSDD123ConfigurationMap;
class AmptekSDD123ThreadedDataServerGroup;
class AmptekSDD123DetectorGroupSGM;

class AMDSScalerConfigurationMap;
class AMDSScalerDetectorManager;
class AMDSDetectorServerManager;

class AMDSCentralServerSGMAmptek : public AMDSCentralServer
{
	Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGMAmptek(QObject *parent = 0);
	~AMDSCentralServerSGMAmptek();

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

	/// slot to handle clear histrogramData request for a given buffer (detector)
	void onClearHistrogramData(const QString &detectorName);
	/// slot to handle new histrogramData request  for a given buffer (detector)
	void onNewHistrogramReceived(const QString &detectorName, AMDSDataHolder *);

	void onDwellStarted(const QString &detectorName);
	void onDwellStopped(const QString &detectorName);

	/// Called when the scaler requests flattened data. Sets the latches for internalRequestActive_ and dwellSecondsRequested
	void onAmptekDetectorRequestFlattenedData(const QString &detectorName, double seconds);
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
	/// the list of configuration map of SGM amptek
	QList<AmptekSDD123ConfigurationMap*> amptekConfigurationMaps_;

	/// the threaded serverGroup to manage the AmptekServers, which will fetch the UDP data from the amptek host to grab the data
	AmptekSDD123ThreadedDataServerGroup *amptekThreadedDataServerGroup_;

	/// the AmptekDetector group, which will manange and hold the Amptek data and manage the communications with the PVs
	AmptekSDD123DetectorGroupSGM *amptekDetectorGroup_;

	/// Latches the flattening time in seconds
	QMap<QString, double> dwellSecondsRequestedForDetector_;
};

#endif // AMDSCENTRALSERVERSGMAMPTEK_H
