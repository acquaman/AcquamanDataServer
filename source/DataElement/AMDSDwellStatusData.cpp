#include "AMDSDwellStatusData.h"

/// =============== implementation of AMDSStatusData ================
AMDSDwellStatusData::AMDSDwellStatusData(quint32 fastCounts, quint32 slowCounts, quint64 detectorTemperature, quint64 accumulationTime, quint64 liveTime, quint64 realTime, quint32 generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime)
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
	quint32 fastCountsPlus = fastCounts() + other.fastCounts();
	quint32 slowCountsPlus = slowCounts() + other.slowCounts();
	quint64 detectorTemperaturePlus = detectorTemperature() + other.detectorTemperature();
	quint64 accumulationTimePlus = accumulationTime() + other.accumulationTime();
	quint64 liveTimePlus = liveTime() + other.liveTime();
	quint64 realTimePlus = realTime() + other.realTime();
	quint32 generalPurposeCounterPlus = generalPurposeCounter() + other.generalPurposeCounter();

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

AMDSDwellStatusData& AMDSDwellStatusData::operator /(quint32 divisor)
{
	quint32 fastCountsDiv = fastCounts() / divisor;
	quint32 slowCountsDiv = slowCounts() / divisor;
	quint64 detectorTemperatureDiv = detectorTemperature() / divisor;
	quint64 accumulationTimeDiv = accumulationTime() / divisor;
	quint64 liveTimeDiv = liveTime() / divisor;
	quint64 realTimeDiv = realTime() / divisor;
	quint32 generalPurposeCounterDiv = generalPurposeCounter() / divisor;

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

bool AMDSDwellStatusData::writeToDataStream(QDataStream *dataStream) const
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
	return QString("Status: %1 %2 %3 %4").arg(dwellStartTime_.toString()).arg(dwellEndTime_.toString()).arg(dwellReplyTime_.toString()).arg(liveTime_);
}

void AMDSDwellStatusData::setStatusData(quint32 fastCounts, quint32 slowCounts, quint64 detectorTemperature, quint64 accumulationTime, quint64 liveTime, quint64 realTime, quint32 generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime)
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
