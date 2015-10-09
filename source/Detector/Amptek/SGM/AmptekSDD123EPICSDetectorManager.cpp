#include "AmptekSDD123EPICSDetectorManager.h"

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataElement/AMDSStatusData.h"
#include "DataHolder/AMDSDataHolder.h"

AmptekSDD123EPICSDetectorManager::AmptekSDD123EPICSDetectorManager(AmptekSDD123ConfigurationMap *amptekConfiguration, QObject *parent)
	:AmptekSDD123DetectorManager(amptekConfiguration, parent)
{
	connected_ = false;
	receivedConfigurationDataOnce_ = false;

	QString detectorBasePVName = detector_->basePVName();

	startDwellControl_ = new AMSinglePVControl("Start Dwell", QString("%1:spectrum:start").arg(detectorBasePVName), this, 0.5);
	stopDwellControl_ = new AMSinglePVControl("Stop Dwell", QString("%1:spectrum:stop").arg(detectorBasePVName), this, 0.5);
	clearSpectrumControl_ = new AMSinglePVControl("Clear Spectrum", QString("%1:spectrum:clear").arg(detectorBasePVName), this, 0.5);
	dwellTimeControl_ = new AMSinglePVControl("Dwell Time", QString("%1:spectrum:dwellTime").arg(detectorBasePVName), this, 0.05);
	dwellModeControl_ = new AMSinglePVControl("Dwell Mode", QString("%1:spectrum:dwellMode").arg(detectorBasePVName), this, 0.5);
	dwellStateControl_ = new AMSinglePVControl("Dwell State", QString("%1:spectrum:dwellState").arg(detectorBasePVName), this, 0.5);

	isAvailableControl_ = new AMSinglePVControl("Detector Available", QString("%1:spectrum:isAvailable").arg(detectorBasePVName), this, 0.5);

	EPICSSpectrumUpdateMSecs_ = 249;
	lastEPICSSpectrumUpdateTime_ = QTime::currentTime().addMSecs(-EPICSSpectrumUpdateMSecs_-100);
	spectrumControl_ = new AMWaveformBinningSinglePVControl("Amptek Array", QString("%1:spectrum").arg(detectorBasePVName), 0, 1023, this);

	/**/
	fastCountsControl_ = new AMSinglePVControl("Fast Counts", QString("%1:spectrum:fastCounts").arg(detectorBasePVName), this, 0.5);
	fastCountsAverageControl_ = new AMSinglePVControl("Average Fast Counts", QString("%1:spectrum:average:fastCounts").arg(detectorBasePVName), this, 0.5);
	slowCountsControl_ = new AMSinglePVControl("Slow Counts", QString("%1:spectrum:slowCounts").arg(detectorBasePVName), this, 0.5);
	slowCountsAverageControl_ = new AMSinglePVControl("Average Slow Counts", QString("%1:spectrum:average:slowCounts").arg(detectorBasePVName), this, 0.5);
	accumulationTimeControl_ = new AMSinglePVControl("Accumulation Time", QString("%1:spectrum:accumulationTime").arg(detectorBasePVName), this, 0.01);
	accumulationTimeAverageControl_ = new AMSinglePVControl("Average Accumulation Time", QString("%1:spectrum:average:accumulationTime").arg(detectorBasePVName), this, 0.01);
	liveTimeControl_ = new AMSinglePVControl("Live Time", QString("%1:spectrum:liveTime").arg(detectorBasePVName), this, 0.01);
	liveTimeAverageControl_ = new AMSinglePVControl("Average Live Time", QString("%1:spectrum:average:liveTime").arg(detectorBasePVName), this, 0.01);
	realTimeControl_ = new AMSinglePVControl("Real Time", QString("%1:spectrum:realTime").arg(detectorBasePVName), this, 0.01);
	realTimeAverageControl_ = new AMSinglePVControl("Average Real Time", QString("%1:spectrum:average:realTime").arg(detectorBasePVName), this, 0.01);
	elapsedTimeControl_ = new AMSinglePVControl("Elapsed Time", QString("%1:spectrum:elapsedTime").arg(detectorBasePVName), this, 0.01);
	startTimeControl_ = new AMSinglePVControl("Start Time", QString("%1:spectrum:startTime").arg(detectorBasePVName), this);
	endTimeControl_ = new AMSinglePVControl("End Time", QString("%1:spectrum:endTime").arg(detectorBasePVName), this);
	replyTimeAverageControl_ = new AMSinglePVControl("Average Reply Time", QString("%1:spectrum:average:replyTime").arg(detectorBasePVName), this, 0.1);
	temperatureControl_ = new AMSinglePVControl("Temperature", QString("%1:parameters:temperature").arg(detectorBasePVName), this, 0.05);
	highVoltageControl_ = new AMSinglePVControl("High Voltage", QString("%1:parameters:highVoltage").arg(detectorBasePVName), this);
	totalGainControl_ = new AMSinglePVControl("Total Gain", QString("%1:parameters:totalGain").arg(detectorBasePVName), this, 0.1);
	mcaChannelCountControl_ = new AMSinglePVControl("MCA Channel Count", QString("%1:parameters:MCAChannelCount").arg(detectorBasePVName), this, 0.5);
	pileUpRejectionControl_ = new AMSinglePVControl("Pile Up Rejection", QString("%1:parameters:pileUpRejection").arg(detectorBasePVName), this);
	thermoelectricCoolerControl_ = new AMSinglePVControl("TE Coolor", QString("%1:parameters:thermoelectricCooler").arg(detectorBasePVName), this, 0.1);
	fastThresholdControl_ = new AMSinglePVControl("Fast Threshold", QString("%1:parameters:fastThreshold").arg(detectorBasePVName), this, 0.5);
	slowThresholdControl_ = new AMSinglePVControl("Slow Threshold", QString("%1:parameters:slowThreshold").arg(detectorBasePVName), this, 0.1);
	peakingTimeControl_ = new AMSinglePVControl("Peaking Time", QString("%1:parameters:peakingTime").arg(detectorBasePVName), this, 0.1);
	fastChannelPeakingTimeControl_ = new AMSinglePVControl("Fast Channel Peaking Time", QString("%1:parameters:fastChannelPeakingTime").arg(detectorBasePVName), this);
	roi1LowIndexControl_ = new AMSinglePVControl("ROI 1 Low Index", QString("%1:ROI:1:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi1HighIndexControl_ = new AMSinglePVControl("ROI 1 High Index", QString("%1:ROI:1:highChannel").arg(detectorBasePVName), this, 0.5);
	roi2LowIndexControl_ = new AMSinglePVControl("ROI 2 Low Index", QString("%1:ROI:2:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi2HighIndexControl_ = new AMSinglePVControl("ROI 2 High Index", QString("%1:ROI:2:highChannel").arg(detectorBasePVName), this, 0.5);
	roi3LowIndexControl_ = new AMSinglePVControl("ROI 3 Low Index", QString("%1:ROI:3:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi3HighIndexControl_ = new AMSinglePVControl("ROI 3 High Index", QString("%1:ROI:3:highChannel").arg(detectorBasePVName), this, 0.5);
	roi4LowIndexControl_ = new AMSinglePVControl("ROI 4 Low Index", QString("%1:ROI:4:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi4HighIndexControl_ = new AMSinglePVControl("ROI 4 High Index", QString("%1:ROI:4:highChannel").arg(detectorBasePVName), this, 0.5);
	roi5LowIndexControl_ = new AMSinglePVControl("ROI 5 Low Index", QString("%1:ROI:5:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi5HighIndexControl_ = new AMSinglePVControl("ROI 5 High Index", QString("%1:ROI:5:highChannel").arg(detectorBasePVName), this, 0.5);
	roi6LowIndexControl_ = new AMSinglePVControl("ROI 6 Low Index", QString("%1:ROI:6:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi6HighIndexControl_ = new AMSinglePVControl("ROI 6 High Index", QString("%1:ROI:6:highChannel").arg(detectorBasePVName), this, 0.5);
	roi7LowIndexControl_ = new AMSinglePVControl("ROI 7 Low Index", QString("%1:ROI:7:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi7HighIndexControl_ = new AMSinglePVControl("ROI 7 High Index", QString("%1:ROI:7:highChannel").arg(detectorBasePVName), this, 0.5);
	roi8LowIndexControl_ = new AMSinglePVControl("ROI 8 Low Index", QString("%1:ROI:8:lowChannel").arg(detectorBasePVName), this, 0.5);
	roi8HighIndexControl_ = new AMSinglePVControl("ROI 8 High Index", QString("%1:ROI:8:highChannel").arg(detectorBasePVName), this, 0.5);

	roiLowIndexControls_.append(roi1LowIndexControl_);
	roiLowIndexControls_.append(roi2LowIndexControl_);
	roiLowIndexControls_.append(roi3LowIndexControl_);
	roiLowIndexControls_.append(roi4LowIndexControl_);
	roiLowIndexControls_.append(roi5LowIndexControl_);
	roiLowIndexControls_.append(roi6LowIndexControl_);
	roiLowIndexControls_.append(roi7LowIndexControl_);
	roiLowIndexControls_.append(roi8LowIndexControl_);

	roiHighIndexControls_.append(roi1HighIndexControl_);
	roiHighIndexControls_.append(roi2HighIndexControl_);
	roiHighIndexControls_.append(roi3HighIndexControl_);
	roiHighIndexControls_.append(roi4HighIndexControl_);
	roiHighIndexControls_.append(roi5HighIndexControl_);
	roiHighIndexControls_.append(roi6HighIndexControl_);
	roiHighIndexControls_.append(roi7HighIndexControl_);
	roiHighIndexControls_.append(roi8HighIndexControl_);
	/**/

	allControls_ = new AMControlSet(this);
	allControls_->addControl(spectrumControl_);
	allControls_->addControl(startDwellControl_);
	allControls_->addControl(stopDwellControl_);
	allControls_->addControl(clearSpectrumControl_);
	allControls_->addControl(dwellTimeControl_);
	allControls_->addControl(dwellModeControl_);
	allControls_->addControl(dwellStateControl_);
	/**/
	allControls_->addControl(fastCountsControl_);
	allControls_->addControl(fastCountsAverageControl_);
	allControls_->addControl(slowCountsControl_);
	allControls_->addControl(slowCountsAverageControl_);
	allControls_->addControl(accumulationTimeControl_);
	allControls_->addControl(accumulationTimeAverageControl_);
	allControls_->addControl(liveTimeControl_);
	allControls_->addControl(liveTimeAverageControl_);
	allControls_->addControl(realTimeControl_);
	allControls_->addControl(realTimeAverageControl_);
	allControls_->addControl(startTimeControl_);
	allControls_->addControl(endTimeControl_);
	allControls_->addControl(replyTimeAverageControl_);
	allControls_->addControl(temperatureControl_);
	allControls_->addControl(highVoltageControl_);
	allControls_->addControl(totalGainControl_);
	allControls_->addControl(mcaChannelCountControl_);
	allControls_->addControl(pileUpRejectionControl_);
	allControls_->addControl(thermoelectricCoolerControl_);
	allControls_->addControl(fastThresholdControl_);
	allControls_->addControl(slowThresholdControl_);
	allControls_->addControl(peakingTimeControl_);
	allControls_->addControl(fastChannelPeakingTimeControl_);
	allControls_->addControl(roi1LowIndexControl_);
	allControls_->addControl(roi1HighIndexControl_);
	allControls_->addControl(roi2LowIndexControl_);
	allControls_->addControl(roi2HighIndexControl_);
	allControls_->addControl(roi3LowIndexControl_);
	allControls_->addControl(roi3HighIndexControl_);
	allControls_->addControl(roi4LowIndexControl_);
	allControls_->addControl(roi4HighIndexControl_);
	allControls_->addControl(roi5LowIndexControl_);
	allControls_->addControl(roi5HighIndexControl_);
	allControls_->addControl(roi6LowIndexControl_);
	allControls_->addControl(roi6HighIndexControl_);
	allControls_->addControl(roi7LowIndexControl_);
	allControls_->addControl(roi7HighIndexControl_);
	allControls_->addControl(roi8LowIndexControl_);
	allControls_->addControl(roi8HighIndexControl_);
	/**/

	connect(this, SIGNAL(continuousAllDataUpdate(QVector<int>,AMDSStatusData,int,double)), this, SLOT(onContinuousAllDataUpdate(QVector<int>,AMDSStatusData,int,double)));
	connect(this, SIGNAL(dwellFinishedAllDataUpdate(QVector<int>,AMDSStatusData,int,double)), this, SLOT(onDwellFinishedAllDataUpdate(QVector<int>,AMDSStatusData,int,double)));
	connect(this, SIGNAL(configurationValuesUpdate(AmptekConfigurationData)), this, SLOT(onConfigurationValuesUpdate(AmptekConfigurationData)));

	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	connect(allControls_, SIGNAL(controlSetTimedOut()), this, SLOT(onAllControlsTimedOut()));

	connect(startDwellControl_, SIGNAL(valueChanged(double)), this, SLOT(onStartDwellControlValueChange(double)));
	connect(stopDwellControl_, SIGNAL(valueChanged(double)), this, SLOT(onStopDwellControlValueChange(double)));
	connect(clearSpectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onClearSpectrumControlValueChange(double)));
	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlValueChange(double)));
	connect(dwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellModeControlValueChange(double)));

	/**/
	connect(highVoltageControl_, SIGNAL(valueChanged(double)), this, SLOT(onHighVoltageControlValueChanged(double)));
	connect(totalGainControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalGainControlValueChanged(double)));
	connect(pileUpRejectionControl_, SIGNAL(valueChanged(double)), this, SLOT(onPileUpRejectionControlValueChanged(double)));
	connect(thermoelectricCoolerControl_, SIGNAL(valueChanged(double)), this, SLOT(onThermoelectricCoolerControlValueChanged(double)));
	connect(fastThresholdControl_, SIGNAL(valueChanged(double)), this, SLOT(onFastThresholdControlValueChanged(double)));
	connect(slowThresholdControl_, SIGNAL(valueChanged(double)), this, SLOT(onSlowThresholdControlValueChanged(double)));
	connect(peakingTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onPeakingTimeControlValueChanged(double)));
	connect(fastChannelPeakingTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onFastChannelPeakingTimeControlValueChanged(double)));
	connect(roi1LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI1LowIndexControlValueChanged(double)));
	connect(roi1HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI1HighIndexControlValueChanged(double)));
	connect(roi2LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI2LowIndexControlValueChanged(double)));
	connect(roi2HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI2HighIndexControlValueChanged(double)));
	connect(roi3LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI3LowIndexControlValueChanged(double)));
	connect(roi3HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI3HighIndexControlValueChanged(double)));
	connect(roi4LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI4LowIndexControlValueChanged(double)));
	connect(roi4HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI4HighIndexControlValueChanged(double)));
	connect(roi5LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI5LowIndexControlValueChanged(double)));
	connect(roi5HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI5HighIndexControlValueChanged(double)));
	connect(roi6LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI6LowIndexControlValueChanged(double)));
	connect(roi6HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI6HighIndexControlValueChanged(double)));
	connect(roi7LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI7LowIndexControlValueChanged(double)));
	connect(roi7HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI7HighIndexControlValueChanged(double)));
	connect(roi8LowIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI8LowIndexControlValueChanged(double)));
	connect(roi8HighIndexControl_, SIGNAL(valueChanged(double)), this, SLOT(onROI8HighIndexControlValueChanged(double)));
	/**/
}

void AmptekSDD123EPICSDetectorManager::onContinuousAllDataUpdate(AMDSDataHolder *spectrum, AMDSStatusData statusData, int count, double elapsedTime)
{
	if(lastEPICSSpectrumUpdateTime_.addMSecs(EPICSSpectrumUpdateMSecs_) <= QTime::currentTime()){
		lastEPICSSpectrumUpdateTime_ = QTime::currentTime();
		dataHelper(spectrum, statusData, count, elapsedTime);
	}
}

void AmptekSDD123EPICSDetectorManager::onDwellFinishedAllDataUpdate(AMDSDataHolder *spectrum, AMDSStatusData statusData, int count, double elapsedTime)
{
	dataHelper(spectrum, statusData, count, elapsedTime);

	connect(spectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlValueChanged()));
}

void AmptekSDD123EPICSDetectorManager::onSpectrumControlValueChanged(){
	disconnect(spectrumControl_, SIGNAL(valueChanged(double)), this, SLOT(onSpectrumControlValueChanged()));

	dwellStateControl_->move(0);
}

void AmptekSDD123EPICSDetectorManager::onConfigurationValuesUpdate(AmptekConfigurationData configurationData){
	if(!connected_)
		return;

	if(!receivedConfigurationDataOnce_)
		receivedConfigurationDataOnce_ = true;

	/**/
	if(!totalGainControl_->withinTolerance(configurationData.totalGain_))
		totalGainControl_->move(configurationData.totalGain_);

	if(highVoltageControl_->withinTolerance(1) && configurationData.hvSet_ == "OFF")
		highVoltageControl_->move(0);
	else if(highVoltageControl_->withinTolerance(0) && configurationData.hvSet_ != "OFF")
		highVoltageControl_->move(1);

	if(!mcaChannelCountControl_->withinTolerance(0) && configurationData.mcaCount_ == 256)
		mcaChannelCountControl_->move(0);
	else if(!mcaChannelCountControl_->withinTolerance(1) && configurationData.mcaCount_ == 512)
		mcaChannelCountControl_->move(1);
	else if(!mcaChannelCountControl_->withinTolerance(0) && configurationData.mcaCount_ == 1024)
		mcaChannelCountControl_->move(2);

	if(pileUpRejectionControl_->withinTolerance(1) && configurationData.pileUpRejection_ == "OFF")
		pileUpRejectionControl_->move(0);
	else if(pileUpRejectionControl_->withinTolerance(0) && configurationData.pileUpRejection_ == "ON")
		pileUpRejectionControl_->move(1);

	if(thermoelectricCoolerControl_->readPV()->getString() != configurationData.coolerSetting_)
		thermoelectricCoolerControl_->move(configurationData.coolerSetting_);

	if(!fastThresholdControl_->withinTolerance(configurationData.fastThreshold_))
		fastThresholdControl_->move(configurationData.fastThreshold_);

	if(!slowThresholdControl_->withinTolerance(configurationData.slowThreshold_))
		slowThresholdControl_->move(configurationData.slowThreshold_);

	if(!peakingTimeControl_->withinTolerance(configurationData.peakingTime_))
		peakingTimeControl_->move(configurationData.peakingTime_);

	if(!fastChannelPeakingTimeControl_->withinTolerance(0) && configurationData.fastChannelPeakingTime_ == 50)
		fastChannelPeakingTimeControl_->move(0);
	else if(!fastChannelPeakingTimeControl_->withinTolerance(1) && configurationData.fastChannelPeakingTime_ == 100)
		fastChannelPeakingTimeControl_->move(1);
	else if(!fastChannelPeakingTimeControl_->withinTolerance(2) && configurationData.fastChannelPeakingTime_ == 400)
		fastChannelPeakingTimeControl_->move(2);

	for(int x = 0, size = configurationData.scaIndices_.count(); x < size; x++){
		if(!roiLowIndexControls_.at(x)->withinTolerance(configurationData.scaLowIndices_.at(x)))
			roiLowIndexControls_.at(x)->move(configurationData.scaLowIndices_.at(x));
		if(!roiHighIndexControls_.at(x)->withinTolerance(configurationData.scaHighIndices_.at(x)))
			roiHighIndexControls_.at(x)->move(configurationData.scaHighIndices_.at(x));
	}
	/**/
}

void AmptekSDD123EPICSDetectorManager::dataHelper(AMDSDataHolder *spectrum, AMDSStatusData statusData, int count, double elapsedTime){
	if(!connected_)
		return;

//	qDebug() << "Going to put " << spectrum;
	AMDSFlatArray spectrumData;
	spectrum->data(&spectrumData);
	spectrumControl_->setValues(spectrumData.constVectorDouble()); // TODO: I need to know the type for a general usage ... How??

	/**/
	if(!fastCountsControl_->withinTolerance(statusData.fastCounts_))
		fastCountsControl_->move(statusData.fastCounts_);
	if(!fastCountsAverageControl_->withinTolerance(statusData.fastCounts_/count))
		fastCountsAverageControl_->move(statusData.fastCounts_/count);
	if(!slowCountsControl_->withinTolerance(statusData.slowCounts_))
		slowCountsControl_->move(statusData.slowCounts_);
	if(!slowCountsAverageControl_->withinTolerance(statusData.slowCounts_/count))
		slowCountsAverageControl_->move(statusData.slowCounts_/count);
	if(!accumulationTimeControl_->withinTolerance(statusData.accumulationTime_))
		accumulationTimeControl_->move(statusData.accumulationTime_);
	if(!accumulationTimeAverageControl_->withinTolerance(statusData.accumulationTime_/count))
		accumulationTimeAverageControl_->move(statusData.accumulationTime_/count);
	if(!liveTimeControl_->withinTolerance(statusData.liveTime_))
		liveTimeControl_->move(statusData.liveTime_);
	if(!liveTimeAverageControl_->withinTolerance(statusData.liveTime_/count))
		liveTimeAverageControl_->move(statusData.liveTime_/count);
	if(!realTimeControl_->withinTolerance(statusData.realTime_))
		realTimeControl_->move(statusData.realTime_);
	if(!realTimeAverageControl_->withinTolerance(statusData.realTime_/count))
		realTimeAverageControl_->move(statusData.realTime_/count);
	if(!elapsedTimeControl_->withinTolerance(elapsedTime))
		elapsedTimeControl_->move(elapsedTime);
	if(startTimeControl_->readPV()->getString() != statusData.dwellStartTime_.toString("hh:mm:ss.zzz"))
		startTimeControl_->move(statusData.dwellStartTime_.toString("hh:mm:ss.zzz"));
	if(endTimeControl_->readPV()->getString() != statusData.dwellEndTime_.toString("hh:mm:ss.zzz"))
		endTimeControl_->move(statusData.dwellEndTime_.toString("hh:mm:ss.zzz"));

	int dwellReplyHours = statusData.dwellReplyTime_.hour();
	int dwellReplyMinutes = statusData.dwellReplyTime_.minute();
	int dwellReplySeconds = statusData.dwellReplyTime_.second();
	int dwellReplyMSecs = statusData.dwellReplyTime_.msec();
	int totalMSecs = dwellReplyMSecs + dwellReplySeconds*1000 + dwellReplyMinutes*1000*60 + dwellReplyHours*1000*60*60;
	qDebug() << "totalMSecs is " << totalMSecs << " so average is " << totalMSecs/count;
	if(!replyTimeAverageControl_->withinTolerance(totalMSecs/count))
		replyTimeAverageControl_->move(totalMSecs/count);
	if(!temperatureControl_->withinTolerance(statusData.detectorTemperature_/count))
		temperatureControl_->move(statusData.detectorTemperature_/count);
	/**/
}

void AmptekSDD123EPICSDetectorManager::onStartDwellControlValueChange(double newValue){
	Q_UNUSED(newValue)
	if(startDwellControl_->withinTolerance(1)){
		startDwell();
		startDwellControl_->move(0);
		dwellStateControl_->move(1);
	}
}

void AmptekSDD123EPICSDetectorManager::onStopDwellControlValueChange(double newValue){
	Q_UNUSED(newValue)
	if(stopDwellControl_->withinTolerance(1)){
		stopDwell();
		stopDwellControl_->move(0);
		dwellStateControl_->move(0);
	}
}

void AmptekSDD123EPICSDetectorManager::onClearSpectrumControlValueChange(double newValue){
	Q_UNUSED(newValue)
	if(clearSpectrumControl_->withinTolerance(1)){
		emit clearDwellHistrogramData(detectorName());
		clearSpectrumControl_->move(0);
	}
}

void AmptekSDD123EPICSDetectorManager::onDwellTimeControlValueChange(double newValue){
	if(!dwellTimeControl_->withinTolerance(dwellTime()))
		setDwellTime((int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onDwellModeControlValueChange(double newValue){
	if(dwellModeControl_->withinTolerance(0) && dwellMode() == AmptekSDD123DetectorManager::PresetDwell)
		return;
	else if(dwellModeControl_->withinTolerance(1) && dwellMode() == AmptekSDD123DetectorManager::ContinuousDwell)
		return;
	else{
		int newMode = (int)newValue;
		switch(newMode){
		case 0:
			setDwellMode(AmptekSDD123DetectorManager::PresetDwell);
			break;
		case 1:
			setDwellMode(AmptekSDD123DetectorManager::ContinuousDwell);
			break;
		default:
			break;
		}
	}
}

void AmptekSDD123EPICSDetectorManager::onHighVoltageControlValueChanged(double newValue){
	Q_UNUSED(newValue)

	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	/*
	if(newValue == 0 && configurationData_.hvSet_ == "OFF")
		return;
	else if(newValue == 1 && configurationData_.hvSet_ == "ON")
		return;
	else if(newValue == 0)
		setDetectorHV(false);
	else
		setDetectorHV(true);
	*/
}

void AmptekSDD123EPICSDetectorManager::onTotalGainControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(totalGainControl_->withinTolerance(configurationData_.totalGain_))
		return;
	setDetectorTotalGain(newValue);
}

void AmptekSDD123EPICSDetectorManager::onPileUpRejectionControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(newValue == 0 && configurationData_.pileUpRejection_ == "OFF")
		return;
	else if(newValue == 1 && configurationData_.pileUpRejection_ == "ON")
		return;
	else if(newValue == 0)
		setDetectorPileUpRejection(false);
	else
		setDetectorPileUpRejection(true);
}

void AmptekSDD123EPICSDetectorManager::onThermoelectricCoolerControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(configurationData_.coolerSetting_ == "OFF"){
		//I need to do something about this
		return;
	}
	double coolerSetting = configurationData_.coolerSetting_.toDouble();
	if(thermoelectricCoolerControl_->withinTolerance(coolerSetting))
		return;
	setDetectorCoolerSetting((int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onFastThresholdControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(fastThresholdControl_->withinTolerance(configurationData_.fastThreshold_))
		return;
	setDetectorFastThreshold((int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onSlowThresholdControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(slowThresholdControl_->withinTolerance(configurationData_.slowThreshold_))
		return;
	setDetectorSlowThreshold(newValue);
}

void AmptekSDD123EPICSDetectorManager::onPeakingTimeControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(peakingTimeControl_->withinTolerance(configurationData_.peakingTime_))
		return;
	setDetectorPeakingTime(newValue);
}

void AmptekSDD123EPICSDetectorManager::onFastChannelPeakingTimeControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(newValue == 0 && configurationData_.fastChannelPeakingTime_ == 50)
		return;
	else if(newValue == 1 && configurationData_.fastChannelPeakingTime_ == 100)
		return;
	else if(newValue == 2 && configurationData_.fastChannelPeakingTime_ == 400)
		return;
	else{
		int enumValue = (int)newValue;
		switch(enumValue){
		case 0:
			setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTime50);
			break;
		case 1:
			setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTime100);
			break;
		case 2:
			setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTime400);
			break;
		default:
			break;
		}
	}
}

void AmptekSDD123EPICSDetectorManager::onROI1LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi1LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(0)))
		return;
	setDetectorSingleChannelAnalyzer(1, (int)newValue, configurationData_.scaHighIndices_.at(0));
}

void AmptekSDD123EPICSDetectorManager::onROI1HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi1HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(0)))
		return;
	setDetectorSingleChannelAnalyzer(1, configurationData_.scaLowIndices_.at(0), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI2LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi2LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(1)))
		return;
	setDetectorSingleChannelAnalyzer(2, (int)newValue, configurationData_.scaHighIndices_.at(1));
}

void AmptekSDD123EPICSDetectorManager::onROI2HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi2HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(1)))
		return;
	setDetectorSingleChannelAnalyzer(2, configurationData_.scaLowIndices_.at(1), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI3LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi3LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(2)))
		return;
	setDetectorSingleChannelAnalyzer(3, (int)newValue, configurationData_.scaHighIndices_.at(2));
}

void AmptekSDD123EPICSDetectorManager::onROI3HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi3HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(2)))
		return;
	setDetectorSingleChannelAnalyzer(3, configurationData_.scaLowIndices_.at(2), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI4LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi4LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(3)))
		return;
	setDetectorSingleChannelAnalyzer(4, (int)newValue, configurationData_.scaHighIndices_.at(3));
}

void AmptekSDD123EPICSDetectorManager::onROI4HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi4HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(3)))
		return;
	setDetectorSingleChannelAnalyzer(4, configurationData_.scaLowIndices_.at(3), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI5LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi5LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(4)))
		return;
	setDetectorSingleChannelAnalyzer(5, (int)newValue, configurationData_.scaHighIndices_.at(4));
}

void AmptekSDD123EPICSDetectorManager::onROI5HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi5HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(4)))
		return;
	setDetectorSingleChannelAnalyzer(5, configurationData_.scaLowIndices_.at(4), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI6LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi6LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(5)))
		return;
	setDetectorSingleChannelAnalyzer(6, (int)newValue, configurationData_.scaHighIndices_.at(5));
}

void AmptekSDD123EPICSDetectorManager::onROI6HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi6HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(5)))
		return;
	setDetectorSingleChannelAnalyzer(6, configurationData_.scaLowIndices_.at(5), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI7LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi7LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(6)))
		return;
	setDetectorSingleChannelAnalyzer(7, (int)newValue, configurationData_.scaHighIndices_.at(6));
}

void AmptekSDD123EPICSDetectorManager::onROI7HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi7HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(6)))
		return;
	setDetectorSingleChannelAnalyzer(7, configurationData_.scaLowIndices_.at(6), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onROI8LowIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi8LowIndexControl_->withinTolerance(configurationData_.scaLowIndices_.at(7)))
		return;
	setDetectorSingleChannelAnalyzer(8, (int)newValue, configurationData_.scaHighIndices_.at(7));
}

void AmptekSDD123EPICSDetectorManager::onROI8HighIndexControlValueChanged(double newValue){
	if(!connected_ || !receivedConfigurationDataOnce_)
		return;
	if(roi8HighIndexControl_->withinTolerance(configurationData_.scaHighIndices_.at(7)))
		return;
	setDetectorSingleChannelAnalyzer(8, configurationData_.scaLowIndices_.at(7), (int)newValue);
}

void AmptekSDD123EPICSDetectorManager::onAllControlsConnected(bool connected){
	if(connected_ != connected)
		connected_ = connected;

	if(!connected_)
		return;

	isAvailableControl_->move(1);

	requestDetectorConfigurationValues();

	startDwellControl_->move(0);
	stopDwellControl_->move(0);
	clearSpectrumControl_->move(0);
	dwellTimeControl_->move(1.0);
	dwellModeControl_->move(0);
	dwellStateControl_->move(0);

	QVector<int> emptySpectrum(1024);
	emptySpectrum.fill(0);
	spectrumControl_->setValues(emptySpectrum);

	/**/
	fastCountsControl_->move(0);
	fastCountsAverageControl_->move(0);
	slowCountsControl_->move(0);
	slowCountsAverageControl_->move(0);
	accumulationTimeControl_->move(0);
	accumulationTimeAverageControl_->move(0);
	liveTimeControl_->move(0);
	liveTimeAverageControl_->move(0);
	realTimeControl_->move(0);
	realTimeAverageControl_->move(0);
	elapsedTimeControl_->move(0);
	startTimeControl_->move("Invalid");
	endTimeControl_->move("Invalid");
	replyTimeAverageControl_->move(0);

	mcaChannelCountControl_->move(1024);
	/**/
}

void AmptekSDD123EPICSDetectorManager::onAllControlsTimedOut(){
	connected_ = false;
	if(isAvailableControl_->isConnected())
		isAvailableControl_->move(0);
}
