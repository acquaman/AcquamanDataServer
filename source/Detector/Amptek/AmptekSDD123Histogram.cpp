#include "AmptekSDD123Histogram.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "DataHolder/Amptek/AMDSAmptekSDD123SpectralDataHolder.h"
#include <QDebug>
AmptekSDD123Histogram::AmptekSDD123Histogram(const QVector<int> &spectrum, const AmptekStatusData &statusData, QObject *parent) :
	QObject(parent)
{
	spectrum_ = spectrum;
	statusData_ = statusData;
}

QVector<int> AmptekSDD123Histogram::spectrum() const{
	return spectrum_;
}

AmptekStatusData AmptekSDD123Histogram::statusData() const{
	return statusData_;
}

QTime AmptekSDD123Histogram::dwellStartTime() const{
	return statusData_.dwellStartTime_;
}

QTime AmptekSDD123Histogram::dwellEndTime() const{
	return statusData_.dwellEndTime_;
}

QTime AmptekSDD123Histogram::dwellReplyTime() const{
	return statusData_.dwellReplyTime_;
}


bool AmptekSDD123Histogram::operator <(const QTime &rhs)
{
	return this->statusData_.dwellStartTime_ < rhs;
}

bool AmptekSDD123Histogram::operator >(const QTime &rhs)
{
	return this->statusData_.dwellStartTime_ > rhs;
}

bool AmptekSDD123Histogram::operator ==(const QTime &rhs)
{
	return this->statusData_.dwellStartTime_ == rhs;
}

void AmptekSDD123Histogram::setSpectrum(const QVector<int> &spectrum){
	spectrum_ = spectrum;
}

void AmptekSDD123Histogram::setStatusData(const AmptekStatusData &statusData){
	statusData_ = statusData;
}

AmptekSDD123DwellHistogramGroup::AmptekSDD123DwellHistogramGroup(QObject *parent) :
	QObject(parent)
{
	initialized_ = false;
}

AmptekSDD123DwellHistogramGroup::AmptekSDD123DwellHistogramGroup(const AmptekSDD123DwellHistogramGroup& other)
	: QObject(other.parent()), histograms_(other.histograms_), cumulativeSpectrum_(other.cumulativeSpectrum_),
	  cumulativeStatusData_(other.cumulativeStatusData_), initialized_(other.initialized_)
{

}

//void AmptekSDD123DwellHistogramGroup::append(AmptekSDD123Histogram *histogram){
void AmptekSDD123DwellHistogramGroup::append(AMDSDataHolder *histogram){
//	QVector<int> histogramSpectrum = histogram->spectrum();
	AMDSFlatArray histogramSpectrum;
	histogram->data(&histogramSpectrum);

	AMDSAmptekSDD123SpectralDataHolder * amptekDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(histogram);

	if (amptekDataHolder) {
		AmptekStatusData histogramStatusData = amptekDataHolder->statusData();

		histograms_.append(histogram);

		if(!initialized_){
			cumulativeStatusData_ = histogramStatusData;
			cumulativeStatusData_.dwellReplyTime_.setHMS(0, 0, 0, 0);

			histogramSpectrum.copyDataToTargetArray(&cumulativeSpectrum_);
//			cumulativeSpectrum_.resize(histogramSpectrum.size());
//			for(int x = 0, size = histogramSpectrum.size(); x < size; x++)
//				cumulativeSpectrum_[x] = histogramSpectrum.at(x);
			initialized_ = true;
		}
		else{
			cumulativeStatusData_.fastCounts_ += histogramStatusData.fastCounts_;
			cumulativeStatusData_.slowCounts_ += histogramStatusData.slowCounts_;
			cumulativeStatusData_.detectorTemperature_ += histogramStatusData.detectorTemperature_;
			cumulativeStatusData_.accumulationTime_ += histogramStatusData.accumulationTime_;
			cumulativeStatusData_.liveTime_ += histogramStatusData.liveTime_;
			cumulativeStatusData_.realTime_ += histogramStatusData.realTime_;
			cumulativeStatusData_.generalPurposeCounter_ += histogramStatusData.generalPurposeCounter_;
			if(cumulativeStatusData_.dwellEndTime_ < histogramStatusData.dwellEndTime_)
				cumulativeStatusData_.dwellEndTime_.setHMS(histogramStatusData.dwellEndTime_.hour(), histogramStatusData.dwellEndTime_.minute(), histogramStatusData.dwellEndTime_.second(), histogramStatusData.dwellEndTime_.msec());
			else if(histogramStatusData.dwellStartTime_ < cumulativeStatusData_.dwellStartTime_)
				cumulativeStatusData_.dwellStartTime_.setHMS(histogramStatusData.dwellStartTime_.hour(), histogramStatusData.dwellStartTime_.minute(), histogramStatusData.dwellStartTime_.second(), histogramStatusData.dwellStartTime_.msec());

			cumulativeStatusData_.dwellReplyTime_ = cumulativeStatusData_.dwellReplyTime_.addMSecs(histogramStatusData.dwellEndTime_.msecsTo(histogramStatusData.dwellReplyTime_));

			cumulativeSpectrum_ = cumulativeSpectrum_ + histogramSpectrum;
//			for(int x = 0, size = histogramSpectrum.size(); x < size; x++)
//				cumulativeSpectrum_[x] += histogramSpectrum.at(x);
		}
	}
}

//AmptekSDD123Histogram* AmptekSDD123DwellHistogramGroup::at(int index) const{
AMDSDataHolder* AmptekSDD123DwellHistogramGroup::at(int index) const{
	if(index < 0 || index >= count())
		return 0;
	return histograms_[index];
}

void AmptekSDD123DwellHistogramGroup::clear(){
	histograms_.clear();
	initialized_ = false;
	cumulativeSpectrum_.clear();
//		cumulativeSpectrum_.resize(0);

}

int AmptekSDD123DwellHistogramGroup::count() const{
	return histograms_.count();
}

AMDSFlatArray AmptekSDD123DwellHistogramGroup::cumulativeSpectrum() const{
	return cumulativeSpectrum_;
}

AmptekStatusData AmptekSDD123DwellHistogramGroup::cumulativeStatusData() const {
	return cumulativeStatusData_;
}

AMDSFlatArray AmptekSDD123DwellHistogramGroup::averageSpectrum() const{
	AMDSFlatArray averageSpectrum = cumulativeSpectrum() / histograms_.count();
	return averageSpectrum;

//	QVector<int> retVal;
//	for(int x = 0, size = cumulativeSpectrum_.size(); x < size; x++)
//				retVal.append(cumulativeSpectrum_.at(x)/histograms_.count());
//	return retVal;

}

AmptekStatusData AmptekSDD123DwellHistogramGroup::averageStatusData() const{
	AmptekStatusData retVal(cumulativeStatusData());
	retVal.fastCounts_ = retVal.fastCounts_/histograms_.count();
	retVal.detectorTemperature_ = retVal.detectorTemperature_/histograms_.count();
	retVal.accumulationTime_ = retVal.accumulationTime_/histograms_.count();
	retVal.liveTime_ = retVal.liveTime_/histograms_.count();
	retVal.realTime_ = retVal.realTime_/histograms_.count();
	retVal.generalPurposeCounter_ = retVal.generalPurposeCounter_/histograms_.count();

		return retVal;
}

AmptekSDD123DwellHistogramGroup&  AmptekSDD123DwellHistogramGroup::operator =(const AmptekSDD123DwellHistogramGroup& other)
{
	this->histograms_ = other.histograms_;
	this->cumulativeSpectrum_ = other.cumulativeSpectrum_;
	this->cumulativeStatusData_ = other.cumulativeStatusData_;
	this->initialized_ = other.initialized_;

	return (*this);
}

AmptekSDD123BufferedHistogramGroup::AmptekSDD123BufferedHistogramGroup(int maxSize, QObject *parent):
	QObject(parent), histograms_(maxSize)
{
}

AmptekSDD123BufferedHistogramGroup::AmptekSDD123BufferedHistogramGroup(const AmptekSDD123BufferedHistogramGroup& other):
	QObject(other.parent()), histograms_(other.histograms_)
{
}

int AmptekSDD123BufferedHistogramGroup::maxSize() const
{
	QReadLocker readLock(&lock_);
	return histograms_.maxSize();
}

//void AmptekSDD123BufferedHistogramGroup::append(AmptekSDD123Histogram *value)
void AmptekSDD123BufferedHistogramGroup::append(AMDSDataHolder *value)
{
	QWriteLocker writeLock(&lock_);
//	AmptekSDD123Histogram* histogramRemoved = histograms_.append(value);
	AMDSDataHolder* histogramRemoved = histograms_.append(value);
	if(histogramRemoved)
	{
		histogramRemoved->deleteLater();
	}
}

void AmptekSDD123BufferedHistogramGroup::clear()
{
	QWriteLocker writeLock(&lock_);
	for(int iElement = 0, elementCount = histograms_.count(); iElement < elementCount; iElement++)
		histograms_[iElement]->deleteLater();

	histograms_.clear();
}

const int AmptekSDD123BufferedHistogramGroup::count() const
{
	QReadLocker readLock(&lock_);
	return histograms_.count();
}

void AmptekSDD123BufferedHistogramGroup::requestData(AMDSClientDataRequest *request)
{
	QReadLocker readLock(&lock_);
	switch(request->requestType()){
	case AMDSClientRequestDefinitions::Continuous:
//		populateData(request, request->time1());
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount:
//		populateData(request, request->time1(), request->count1());
		break;
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
//		populateData(request, request->count1(), request->count2());
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
//		populateData(request, request->time1(), request->time2());
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
//		populateData(request, request->time1(), request->count1(), request->count2());
		break;
	}
	emit dataRequestReady(request);
}

//int AmptekSDD123BufferedHistogramGroup::lowerBound(const QTime &dwellTime)
//{
//	if (histograms_.isEmpty())
//		return -1;

//	int start = 0;
//	int end = histograms_.count() -1;
//	int middle = (start + end) / 2;

//	//// TODO

////	bool found = false;

////	while(!found)
////	{
////		if(middle == 0 || middle == start)
////			found = true;
////		else if (*(histograms_[middle]) == dwellTime)
////		{
////			while(*(histograms_[middle -1]) > dwellTime && middle > 0)
////			{
////				middle--;
////			}
////			found = true;
////		}
////		else if (*(histograms_[middle]) < dwellTime && *(histograms_[middle - 1]) > dwellTime)
////			found = true;
////		else if (*(histograms_[middle]) < dwellTime)
////		{
////			start = middle;
////			middle = (start + end) / 2;
////		}
////		else if (*(histograms_[middle]) > dwellTime)
////		{
////			end = middle;
////			middle = (start + end) / 2;
////		}
////	}

//	return middle;
//}

//void AmptekSDD123BufferedHistogramGroup::populateData(AMDSClientDataRequest *request, const QTime &lastFetch)
//{
//	int startIndex = lowerBound(lastFetch);

//	if(startIndex == -1)
//;//		request->setError(QString("Could not locate spectrum data for time %1").arg(lastFetch.toString()));
//	else
//	{
//		// Since the last fetch actually included the spectrum at the given time, we need to increment the index
//		// by one, to start from the one following:
//		startIndex++;
//		for(int iCurrent = startIndex, size = histograms_.count();iCurrent < size; iCurrent++)
//		{
//			AMDSDataHolder *hist = histograms_[iCurrent];
////			AmptekSDD123Histogram* hist = histograms_[iCurrent];
////			request->histogramData()->append(hist);
//		}
//	}
//}

//void AmptekSDD123BufferedHistogramGroup::populateData(AMDSClientDataRequest* request, const QTime& startTime, int count)
//{
//	int startIndex = lowerBound(startTime);

//	if(startIndex == -1)
//;//		request->setError(QString("Could not locate spectrum data for time %1").arg(startTime.toString()));
//	else
//	{
//		count = startIndex + count;
//		for (int iCurrent = startIndex, limit = histograms_.count(); iCurrent < count && iCurrent < limit; iCurrent++)
//		{
////			request->histogramData()->append(histograms_[iCurrent]);
//		}
//	}
//}
//void AmptekSDD123BufferedHistogramGroup::populateData(AMDSClientDataRequest* request, int relativeCount, int count)
//{
//	int startIndex = histograms_.count() - 1 - relativeCount - count;
//	int endIndex = histograms_.count() - 1 - relativeCount + count;
//	if(startIndex < 0)
//		startIndex = 0;

//	if(endIndex >= histograms_.count())
//		endIndex = histograms_.count() - 1;

//	for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//	{
////		request->histogramData()->append(histograms_[iCurrent]);
//	}
//}

//void AmptekSDD123BufferedHistogramGroup::populateData(AMDSClientDataRequest* request, const QTime& startTime, const QTime& endTime)
//{
//	int startIndex = lowerBound(startTime);
//	int endIndex = lowerBound(endTime);

//	if(startIndex == -1)
//;//		request->setError(QString("Could not locate spectrum data for time %1").arg(startTime.toString()));
//	else if(endIndex == -1)
//;//		request->setError(QString("Could not locate spectrum data for time %1").arg(endTime.toString()));
//	else
//	{
//		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//		{
////			request->histogramData()->append(histograms_[iCurrent]);
//		}
//	}
//}

//void AmptekSDD123BufferedHistogramGroup::populateData(AMDSClientDataRequest* request, const QTime& middleTime, int countBefore, int countAfter)
//{
//	int middleIndex = lowerBound(middleTime);
//	if(middleIndex == -1)
//;//		request->setError(QString("Could not locate spectrum data for time %1").arg(middleTime.toString()));
//	else
//	{
//		int startIndex = middleIndex - countBefore;
//		if(startIndex < 0)
//			startIndex = 0;

//		int endIndex = middleIndex + countAfter;
//		if(endIndex >= histograms_.count())
//			endIndex = histograms_.count() - 1;

//		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//		{
////			request->histogramData()->append(histograms_[iCurrent]);
//		}
//	}
//}









