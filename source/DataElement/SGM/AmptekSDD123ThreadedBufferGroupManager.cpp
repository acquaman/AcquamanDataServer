#include "AmptekSDD123ThreadedBufferGroupManager.h"

#include "Detector/Amptek/AmptekEventDefinitions.h"

AmptekSDD123ThreadedBufferGroupManager::AmptekSDD123ThreadedBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, bool enableCumulative, QObject *parent)
	:AMDSThreadedBufferGroup(bufferGroupInfo, maxCountSize, enableCumulative, parent)
{
	dwellActive_ = false;
	dwellTime_ = 2.0;
	setPresetDwellEndTimeOnNextEvent_ = false;

	dwellData_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize, enableCumulative);
}

void AmptekSDD123ThreadedBufferGroupManager::onSpectrumEvent(QEvent *e)
{
	QVector<double> spectrumVector = ((AmptekSpectrumEvent*)e)->spectrum_;
	AmptekStatusData statusData = ((AmptekSpectrumEvent*)e)->statusData_;

	int totalCounts = 0;
	QVector<int> spectrumIntVector;
	for(int x = 0; x < spectrumVector.count(); x++){
		spectrumIntVector.append(spectrumVector.at(x));
		totalCounts += spectrumVector.at(x);
	}

	if(setPresetDwellEndTimeOnNextEvent_){
		setPresetDwellEndTimeOnNextEvent_ = false;
		presetDwellLocalStartTime_ = statusData.dwellStartTime_;
		presetDwellEndTime_ = statusData.dwellStartTime_.addMSecs(dwellTime_*1000);

		//qDebug() << "Local start time was " << presetDwellLocalStartTime_.toString("hh:mm:ss.zzz") << "I want to end after " << presetDwellEndTime_.toString("hh:mm:ss.zzz");
	} else {
//		if((dwellMode_ == AmptekSDD123DetectorManager::PresetDwell) && dwellActive_ ) {
//			AMDSDataHolder *dataHolder = dwellData_[dwellData_->count()-1];
//			if (dataHolder && && (dataHolder->statusData().dwellEndTime_ >= presetDwellEndTime_)) {
				//		dwellActive_ = false;

				//		presetDwellLocalEndTime_ = dwellData_->at(dwellData_->count()-1)->statusData().dwellEndTime_;

				//		//qDebug() << "I actually stopped dwelling at " << presetDwellLocalEndTime_.toString("hh:mm:ss.zzz") << " for a difference of " << ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;

				//		emit dwellFinishedDataUpdate(dwellData_->cumulativeSpectrum());
				//		emit dwellFinishedStatusDataUpdate(dwellData_->cumulativeStatusData(), dwellData_->count());

				//		double elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(presetDwellLocalEndTime_))/1000;
				//		emit dwellFinishedAllDataUpdate(dwellData_->cumulativeSpectrum(), dwellData_->cumulativeStatusData(), dwellData_->count(), elapsedTime);

				//		emit dwellFinishedTimeUpdate(elapsedTime);
//			}
			//	//else if((dwellMode_ == AmptekSDD123DetectorManager::PresetDwell) && dwellActive_ && (dwellData_->at(dwellData_->count()-1)->statusData().dwellEndTime_ < presetDwellEndTime_))
			//	//	qDebug() << "Couldn't stop dwelling because current time is " << QTime::currentTime().toString("hh:mm:ss.zzz") << "(" << QTime::currentTime().addMSecs(-statusData.dwellEndTime_.msecsTo(statusData.dwellReplyTime_)).toString("hh:mm:ss.zzz") << ")";
//		}
	}

//	AmptekSDD123Histogram *oneHistogram = new AmptekSDD123Histogram(spectrumIntVector, statusData, this);
//	allData_->append(oneHistogram);
//	if(!initialized_ || dwellActive_)
//		dwellData_->append(oneHistogram);

//	if(!initialized_){
//		allData_->clear();
//		dwellData_->clear();
//		initialized_ = true;
//	}
//	else if(dwellActive_){
//		//emit continuousDataUpdate(allData_->cumulativeSpectrum());
//		//emit cumulativeStatusDataUpdate(allData_->cumulativeStatusData(), allData_->count());
//		emit continuousDataUpdate(dwellData_->cumulativeSpectrum());
//		emit cumulativeStatusDataUpdate(dwellData_->cumulativeStatusData(), dwellData_->count());

//		QTime elapsedEndTime_ = dwellData_->at(dwellData_->count()-1)->statusData().dwellEndTime_;
//		double elapsedTime = ((double)presetDwellLocalStartTime_.msecsTo(elapsedEndTime_))/1000;
//		emit continuousAllDataUpdate(dwellData_->cumulativeSpectrum(), dwellData_->cumulativeStatusData(), dwellData_->count(), elapsedTime);
//	}
}
