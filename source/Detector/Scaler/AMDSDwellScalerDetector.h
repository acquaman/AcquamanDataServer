#ifndef AMDSDWELLSCALERDETECTOR_H
#define AMDSDWELLSCALERDETECTOR_H

#include <QObject>
#include <QMap>
#include <QSignalMapper>

#include "Detector/AMDSDwellDetector.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

class QTimer;

class AMControlSet;
class AMSinglePVControl;


/// This class is designed for the softIOC scaler detector
class AMDSDwellScalerDetector : public AMDSDwellDetector
{
	Q_OBJECT

	enum ScalerChannelStatus {
		Disabled = 0,
		Enabled = 1
	};

public:
	explicit AMDSDwellScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	virtual ~AMDSDwellScalerDetector();

	/// returns the name of the scaler
	inline QString scalerName() const { return scalerConfiguration_->scalerName(); }
	/// returns the basePVName of the scaler
	inline QString scalerBasePVName() const { return scalerConfiguration_->scalerBasePVName(); }
	/// returns the number of channels
	inline quint8 enabledChannelCount() const { return scalerConfiguration_->enabledChannels().count(); }

signals:
	/// Signal to request flattened data from the central server
	void requestFlattenedData(double seconds);

public slots:
	/// Called to set the flattened data once the request has been processed
	void setFlattenedData(const QVector<double> &data);

protected slots:
	/// slot to handle the all PV connected signal
	void onAllControlsConnected(bool connected);
	/// slot to handle the PV connected timout signal
	void onAllControlsTimedOut();

	/// Listen to changes on the trigger/dwell start PV
	void onDwellScalerDwellModeControlValueChanged(double value);
	/// Listen to changes on the trigger/dwell start PV
	void onDwellScalerStartControlValueChanged(double value);
	/// Listen to changes on the trigger/dwell dwell time PV
	void onDwellScalerDwellTimeControlValueChanged(double value);

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

	/// helper function to check whether the detetor start data acquiring or not
	bool isDwellStarted() const;
	/// helper function to check whether the detector is working in continuous mode or not
	bool isContinuousDwell() const;
	/// helper function to check whether the detector is acquiring data or not
	bool isAcquiring() const ;

protected:
	/// the dwellTime in second
	double dwellTimeInSecond_;

	/// the configuration of the scaler
	AMDSScalerConfigurationMap *scalerConfiguration_;

	/// Holds the mapping of the value changed signal of enabled channels during an acquisition.
	QSignalMapper *channelValueChangedSignalMapper_;
	/// Timer in control of emitting signal to request flattened data
	QTimer *triggerDwellTimer_;
	/// The list of channels we are still waiting to get their monitor before emitting that the scaler acquisition has finished.
	QList<int> waitingChannels_;


	/// the control set to manager all the controls
	AMControlSet *pvControlSet_;

	/// The feedback controls for the channels of the new trigger/dwell interface
	QMap<int, AMSinglePVControl*> dwellScalerChannelFeedbackControls_;
	/// The enable controls for the channels of the new trigger/dwell interface
	QMap<int, AMSinglePVControl*> dwellScalerChannelEnableControls_;

	/// The start control for the new trigger/dwell interface
	AMSinglePVControl *dwellScalerStartControl_;
	/// The dwell time control for the new trigger/dwell interface
	AMSinglePVControl *dwellScalerDwellTimeControl_;
	/// The dwell status control for the new trigger/dwell interface
	AMSinglePVControl *dwellScalerDwellStateControl_;
	/// The dwell mode control for the new trigger/dwell interface
	AMSinglePVControl *dwellScalerDwellModeControl_;
};

#endif // AMDSDWELLSCALERDETECTOR_H
