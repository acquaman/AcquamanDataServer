#ifndef AMDSSCALERDETECTOR_H
#define AMDSSCALERDETECTOR_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QSignalMapper>

#include "DataHolder/AMDSDataHolder.h"
#include "Detector/AMDSDwellDetector.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

class AMControlSet;
class AMSinglePVControl;
class AMWaveformBinningSinglePVControl;


/// This class is designed for the physical scaler detector
class AMDSScalerDetector : public AMDSDwellDetector
{
	Q_OBJECT

	enum ScalerChannelStatus {
		Disabled = 0,
		Enabled = 1
	};

public:
	explicit AMDSScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	virtual ~AMDSScalerDetector();

	/// returns the name of the scaler
	inline QString scalerName() const { return scalerConfiguration_->scalerName(); }
	/// returns the basePVName of the scaler
	inline QString scalerBasePVName() const { return scalerConfiguration_->scalerBasePVName(); }
	/// returns the number of channels
	inline quint8 enabledChannelCount() const { return scalerConfiguration_->enabledChannels().count(); }

signals:
	/// signal to indicate that we received new counts data
	void newScalerScanDataReceived(const AMDSDataHolderList &scanCountsDataHolder);

public slots:
	/// slot to start dwelling
	void onStartDwelling();
	/// slot to stop dwelling
	void onStopDwelling();
	/// slot to enable a given channel
	void onEnableChannel(int channelId);
	/// slot to disable a given channel
	void onDisableChannel(int channelId);

protected slots:
	/// slot to handle the all PV connected signal
	void onAllControlsConnected(bool connected);
	/// slot to handle the PV connected timout signal
	void onAllControlsTimedOut();

	/// slot to handle the value changed singale for channel controls
	void onChannelControlValueChanged(double);

	/// slot to response to the value change of scan control
	void onScanControlValueChanged(double);
	/// slot to update the timer for dwell time
	void onDwellTimeControlValueChanged(double);
	/// slot to update the # of scans in a buffer
	void onScansInABufferControlValueChanged(double);
	/// slot to update the total # of scans
	void onTotalNumberOfScansControlValueChanged(double);
	/// slot to fetch the channel data buffer from the scaler scan control
	void onFetchScanBuffer();

protected:
	/// helper function to initialize the PV controls of the scaler
	void initializePVControls();

	/// helper function to configuration channel pv
	void configureChannelControl(bool enable, int channelId);

protected:
	/// the configuration of the scaler
	AMDSScalerConfigurationMap *scalerConfiguration_;

	/// the dwell time to read the scaler buffer (in ms, default 1ms)
	int dwellTime_;
	/// the number of scans in a single buffer (default 100)
	int scansInABuffer_;
	/// the total number of scans (default 0)
	int totalNumberOfScans_;

	/// the control set to manager all the controls
	AMControlSet *pvControlSet_;
	/// the set of PV controls for channel control, to enable/disable the channel
	QMap<int, AMSinglePVControl*> configuredChannelControlSet_;

	/// the PV control to access to the scaler scan Control (scanning / stopped )
	AMSinglePVControl *scanControl_;
	/// the PV control to access to the dwellTimeControl
	AMSinglePVControl *dwellTimeControl_;
	/// the PV control to access to the number of scans in a single buffer Control
	AMSinglePVControl *scansInABufferControl_;
	/// the PV control to access to the total number of scans Control
	AMSinglePVControl *totalNumberOfScansControl_;

	/// the pv control to access the channel scan array
	AMWaveformBinningSinglePVControl *scanBufferControl_;
};


#endif // AMDSSCALERDETECTOR_H
