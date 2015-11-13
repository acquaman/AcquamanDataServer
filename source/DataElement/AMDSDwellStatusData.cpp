#include "AMDSDwellStatusData.h"



/// =============== implementation of AMDSStatusData ================
AMDSDwellStatusData::AMDSDwellStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime)
{
	setStatusData(fastCounts, slowCounts, detectorTemperature, accumulationTime, liveTime, realTime, generalPurposeCounter, dwellStartTime, dwellEndTime, dwellReplyTime);
}

AMDSDwellStatusData::AMDSDwellStatusData(const AMDSDwellStatusData &other)
{
	this->operator =(other);
}

AMDSDwellStatusData::~AMDSDwellStatusData()
{

}

AMDSDwellStatusData& AMDSDwellStatusData::operator +(const AMDSDwellStatusData &other)
{
	int fastCountsPlus = fastCounts() + other.fastCounts();
	int slowCountsPlus = slowCounts() + other.slowCounts();
	double detectorTemperaturePlus = detectorTemperature() + other.detectorTemperature();
	double accumulationTimePlus = accumulationTime() + other.accumulationTime();
	double liveTimePlus = liveTime() + other.liveTime();
	double realTimePlus = realTime() + other.realTime();
	int generalPurposeCounterPlus = generalPurposeCounter() + other.generalPurposeCounter();

	QTime dwellStartTimePlus =  dwellStartTime();
	QTime dwellEndTimePlus =  dwellEndTime();
	if(dwellEndTime() < other.dwellEndTime())
		dwellEndTimePlus.setHMS(other.dwellEndTime().hour(), other.dwellEndTime().minute(), other.dwellEndTime().second(), other.dwellEndTime().msec());
	else if(dwellStartTime() < other.dwellStartTime())
		dwellStartTimePlus.setHMS(other.dwellStartTime().hour(), other.dwellStartTime().minute(), other.dwellStartTime().second(), other.dwellStartTime().msec());

	QTime dwellReplyTimePlus = dwellReplyTime().addMSecs(other.dwellEndTime().msecsTo(other.dwellReplyTime()));

	setStatusData(fastCountsPlus, slowCountsPlus, detectorTemperaturePlus, accumulationTimePlus, liveTimePlus, realTimePlus, generalPurposeCounterPlus, dwellStartTimePlus, dwellEndTimePlus, dwellReplyTimePlus);
	return *this;
}

AMDSDwellStatusData& AMDSDwellStatusData::operator /(int divisor)
{
	int fastCountsDiv = fastCounts() / divisor;
	int slowCountsDiv = slowCounts() / divisor;
	double detectorTemperatureDiv = detectorTemperature() / divisor;
	double accumulationTimeDiv = accumulationTime() / divisor;
	double liveTimeDiv = liveTime() / divisor;
	double realTimeDiv = realTime() / divisor;
	int generalPurposeCounterDiv = generalPurposeCounter() / divisor;

	setStatusData(fastCountsDiv, slowCountsDiv, detectorTemperatureDiv, accumulationTimeDiv, liveTimeDiv, realTimeDiv, generalPurposeCounterDiv, dwellStartTime(), dwellEndTime(), dwellReplyTime());
	return *this;
}

AMDSDwellStatusData& AMDSDwellStatusData::operator =(const AMDSDwellStatusData &other)
{
	if(this != &other){
		setStatusData( other.fastCounts(), other.slowCounts(), other.detectorTemperature(), other.accumulationTime(), other.liveTime(), other.realTime(), other.generalPurposeCounter(), other.dwellStartTime(), other.dwellEndTime(), other.dwellReplyTime());
	}
	return *this;
}

bool AMDSDwellStatusData::readFromDataStream(QDataStream *dataStream)
{
	*dataStream >> fastCounts_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> slowCounts_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> detectorTemperature_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> accumulationTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> liveTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> realTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> generalPurposeCounter_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> dwellStartTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> dwellEndTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream >> dwellReplyTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSDwellStatusData::writeToDataStream(QDataStream *dataStream)
{
	*dataStream << fastCounts_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << slowCounts_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << detectorTemperature_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << accumulationTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << liveTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << realTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << generalPurposeCounter_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << dwellStartTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << dwellEndTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << dwellReplyTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

QString AMDSDwellStatusData::toString() const
{
//	return QString("Status: %1 %2 %3 %4").arg(dwellStartTime_.toString()).arg(dwellEndTime_.toString()).arg(dwellReplyTime_.toString()).arg(liveTime_);
	return QString("Status: %1 %2 %3 %4 %5").arg(dwellStartTime_.toString("hh:mm:ss.zzz")).arg(dwellEndTime_.toString("hh:mm:ss.zzz")).arg(dwellReplyTime_.toString("hh:mm:ss.zzz")).arg(liveTime_).arg(generalPurposeCounter_);
}

void AMDSDwellStatusData::setStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime)
{
	fastCounts_ = fastCounts ;
	slowCounts_ = slowCounts ;
	detectorTemperature_ = detectorTemperature ;
	accumulationTime_ = accumulationTime ;
	liveTime_ = liveTime ;
	realTime_ = realTime ;
	generalPurposeCounter_ = generalPurposeCounter ;

	dwellStartTime_.setHMS(dwellStartTime.hour(), dwellStartTime.minute(), dwellStartTime.second(), dwellStartTime.msec());
	dwellEndTime_.setHMS(dwellEndTime.hour(), dwellEndTime.minute(), dwellEndTime.second(), dwellEndTime.msec());
	dwellReplyTime_.setHMS(dwellReplyTime.hour(), dwellReplyTime.minute(), dwellReplyTime.second(), dwellReplyTime.msec());
}
