#ifndef AMPTEKSDD123EPICSDETECTORMANAGER_H
#define AMPTEKSDD123EPICSDETECTORMANAGER_H

#include "Detector/Amptek/AmptekSDD123DetectorManager.h"

class AMWaveformBinningSinglePVControl;
class AMSinglePVControl;
class AMControlSet;

class AMDSDwellStatusData;
class AMDSDataHolder;

class AmptekSDD123EPICSDetectorManager : public AmptekSDD123DetectorManager
{
Q_OBJECT
public:
	AmptekSDD123EPICSDetectorManager(AmptekSDD123ConfigurationMap *amptekConfiguration, QObject *parent = 0);
	~AmptekSDD123EPICSDetectorManager();

public slots:
	/// Called to set the flattened data once the request has been processed
	virtual void setFlattenedData(AMDSDataHolder *dataHolder);

protected slots:
	/// slot to handle continuous data update
	virtual void onContinuousDwellDataUpdate(AMDSDataHolder *dwellSpectrum, int count, double elapsedTime) ;
	/// slot to handle dwell finish data update
	virtual void onFinalDwellDataUpdate(AMDSDataHolder *dwellSpectrum, int count, double elapsedTime);
	/// slot to handle configuration value update signal
	void onConfigurationValuesUpdate(const AmptekConfigurationData &configurationData);

	void onSpectrumControlValueChanged();

	void onStartDwellControlValueChange(double newValue);
	void onStopDwellControlValueChange(double newValue);
	void onClearSpectrumControlValueChange(double newValue);
	void onDwellTimeControlValueChange(double newValue);
	void onDwellModeControlValueChange(double newValue);

	void onHighVoltageControlValueChanged(double newValue);
	void onTotalGainControlValueChanged(double newValue);
	void onPileUpRejectionControlValueChanged(double newValue);
	void onThermoelectricCoolerControlValueChanged(double newValue);
	void onFastThresholdControlValueChanged(double newValue);
	void onSlowThresholdControlValueChanged(double newValue);
	void onPeakingTimeControlValueChanged(double newValue);
	void onFastChannelPeakingTimeControlValueChanged(double newValue);
	void onROI1LowIndexControlValueChanged(double newValue);
	void onROI1HighIndexControlValueChanged(double newValue);
	void onROI2LowIndexControlValueChanged(double newValue);
	void onROI2HighIndexControlValueChanged(double newValue);
	void onROI3LowIndexControlValueChanged(double newValue);
	void onROI3HighIndexControlValueChanged(double newValue);
	void onROI4LowIndexControlValueChanged(double newValue);
	void onROI4HighIndexControlValueChanged(double newValue);
	void onROI5LowIndexControlValueChanged(double newValue);
	void onROI5HighIndexControlValueChanged(double newValue);
	void onROI6LowIndexControlValueChanged(double newValue);
	void onROI6HighIndexControlValueChanged(double newValue);
	void onROI7LowIndexControlValueChanged(double newValue);
	void onROI7HighIndexControlValueChanged(double newValue);
	void onROI8LowIndexControlValueChanged(double newValue);
	void onROI8HighIndexControlValueChanged(double newValue);

	void onAllControlsConnected(bool connected);
	void onAllControlsTimedOut();

	/// Called to actually request the flattened data from the server by emitting the signal
	virtual void onTriggerDwellTimerTimeout();

protected:
	/// helper function to handle continuous spectrum data update
	void dataHelper(AMDSDataHolder *spectrum, int count, double elapsedTime);

protected:
	QTime lastEPICSSpectrumUpdateTime_;
	int EPICSSpectrumUpdateMSecs_;
	bool connected_;
	bool receivedConfigurationDataOnce_;

	AMControlSet *allControls_;

	AMSinglePVControl *startDwellControl_;
	AMSinglePVControl *stopDwellControl_;
	AMSinglePVControl *clearSpectrumControl_;
	AMSinglePVControl *dwellTimeControl_;
	AMSinglePVControl *dwellModeControl_;
	AMSinglePVControl *dwellStateControl_;
	AMSinglePVControl *isAvailableControl_;

	AMWaveformBinningSinglePVControl *spectrumControl_;

	AMSinglePVControl *fastCountsControl_;
	AMSinglePVControl *fastCountsAverageControl_;
	AMSinglePVControl *slowCountsControl_;
	AMSinglePVControl *slowCountsAverageControl_;
	AMSinglePVControl *accumulationTimeControl_;
	AMSinglePVControl *accumulationTimeAverageControl_;
	AMSinglePVControl *liveTimeControl_;
	AMSinglePVControl *liveTimeAverageControl_;
	AMSinglePVControl *realTimeControl_;
	AMSinglePVControl *realTimeAverageControl_;
	AMSinglePVControl *elapsedTimeControl_;
	AMSinglePVControl *startTimeControl_;
	AMSinglePVControl *endTimeControl_;
	AMSinglePVControl *replyTimeAverageControl_;

	AMSinglePVControl *temperatureControl_;
	AMSinglePVControl *highVoltageControl_;
	AMSinglePVControl *totalGainControl_;
	AMSinglePVControl *mcaChannelCountControl_;
	AMSinglePVControl *pileUpRejectionControl_;
	AMSinglePVControl *thermoelectricCoolerControl_;
	AMSinglePVControl *fastThresholdControl_;
	AMSinglePVControl *slowThresholdControl_;
	AMSinglePVControl *peakingTimeControl_;
	AMSinglePVControl *fastChannelPeakingTimeControl_;

	AMSinglePVControl *roi1LowIndexControl_;
	AMSinglePVControl *roi1HighIndexControl_;
	AMSinglePVControl *roi2LowIndexControl_;
	AMSinglePVControl *roi2HighIndexControl_;
	AMSinglePVControl *roi3LowIndexControl_;
	AMSinglePVControl *roi3HighIndexControl_;
	AMSinglePVControl *roi4LowIndexControl_;
	AMSinglePVControl *roi4HighIndexControl_;
	AMSinglePVControl *roi5LowIndexControl_;
	AMSinglePVControl *roi5HighIndexControl_;
	AMSinglePVControl *roi6LowIndexControl_;
	AMSinglePVControl *roi6HighIndexControl_;
	AMSinglePVControl *roi7LowIndexControl_;
	AMSinglePVControl *roi7HighIndexControl_;
	AMSinglePVControl *roi8LowIndexControl_;
	AMSinglePVControl *roi8HighIndexControl_;
	QList<AMSinglePVControl*> roiLowIndexControls_;
	QList<AMSinglePVControl*> roiHighIndexControls_;

};

#endif // AMPTEKSDD123EPICSDETECTORMANAGER_H
