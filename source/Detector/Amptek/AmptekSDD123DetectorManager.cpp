#include "AmptekSDD123DetectorManager.h"

#include <QDebug>
#include <QSignalMapper>
#include <QCoreApplication>

#include "DataElement/AMDSFlatArray.h"
#include "Detector/Amptek/AmptekSDD123Application.h"

#include "ClientRequest/AMDSClientDataRequest.h"
#include "Detector/Amptek/AmptekSDD123Detector.h"
#include "Detector/Amptek/AmptekSDD123Histogram.h"

#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123ThreadedHistogramGroup.h"
#include "beamline/AMPVControl.h"
#include "DataHolder/Amptek/AMDSAmptekSDD123SpectralDataHolder.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataElement/AMDSThreadedBufferGroup.h"

AmptekSDD123DetectorManager::AmptekSDD123DetectorManager(AmptekSDD123ConfigurationMap *amptekConfiguration, QObject *parent) :
	QObject(parent)
{
	initialized_ = false;

	detector_ = new AmptekSDD123Detector(amptekConfiguration->detectorName(), amptekConfiguration->detectorBasePVName(), amptekConfiguration->dataType(), amptekConfiguration->spectrumCountSize());
	detector_->setSpectrumReceiver(this);

	dwellTime_ = 2.0;
	dwellMode_ = AmptekSDD123DetectorManager::PresetDwell;



	dwellActive_ = false;
	setPresetDwellEndTimeOnNextEvent_ = false;

	presetDwellActive_ = false;

	configurationRequestReason_ = AmptekSDD123DetectorManager::InvalidReason;
}

QString AmptekSDD123DetectorManager::detectorName() const
{
	return detector_->name();
}

AmptekSDD123Detector* AmptekSDD123DetectorManager::detector(){
	return detector_;
}

double AmptekSDD123DetectorManager::dwellTime() const{
	return dwellTime_;
}

AmptekSDD123DetectorManager::DwellMode AmptekSDD123DetectorManager::dwellMode() const{
	return dwellMode_;
}

bool AmptekSDD123DetectorManager::event(QEvent *e){
	if(e->type() == (QEvent::Type)AmptekEventDefinitions::SpectrumEvent){
		spectrumEventHelper(e);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationValuesEvent){
		if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
			qDebug() << "It was a configurationValuesEvent";
		configurationValuesEventHelper(e);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationModeConfirmationEvent){
		if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
			qDebug() << "It was a configurationModeConfirmationEvent";
		configurationModeConfirmationEventHelper(e);
		e->accept();
		return true;
	}
	return QObject::event(e);
}

void AmptekSDD123DetectorManager::setRequestEventReceiver(QObject *requestEventReceiver){
	requestEventReceiver_ = requestEventReceiver;
}

void AmptekSDD123DetectorManager::startDwell(){
	dwellActive_ = true;
	if(dwellMode_ == AmptekSDD123DetectorManager::PresetDwell){
		emit clearDwellHistrogramData(detectorName());
		setPresetDwellEndTimeOnNextEvent_ = true;
	}
}

void AmptekSDD123DetectorManager::stopDwell(){
	dwellActive_ = false;
}

void AmptekSDD123DetectorManager::setDwellActive(bool dwellActive){
	dwellActive_ = dwellActive;
}

void AmptekSDD123DetectorManager::setDwellTime(double dwellTime){
	dwellTime_ = dwellTime;
}

void AmptekSDD123DetectorManager::setDwellMode(AmptekSDD123DetectorManager::DwellMode dwellMode){
	dwellMode_ = dwellMode;
}

void AmptekSDD123DetectorManager::requestDetectorConfigurationValues(){
	configurationRequestReason_ = AmptekSDD123DetectorManager::RequestConfigurationReason;
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorConfigurationValues(){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorAnalogGainIndex(int analogGainIndex){
	if(analogGainIndex < 1 || analogGainIndex > 16)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIA=%1;").arg(analogGainIndex);
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorFineGain(double fineGain){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIF=%1;").arg(fineGain, 0, 'e', 3);
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorTotalGain(double totalGain){
	if(totalGain < 0.75 || totalGain > 150)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("GAIN=%1;").arg(totalGain, 0, 'e', 3);
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorHV(bool on){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(on)
		configurationSetCommand_ = QString("HVSE=ON;");
	else
		configurationSetCommand_ = QString("HVSE=OFF;");
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorMCAEnabled(bool enabled){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(enabled)
		configurationSetCommand_ = QString("MCAE=ON;");
	else
		configurationSetCommand_ = QString("MCAE=OFF;");
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorPileUpRejection(bool enabled){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(enabled)
		configurationSetCommand_ = QString("PURE=ON;");
	else
		configurationSetCommand_ = QString("PURE=OFF;");
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorCoolerSetting(int coolerSetting){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(coolerSetting == -2727)
		configurationSetCommand_ = QString("TECS=OFF;");
	else if(coolerSetting < 0 || coolerSetting > 299)
		return;
	else{
		configurationSetCommand_ = QString("TECS=%1;").arg(coolerSetting);
		qDebug() << "Set TECS to " << coolerSetting;
	}
	initiateRequestConfigurationModeHelper();
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
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorFastThreshold(int fastThreshold){
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	if(fastThreshold < 0 || fastThreshold > 255)
		return;
	else
		configurationSetCommand_ = QString("THFA=%1;").arg(fastThreshold);
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorSlowThreshold(double slowThreshold){
	if(slowThreshold < 0 || slowThreshold > 24.9)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("THSL=%1;").arg(slowThreshold, 0, 'e', 3);
	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::setDetectorPeakingTime(double peakingTime){
	if(peakingTime < 0.2 || peakingTime > 25.6)
		return;
	configurationRequestReason_ = AmptekSDD123DetectorManager::SetConfigurationReason;
	configurationSetCommand_ = QString("TPEA=%1;").arg(peakingTime, 0, 'e', 3);
	initiateRequestConfigurationModeHelper();
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

	initiateRequestConfigurationModeHelper();
}

void AmptekSDD123DetectorManager::initiateRequestConfigurationModeHelper(){
	if(requestEventReceiver_){
		AmptekConfigurationInitiateRequestEvent *configurationInitiateRequestEvent = new AmptekConfigurationInitiateRequestEvent();
		configurationInitiateRequestEvent->configurationMode_ = true;
		QCoreApplication::postEvent(requestEventReceiver_, configurationInitiateRequestEvent);
	}
}

void AmptekSDD123DetectorManager::spectrumEventHelper(QEvent *e){
//	QVector<double> spectrumVector = ((AmptekSpectrumEvent*)e)->spectrum_;
	AMDSFlatArray spectrumVector = ((AmptekSpectrumEvent*)e)->spectrum_;
	AmptekStatusData statusData = ((AmptekSpectrumEvent*)e)->statusData_;

	int totalCounts = 0;
	QVector<int> spectrumIntVector;
	for(int x = 0; x < spectrumVector.size(); x++){
//		spectrumIntVector.append(spectrumVector.at(x));
//TODO: caculate the total count
//		totalCounts += spectrumVector.at(x);
	}

	if(setPresetDwellEndTimeOnNextEvent_){
		setPresetDwellEndTimeOnNextEvent_ = false;
		presetDwellLocalStartTime_ = statusData.dwellStartTime_;
		presetDwellEndTime_ = statusData.dwellStartTime_.addMSecs(dwellTime_*1000);

		//qDebug() << "Local start time was " << presetDwellLocalStartTime_.toString("hh:mm:ss.zzz") << "I want to end after " << presetDwellEndTime_.toString("hh:mm:ss.zzz");
	}
	else {
//// TODO the usage of dwelldata
////		AMDSAmptekSDD123SpectralDataHolder * spectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_[dwellData_->count()-1]);
//		AMDSAmptekSDD123SpectralDataHolder * spectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_->at(dwellData_->count()-1));
//		if((dwellMode_ == AmptekSDD123DetectorManager::PresetDwell) && dwellActive_ && spectrumDataHolder && (spectrumDataHolder->statusData().dwellEndTime_ >= presetDwellEndTime_)){
//			dwellActive_ = false;

//			presetDwellLocalEndTime_ = spectrumDataHolder->statusData().dwellEndTime_;

//			//qDebug() << "I actually stopped dwelling at " << presetDwellLocalEndTime_.toString("hh:mm:ss.zzz") << " for a difference of " << ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;

//			AMDSAmptekSDD123SpectralDataHolder * cumulativeSpectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_->cumulativeDataHolder());
////			emit dwellFinishedDataUpdate(dwellData_->cumulativeSpectrum());
////			emit dwellFinishedStatusDataUpdate(dwellData_->cumulativeStatusData(), dwellData_->count());
//			emit dwellFinishedDataUpdate(cumulativeSpectrumDataHolder);
//			emit dwellFinishedStatusDataUpdate(cumulativeSpectrumDataHolder->statusData(), dwellData_->count());

//			double elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;
////			emit dwellFinishedAllDataUpdate(dwellData_->cumulativeSpectrum(), dwellData_->cumulativeStatusData(), dwellData_->count(), elapsedTime);
//			emit dwellFinishedAllDataUpdate(cumulativeSpectrumDataHolder, cumulativeSpectrumDataHolder->statusData(), dwellData_->count(), elapsedTime);

//			emit dwellFinishedTimeUpdate(elapsedTime);
//		}
//		//else if((dwellMode_ == AmptekSDD123DetectorManager::PresetDwell) && dwellActive_ && (dwellData_->at(dwellData_->count()-1)->statusData().dwellEndTime_ < presetDwellEndTime_))
//		//	qDebug() << "Couldn't stop dwelling because current time is " << QTime::currentTime().toString("hh:mm:ss.zzz") << "(" << QTime::currentTime().addMSecs(-statusData.dwellEndTime_.msecsTo(statusData.dwellReplyTime_)).toString("hh:mm:ss.zzz") << ")";
	}

	AMDSAmptekSDD123SpectralDataHolder *oneHistogram = new AMDSAmptekSDD123SpectralDataHolder(detector_->dataType(), detector_->bufferSize(), this);
	oneHistogram->setData(&spectrumVector);
	oneHistogram->setStatusData(statusData);

//	allData_->append(oneHistogram);
	emit newHistrogramReceived(detectorName(), oneHistogram);
	if(!initialized_ || dwellActive_)
		emit newDwellHistrogramReceived(detectorName(), oneHistogram);
//		dwellData_->append(oneHistogram);

	if(!initialized_){
		emit clearHistrogramData(detectorName());
		emit clearDwellHistrogramData(detectorName());
//		allData_->clear();
//		dwellData_->clear();
		initialized_ = true;
	}
	else if(dwellActive_){
////TODO the usage of dwell data
//		AMDSAmptekSDD123SpectralDataHolder * cumulativeSpectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_->cumulativeDataHolder());

//		//emit continuousDataUpdate(allData_->cumulativeSpectrum());
//		//emit cumulativeStatusDataUpdate(allData_->cumulativeStatusData(), allData_->count());
////		emit continuousDataUpdate(dwellData_->cumulativeSpectrum());
////		emit cumulativeStatusDataUpdate(dwellData_->cumulativeStatusData(), dwellData_->count());
//		emit continuousDataUpdate(cumulativeSpectrumDataHolder);
//		emit cumulativeStatusDataUpdate(cumulativeSpectrumDataHolder->statusData(), dwellData_->count());

////		AMDSAmptekSDD123SpectralDataHolder * spectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_[dwellData_->count()-1]);
//		AMDSAmptekSDD123SpectralDataHolder * spectrumDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(dwellData_->at(dwellData_->count()-1));
//		QTime elapsedEndTime_ = spectrumDataHolder->statusData().dwellEndTime_;
//		double elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(elapsedEndTime_))/1000;
//		emit continuousAllDataUpdate(cumulativeSpectrumDataHolder, cumulativeSpectrumDataHolder->statusData(), dwellData_->count(), elapsedTime);
	}

	/*
	qDebug() << "Heard " << detectorSourceName
		 << "t: " << totalCounts
		 << "f: " << oneHistogram->statusData().fastCounts_
		 << "ti: " << oneHistogram->statusData().dwellStartTime_.toString("hh:mm:ss.zzz")
		 << "tf: " << oneHistogram->statusData().dwellEndTime_.toString("hh:mm:ss.zzz")
		 << "tr: " << oneHistogram->statusData().dwellReplyTime_.toString("hh:mm:ss.zzz")
		 << "a: " << oneHistogram->statusData().accumulationTime_ << " " << (oneHistogram->statusData().accumulationTime_/0.05)*100
		 << "r: " << oneHistogram->statusData().realTime_
		 << "l: " << oneHistogram->statusData().liveTime_;
	*/
}

void AmptekSDD123DetectorManager::configurationValuesEventHelper(QEvent *e){
	configurationData_ = ((AmptekConfigurationValuesEvent*)e)->configurationData_;
	emit configurationValuesUpdate(configurationData_);

	configurationRequestReason_ = AmptekSDD123DetectorManager::InvalidReason;

	if(requestEventReceiver_){
		AmptekDwellRequestEvent *dwellRequestEvent = new AmptekDwellRequestEvent();
		dwellRequestEvent->dwellMode_ = true;
		QCoreApplication::postEvent(requestEventReceiver_, dwellRequestEvent);
	}
}

void AmptekSDD123DetectorManager::configurationModeConfirmationEventHelper(QEvent *e){
	if(((AmptekConfigurationModeConfirmationEvent*)e)->confirmConfigurationMode_){
		if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
			qDebug() << "In setConfigurationModeInitiated";

		if(configurationRequestReason_ == AmptekSDD123DetectorManager::InvalidReason)
			return;

		if(configurationRequestReason_ == AmptekSDD123DetectorManager::RequestConfigurationReason){
			if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
				qDebug() << "Heard detectorManger is now in configuration mode, request the configuration values";
			if(requestEventReceiver_){
				AmptekConfigurationRequestEvent *configurationRequestEvent = new AmptekConfigurationRequestEvent();
				configurationRequestEvent->allParametersRequest_ = true;
				QCoreApplication::postEvent(requestEventReceiver_, configurationRequestEvent);
			}
		}

		if(configurationRequestReason_ == AmptekSDD123DetectorManager::SetConfigurationReason){
			if(requestEventReceiver_){
				AmptekConfigurationSetEvent *configurationSetEvent = new AmptekConfigurationSetEvent();
				QStringList configurationCommands;
				configurationCommands << configurationSetCommand_;
				configurationSetEvent->configurationCommands_ = configurationCommands;
				QCoreApplication::postEvent(requestEventReceiver_, configurationSetEvent);
			}
		}
		}
}

//void AmptekSDD123DetectorManager::forwardDataRequest(AMDSClientDataRequest *data)
//{
//	emit requestData(data);
//}


