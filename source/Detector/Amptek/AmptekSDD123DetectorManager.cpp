#include "AmptekSDD123DetectorManager.h"

#include <QCoreApplication>

#include "DataElement/AMDSFlatArray.h"
#include "DataElement/AMDSDwellStatusData.h"
#include "DataHolder/AMDSSpectralDataHolder.h"

#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"


AmptekSDD123DetectorManager::AmptekSDD123DetectorManager(AmptekSDD123ConfigurationMap *amptekConfiguration, QObject *parent) :
	QObject(parent)
{
	requestEventReceiver_ = 0;

	initialized_ = false;

	detector_ = new AmptekSDD123Detector(amptekConfiguration->detectorName(), amptekConfiguration->detectorBasePVName(), amptekConfiguration->dataType(), amptekConfiguration->spectrumCountSize());
	detector_->setSpectrumReceiver(this);

	dwellMode_ = AmptekSDD123DetectorManager::PresetDwell;
	dwellActive_ = false;
	dwellTime_ = 2;
	setPresetDwellEndTimeOnNextEvent_ = false;

	configurationRequestReason_ = AmptekSDD123DetectorManager::InvalidReason;
}

AmptekSDD123DetectorManager::~AmptekSDD123DetectorManager()
{
	detector_->deleteLater();
	detector_ = 0;
}

bool AmptekSDD123DetectorManager::event(QEvent *e){
	if(e->type() == (QEvent::Type)AmptekEventDefinitions::SpectrumEvent){
		onSpectrumEventReceived((AmptekSpectrumEvent *)e);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationValuesEvent){
		onConfigurationValuesEventReceived((AmptekConfigurationValuesEvent *)e);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationModeConfirmationEvent){
		onConfigurationModeConfirmationEventReceived((AmptekConfigurationModeConfirmationEvent *)e);
		e->accept();
		return true;
	}

	return QObject::event(e);
}

void AmptekSDD123DetectorManager::setRequestEventReceiver(QObject *requestEventReceiver){
	requestEventReceiver_ = requestEventReceiver;
}

void AmptekSDD123DetectorManager::startDwell(){
	setDwellActive(true);

	if(dwellMode_ == AmptekSDD123DetectorManager::PresetDwell){
		emit clearDwellHistrogramData(detectorName());
		setPresetDwellEndTimeOnNextEvent_ = true;
	}
}

void AmptekSDD123DetectorManager::stopDwell(){
	setDwellActive(false);
}

void AmptekSDD123DetectorManager::setDwellActive(bool dwellActive){
	dwellActive_ = dwellActive;
}

void AmptekSDD123DetectorManager::setDwellTime(int dwellTime){
	dwellTime_ = dwellTime;
}

void AmptekSDD123DetectorManager::setDwellMode(AmptekSDD123DetectorManager::DwellMode dwellMode){
	dwellMode_ = dwellMode;
}

void AmptekSDD123DetectorManager::requestDetectorConfigurationValues(){
	configurationRequestReason_ = AmptekSDD123DetectorManager::RequestConfigurationReason;
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorConfigurationValues(){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorAnalogGainIndex(int analogGainIndex){
	if(analogGainIndex < 1 || analogGainIndex > 16)
		return;

	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIA=%1;").arg(analogGainIndex);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorFineGain(double fineGain){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIF=%1;").arg(fineGain, 0, 'e', 3);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorTotalGain(double totalGain){
	if(totalGain < 0.75 || totalGain > 150)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIN=%1;").arg(totalGain, 0, 'e', 3);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorHV(bool on){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(on)
		configurationSetCommand_ = QString("HVSE=ON;");
	else
		configurationSetCommand_ = QString("HVSE=OFF;");
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorMCAEnabled(bool enabled){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(enabled)
		configurationSetCommand_ = QString("MCAE=ON;");
	else
		configurationSetCommand_ = QString("MCAE=OFF;");
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorPileUpRejection(bool enabled){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(enabled)
		configurationSetCommand_ = QString("PURE=ON;");
	else
		configurationSetCommand_ = QString("PURE=OFF;");
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorCoolerSetting(int coolerSetting){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(coolerSetting == -2727)
		configurationSetCommand_ = QString("TECS=OFF;");
	else if(coolerSetting < 0 || coolerSetting > 299)
		return;
	else{
		configurationSetCommand_ = QString("TECS=%1;").arg(coolerSetting);
	}
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorSingleChannelAnalyzer(int singleChannelAnalyzerIndex, int lowChannelIndex, int highChannelIndex){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(singleChannelAnalyzerIndex < 1 || singleChannelAnalyzerIndex > 16)
		return;
	if(lowChannelIndex < 0 || highChannelIndex < 0)
		return;
	if(lowChannelIndex > 1023 || highChannelIndex > 1023)
		return;
	if(lowChannelIndex >= highChannelIndex)
		return;
	configurationSetCommand_ = QString("SCAI=%1;SCAH=%2;SCAL=%3;SCAO=HI;").arg(singleChannelAnalyzerIndex).arg(highChannelIndex).arg(lowChannelIndex);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorFastThreshold(int fastThreshold){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(fastThreshold < 0 || fastThreshold > 255)
		return;
	else
		configurationSetCommand_ = QString("THFA=%1;").arg(fastThreshold);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorSlowThreshold(double slowThreshold){
	if(slowThreshold < 0 || slowThreshold > 24.9)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("THSL=%1;").arg(slowThreshold, 0, 'e', 3);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorPeakingTime(double peakingTime){
	if(peakingTime < 0.2 || peakingTime > 25.6)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("TPEA=%1;").arg(peakingTime, 0, 'e', 3);
	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTimeValue fastPeakingTimeValue){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	switch(fastPeakingTimeValue){
	case AmptekSDD123DetectorManager::FastPeakingTime50:
		configurationSetCommand_ = QString("TPFA=50;");
		break;
	case AmptekSDD123DetectorManager::FastPeakingTime100:
		configurationSetCommand_ = QString("TPFA=100;");
		break;
	case AmptekSDD123DetectorManager::FastPeakingTime400:
		configurationSetCommand_ = QString("TPFA=400;");
		break;
	}

	postConfigurationInitiateRequestEvent();
}

void AmptekSDD123DetectorManager::onSpectrumEventReceived(AmptekSpectrumEvent *spectrumEvent){
	AMDSFlatArray spectrumData = spectrumEvent->spectrum_;
	AMDSDwellStatusData statusData = spectrumEvent->statusData_;

	if(setPresetDwellEndTimeOnNextEvent_){
		setPresetDwellEndTimeOnNextEvent_ = false;
		presetDwellLocalStartTime_ = statusData.dwellStartTime();
		presetDwellEndTime_ = statusData.dwellStartTime().addMSecs(dwellTime_*1000);
	}
	else if (dwellActive_ && isPresetDwell() && presetDwellLocalEndTime_.isValid()) {
		if ( presetDwellLocalEndTime_ >= presetDwellEndTime_ ) { //dwellToBeDeactived
			dwellActive_ = false;

			double elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;
			emit dwellFinishedUpdate(detectorName(), elapsedTime);
		}
	}

	if (!initialized_) {
		emit clearHistrogramData(detectorName());
		emit clearDwellHistrogramData(detectorName());
		initialized_ = true;
	}

	// generate the spectrum data holder and notice the bufferGroup new data is ready
	AMDSDwellSpectralDataHolder *oneHistogram = new AMDSDwellSpectralDataHolder(detector_->dataType(), detector_->bufferSize(), this);
	oneHistogram->setData(&spectrumData);
	oneHistogram->setDwellStatusData(statusData);

	emit newHistrogramReceived(detectorName(), oneHistogram);
	if (dwellActive_) {
		presetDwellLocalEndTime_ = statusData.dwellEndTime();

		double elapsedTime = 0;
		if (presetDwellLocalStartTime_.isValid())
			elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;

		emit newDwellHistrogramReceived(detectorName(), oneHistogram, elapsedTime);
	}
}

void AmptekSDD123DetectorManager::onConfigurationValuesEventReceived(AmptekConfigurationValuesEvent *configurationValueEvent){
	configurationData_ = configurationValueEvent->configurationData();
	emit configurationValuesUpdate(configurationData_);

	configurationRequestReason_ = AmptekSDD123DetectorManager::InvalidReason;

	postDwellRequestEvent();
}

void AmptekSDD123DetectorManager::onConfigurationModeConfirmationEventReceived(AmptekConfigurationModeConfirmationEvent *configurationModeConfirmationEvent){
	if(configurationModeConfirmationEvent->confirmConfigurationMode_){
		if(configurationRequestReason_ == AmptekSDD123DetectorManager::InvalidReason)
			return;

		if(configurationRequestReason_ == AmptekSDD123DetectorManager::RequestConfigurationReason){
			postConfigurationRequestEvent();
		}

		if(configurationRequestReason_ == AmptekSDD123DetectorManager::SetConfigurationReason){
			postConfigurationSetEvent();
		}
	}
}

void AmptekSDD123DetectorManager::postDwellRequestEvent()
{
	if(requestEventReceiver_ != 0){
		AmptekDwellRequestEvent *dwellRequestEvent = new AmptekDwellRequestEvent();
		dwellRequestEvent->dwellMode_ = true;
		QCoreApplication::postEvent(requestEventReceiver_, dwellRequestEvent);
	} else {
		AMErrorMon::alert(this, DETECTOR_MANAGER_ALERT_EVENT_RECEIVER_NO_INITIALIZED, "The requestEventReceiver of AmptekSDD123DetectorManager is NOT initialized.");
	}
}

void AmptekSDD123DetectorManager::postConfigurationInitiateRequestEvent(){
	if(requestEventReceiver_ != 0){
		AmptekConfigurationInitiateRequestEvent *configurationInitiateRequestEvent = new AmptekConfigurationInitiateRequestEvent();
		configurationInitiateRequestEvent->configurationMode_ = true;
		QCoreApplication::postEvent(requestEventReceiver_, configurationInitiateRequestEvent);
	} else {
		AMErrorMon::alert(this, DETECTOR_MANAGER_ALERT_EVENT_RECEIVER_NO_INITIALIZED, "The requestEventReceiver of AmptekSDD123DetectorManager is NOT initialized.");
	}
}
void AmptekSDD123DetectorManager::postConfigurationRequestEvent()
{
	if(requestEventReceiver_ != 0){
		AmptekConfigurationRequestEvent *configurationRequestEvent = new AmptekConfigurationRequestEvent();
		configurationRequestEvent->allParametersRequest_ = true;
		QCoreApplication::postEvent(requestEventReceiver_, configurationRequestEvent);
	} else {
		AMErrorMon::alert(this, DETECTOR_MANAGER_ALERT_EVENT_RECEIVER_NO_INITIALIZED, "The requestEventReceiver of AmptekSDD123DetectorManager is NOT initialized.");
	}
}

void AmptekSDD123DetectorManager::postConfigurationSetEvent()
{
	if(requestEventReceiver_ != 0){
		AmptekConfigurationSetEvent *configurationSetEvent = new AmptekConfigurationSetEvent();
		QStringList configurationCommands;
		configurationCommands << configurationSetCommand_;
		configurationSetEvent->configurationCommands_ = configurationCommands;
		QCoreApplication::postEvent(requestEventReceiver_, configurationSetEvent);
	} else {
		AMErrorMon::alert(this, DETECTOR_MANAGER_ALERT_EVENT_RECEIVER_NO_INITIALIZED, "The requestEventReceiver of AmptekSDD123DetectorManager is NOT initialized.");
	}

}
