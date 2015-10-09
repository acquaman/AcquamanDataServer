#ifndef AMPTEKSDD123DETECTORMANAGER_H
#define AMPTEKSDD123DETECTORMANAGER_H

#include <QObject>

#include "Detector/Amptek/AmptekEventDefinitions.h"
#include "Detector/Amptek/AmptekSDD123Detector.h"

class AMDSDataHolder;
class AmptekSDD123Detector;
class AmptekSDD123ConfigurationMap;

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
	void clearHistrogramData(QString detectorName);
	/// signal to request clear dwell histrogram data
	void clearDwellHistrogramData(QString detectorName);
	/// signal to indicate new histrogram data
	void newHistrogramReceived(QString detectorName, AMDSDataHolder *);
	/// signal to indicate new dwell histrogram data
	void newDwellHistrogramReceived(QString detectorName, AMDSDataHolder * dataHolder, double elapsedDwellTime);
	/// signal to indicate dwell finished updating data
	void dwellFinishedUpdate(QString detectorName, double elapsedTime);

	/// signal to indicate that configuration value is updated
	void configurationValuesUpdate(AmptekConfigurationData configurationData);

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

	void setDetectorConfigurationValues();
	void setDetectorAnalogGainIndex(int analogGainIndex);
	void setDetectorFineGain(double fineGain);
	void setDetectorTotalGain(double totalGain);
	void setDetectorHV(bool on);
	void setDetectorMCAEnabled(bool enabled);
	void setDetectorPileUpRejection(bool enabled);
	void setDetectorCoolerSetting(int coolerSetting);
	void setDetectorSingleChannelAnalyzer(int singleChannelAnalyzerIndex, int lowChannelIndex, int highChannelIndex);
	void setDetectorFastThreshold(int fastThreshold);
	void setDetectorSlowThreshold(double slowThreshold);
	void setDetectorPeakingTime(double peakingTime);
	void setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTimeValue fastPeakingTimeValue);

protected slots:
	/// slot to handle continuous data update
	virtual void onContinuousAllDataUpdate(AMDSDataHolder *spectrum, AMDSStatusData statusData, int count, double elapsedTime) = 0;
	/// slot to handle dwell data update
	virtual void onDwellFinishedAllDataUpdate(AMDSDataHolder *spectrum, AMDSStatusData statusData, int count, double elapsedTime) = 0;

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
	AmptekSDD123Detector *detector_;
	AmptekConfigurationData configurationData_;

	QObject *requestEventReceiver_;

	int dwellTime_;
	bool dwellActive_;
	bool setPresetDwellEndTimeOnNextEvent_;

	QTime lastestDwellEndTime_;

	QTime presetDwellEndTime_;
	QTime presetDwellLocalStartTime_;
	QTime presetDwellLocalEndTime_;
	AmptekSDD123DetectorManager::DwellMode dwellMode_;

	int maxHistogramStackSize_;

	bool presetDwellActive_;

	bool initialized_;

	AmptekSDD123DetectorManager::ConfigurationModeReason configurationRequestReason_;
	QString configurationSetCommand_;
};

#endif // AMPTEKSDD123DETECTORMANAGER_H
