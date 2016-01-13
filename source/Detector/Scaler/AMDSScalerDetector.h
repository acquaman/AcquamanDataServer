#ifndef AMDSSCALERDETECTOR_H
#define AMDSSCALERDETECTOR_H

#include <QObject>
#include <QMap>
#include <QThread>
#include <QSignalMapper>

#include "DataHolder/AMDSDataHolder.h"
#include "Detector/AMDSDwellDetector.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

class QTimer;

class AMControlSet;
class AMSinglePVControl;
class AMWaveformBinningSinglePVControl;

class AMDSScalerDetector ;

#define AMDS_SCALER_DETECTOR_ALT_PV_NOT_CONNECTED 50101
#define AMDS_SCALER_DETECTOR_ALT_SWITCH_TO_DWELLING  50102
#define AMDS_SCALER_DETECTOR_ALT_SWITCH_TO_CONFIGURATION  50103
#define AMDS_SCALER_DETECTOR_ALT_DWELL_TIME_CHANGED  50104
#define AMDS_SCALER_DETECTOR_ALT_SCAN_IN_BUFFER_CHANGED  50105
#define AMDS_SCALER_DETECTOR_ALT_TOTAL_NUM_SCANS  50106
#define AMDS_SCALER_DETECTOR_ALT_FAILED_TO_CONFIG_CHANNEL  50107


class AMDSThreadedScalerDetector : public QObject
{
	Q_OBJECT
public:
	explicit AMDSThreadedScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	~AMDSThreadedScalerDetector();

	/// returns the instance of scaler detector
	inline AMDSScalerDetector *scalerDetector() const { return scalerDetector_; }

protected:
	/// the instance of detector manager thread
	QThread *detectorThread_;
	/// the instance of scaler detector
	AMDSScalerDetector *scalerDetector_;
};


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
	/// returns the enabled channelIds
	inline QList<quint8> enabledChannelIds() const { return scalerConfiguration_->enabledChannels(); }
	/// returns the number of channels
	inline quint8 enabledChannelCount() const { return enabledChannelIds().count(); }

signals:
	/// signal to indicate that we received new counts data
	void newScalerScanDataReceived(const AMDSDataHolderList &scanCountsDataHolder);

	/// Signal to request flattened data from the central server
	void requestFlattenedData(double seconds);

public slots:
	/// slot to start dwelling
	void onServerGoingToStartDwelling();
	/// slot to stop dwelling
	void onServerStopDwelling();
	/// slot to enable a given channel
	void onEnableChannel(int channelId);
	/// slot to disable a given channel
	void onDisableChannel(int channelId);

	/// Called to set the flattened data once the request has been processed
	void setFlattenedData(const QVector<double> &data);

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

	/// Listen to changes on the trigger/dwell start PV
	void onTriggerDwellInterfaceDwellModeControlValueChanged(double value);
	/// Listen to changes on the trigger/dwell start PV
	void onTriggerDwellInterfaceStartControlValueChanged(double value);
	/// Listen to changes on the trigger/dwell dwell time PV
	void onTriggerDwellInterfaceDwellTimeControlValueChanged(double value);

	/// Called to actually request the flattened data from the server by emitting the signal
	void onTriggerDwellTimerTimeout();

	/// slot to start scaler dwell acquisition
	void startScalerDwellAcquisition();
	/// slot to stop scaler dwell acquisition
	void stopScalerDwellAcquisition(bool forceStop=false);
	/// slot to handle the value changed signal from the scaler channel
	void onScalerChannelValueChanged(int channelIndex);

protected:
	/// helper function to initialize the PV controls of the scaler
	void initializePVControls();

	/// helper function to configuration channel pv
	void configureChannelControl(bool enable, int channelId);

protected:
	/// the configuration of the scaler
	AMDSScalerConfigurationMap *scalerConfiguration_;

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

	/// The start control for the new trigger/dwell interface
	AMSinglePVControl *triggerDwellInterfaceStartControl_;
	/// The dwell time control for the new trigger/dwell interface
	AMSinglePVControl *triggerDwellInterfaceDwellTimeControl_;
	/// The dwell status control for the new trigger/dwell interface
	AMSinglePVControl *triggerDwellInterfaceDwellStateControl_;
	/// The dwell mode control for the new trigger/dwell interface
	AMSinglePVControl *triggerDwellInterfaceDwellModeControl_;

	/// The feedback controls for the channels of the new trigger/dwell interface
	QMap<int, AMSinglePVControl*> triggerDwellInterfaceChannelFeedbackControls_;

	/// The enable controls for the channels of the new trigger/dwell interface
	QMap<int, AMSinglePVControl*> triggerDwellInterfaceChannelEnableControls_;

        /// the dwell time to read the scaler buffer (in ms, default 1ms)
	int defaultDwellTime_;
	int dwellTime_;
	/// the number of scans in a single buffer (default 1000)
	int defaultScansInABuffer_;
	int scansInABuffer_;
	/// the total number of scans (default 0)
	int defaultTotalNumberOfScans_;
	int totalNumberOfScans_;

	/// The list of channels we are still waiting to get their monitor before emitting that the scaler acquisition has finished.
	QList<int> waitingChannels_;
	/// Holds the mapping of the value changed signal of enabled channels during an acquisition.
	QSignalMapper *channelValueChangedSignalMapper_;
	/// Timer in control of emitting signal to request flattened data
	QTimer *triggerDwellTimer_;
};

#endif // AMDSSCALERDETECTOR_H
