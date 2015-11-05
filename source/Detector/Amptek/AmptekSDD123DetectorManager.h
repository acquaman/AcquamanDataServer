#ifndef AMPTEKSDD123DETECTORMANAGER_H
#define AMPTEKSDD123DETECTORMANAGER_H

#include <QObject>

#include "Detector/Amptek/AmptekSDD123Detector.h"

class AMDSDataHolder;
class AmptekSDD123Detector;
class AmptekSDD123ConfigurationMap;

#define DETECTOR_MANAGER_ALERT_EVENT_RECEIVER_NO_INITIALIZED 30100


class AmptekSDD123DetectorManager : public QObject
{
	Q_OBJECT
public:
	enum DwellMode{
		ContinuousDwell = 0,
		PresetDwell = 1
	};

	enum ConfigurationModeReason{
		RequestConfigurationReason = 0,
		SetConfigurationReason = 1,
		InvalidReason
	};

	enum FastPeakingTimeValue{
		FastPeakingTime50 = 0,
		FastPeakingTime100 = 1,
		FastPeakingTime400 = 2
	};

	AmptekSDD123DetectorManager(AmptekSDD123ConfigurationMap *amptekConfiguration, QObject *parent = 0);
	~AmptekSDD123DetectorManager();

	/// function to handle QEvent
	bool event(QEvent *e);

	/// helper function to return the name of the Amptek detector
	inline QString detectorName() const { return detector_->name(); }
	/// helper function to return the detector of the current manager
	inline AmptekSDD123Detector *detector() { return detector_; }
	/// helper function to return the current dwell time
	inline double dwellTime() const { return dwellTime_; }
	/// helper function to return whether it is presetDwell now
	inline bool isPresetDwell() { return dwellMode_ == AmptekSDD123DetectorManager::PresetDwell; }
	/// helper function to return whether it is continuousDwell now
	inline bool isContinuousDwell() { return dwellMode_ == AmptekSDD123DetectorManager::ContinuousDwell; }

	/// helper function to return the current dwell mode
	inline AmptekSDD123DetectorManager::DwellMode dwellMode() const { return dwellMode_; }

signals:
	/// signal to request clear histrogram data
	void clearHistrogramData(const QString &detectorName);
	/// signal to request clear dwell histrogram data
	void clearDwellHistrogramData(const QString &detectorName);
	/// signal to indicate new histrogram data
	void newHistrogramReceived(const QString &detectorName, const AMDSDataHolder *);
	/// signal to indicate new dwell histrogram data
	void newDwellHistrogramReceived(const QString &detectorName, AMDSDataHolder * dataHolder, double elapsedDwellTime);
	/// signal to indicate dwell finished updating data
	void dwellFinishedUpdate(const QString &detectorName, double elapsedTime);

	/// signal to indicate that configuration value is updated
	void configurationValuesUpdate(const AmptekConfigurationData &configurationData);

public slots:
	/// function to set the event receiver to handle the request
	void setRequestEventReceiver(QObject *requestEventReceiver);

	/// function to start dwell
	void startDwell();
	/// function to stop dwell
	void stopDwell();

	/// function to active dwell
	void setDwellActive(bool dwellActive);
	/// function to set Dwell Time
	void setDwellTime(int dwellTime);
	/// function to set dwell mode
	void setDwellMode(AmptekSDD123DetectorManager::DwellMode dwellMode);

	/// function to request AmptekDetector configuration values
	void requestDetectorConfigurationValues();

	/// slot to request set detector configuration
	void setDetectorConfigurationValues();
	/// slot to request set detector analog gain index
	void setDetectorAnalogGainIndex(int analogGainIndex);
	/// slot to request set detector fine gain
	void setDetectorFineGain(double fineGain);
	/// slot to request set detector total gain
	void setDetectorTotalGain(double totalGain);
	/// slot to request turn on/off detector HV
	void setDetectorHV(bool on);
	/// slot to request enable/disable detector MCA
	void setDetectorMCAEnabled(bool enabled);
	/// slot to request enable/disable detector pile up rejection
	void setDetectorPileUpRejection(bool enabled);
	/// slot to request set detector coolerSetting
	void setDetectorCoolerSetting(int coolerSetting);
	/// slot to request set detector single channel analyzer
	void setDetectorSingleChannelAnalyzer(int singleChannelAnalyzerIndex, int lowChannelIndex, int highChannelIndex);
	/// slot to request set detector fast threshold
	void setDetectorFastThreshold(int fastThreshold);
	/// slot to request set detector slow threshold
	void setDetectorSlowThreshold(double slowThreshold);
	/// slot to request set detector pearking time
	void setDetectorPeakingTime(double peakingTime);
	/// slot to request set detector fast pearking time
	void setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTimeValue fastPeakingTimeValue);

protected slots:
	/// slot to handle continuous data update
	virtual void onContinuousAllDataUpdate(AMDSDataHolder *spectrum, const AMDSDwellStatusData &statusData, int count, double elapsedTime) = 0;
	/// slot to handle dwell data update
	virtual void onDwellFinishedAllDataUpdate(AMDSDataHolder *spectrum, const AMDSDwellStatusData &statusData, int count, double elapsedTime) = 0;

protected:
	/// helper function to handle the incoming SpectrumEvent
	void onSpectrumEventReceived(AmptekSpectrumEvent *spectrumEvent);
	/// helper function to handle the incoming ConfigurationValuesEvent
	void onConfigurationValuesEventReceived(AmptekConfigurationValuesEvent *configurationValueEvent);
	/// helper function to handle the incoming ConfigurationModeConfirmationEvent
	void onConfigurationModeConfirmationEventReceived(AmptekConfigurationModeConfirmationEvent *configurationModeConfirmationEvent);

	/// helper function to post the DwellRequestEvent
	void postDwellRequestEvent();
	/// helper function to post the ConfigurationInitiateRequestEvent
	void postConfigurationInitiateRequestEvent();
	/// helper function to post the ConfigurationRequestEvent
	void postConfigurationRequestEvent();
	/// helper function to post the ConfigurationSetEvent
	void postConfigurationSetEvent();

protected:
	/// the instance of the Amptek detector
	AmptekSDD123Detector *detector_;
	/// the QEvent receiver
	QObject *requestEventReceiver_;

	/// flag to indicate whether the data is initialized or not (whether the coming package is the first package)
	bool initialized_;
	/// the default dwell time
	int dwellTime_;
	/// the current dwell mode
	AmptekSDD123DetectorManager::DwellMode dwellMode_;
	/// flag to indicate whether dwell is actived or not
	bool dwellActive_;
	/// flag to indicate whether to end preset dwell on next event
	bool setPresetDwellEndTimeOnNextEvent_;
	/// the expected preset dwell end time
	QTime presetDwellEndTime_;
	/// the start time of presetdwell
	QTime presetDwellLocalStartTime_;
	/// the preset dwell end time of the latest event
	QTime presetDwellLocalEndTime_;

	/// the configuration data received via configuration value event
	AmptekConfigurationData configurationData_;
	/// the current reason of configuration
	AmptekSDD123DetectorManager::ConfigurationModeReason configurationRequestReason_;
	/// the current command of setting configuration
	QString configurationSetCommand_;
};

#endif // AMPTEKSDD123DETECTORMANAGER_H
