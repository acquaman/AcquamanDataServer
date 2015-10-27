#include "AMDSDwellStatusData.h"

/// =============== implementation of AMDSStatusData ================
AMDSDwellStatusData::AMDSDwellStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QDateTime &dwellStartDateTime, const QDateTime &dwellEndDateTime, const QDateTime &dwellReplyDateTime)
{
	setStatusData(fastCounts, slowCounts, detectorTemperature, accumulationTime, liveTime, realTime, generalPurposeCounter, dwellStartDateTime, dwellEndDateTime, dwellReplyDateTime);
}

AMDSDwellStatusData::AMDSDwellStatusData(const AMDSDwellStatusData &other)
{
	this->operator =(other);
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

	QDateTime dwellStartDateTimePlus =  dwellStartDateTime();
	QDateTime dwellEndDateTimePlus =  dwellEndDateTime();
	if(dwellEndDateTime() < other.dwellEndDateTime())
		dwellEndDateTimePlus = other.dwellEndDateTime();
	else if(dwellStartDateTime() < other.dwellStartDateTime())
		dwellStartDateTimePlus = other.dwellStartDateTime();

	QDateTime dwellReplyDateTimePlus = dwellReplyDateTime().addMSecs(other.dwellEndDateTime().msecsTo(other.dwellReplyDateTime()));

	setStatusData(fastCountsPlus, slowCountsPlus, detectorTemperaturePlus, accumulationTimePlus, liveTimePlus, realTimePlus, generalPurposeCounterPlus, dwellStartDateTimePlus, dwellEndDateTimePlus, dwellReplyDateTimePlus);
	return *this;
}

AMDSDwellStatusData& AMDSDwellStatusData::operator /(quint32 divisor)
{
	int fastCountsDiv = fastCounts() / divisor;
	int slowCountsDiv = slowCounts() / divisor;
	double detectorTemperatureDiv = detectorTemperature() / divisor;
	double accumulationTimeDiv = accumulationTime() / divisor;
	double liveTimeDiv = liveTime() / divisor;
	double realTimeDiv = realTime() / divisor;
	int generalPurposeCounterDiv = generalPurposeCounter() / divisor;

	setStatusData(fastCountsDiv, slowCountsDiv, detectorTemperatureDiv, accumulationTimeDiv, liveTimeDiv, realTimeDiv, generalPurposeCounterDiv, dwellStartDateTime(), dwellEndDateTime(), dwellReplyDateTime());
	return *this;
}

AMDSDwellStatusData& AMDSDwellStatusData::operator =(const AMDSDwellStatusData &other)
{
	if(this != &other){
		setStatusData( other.fastCounts(), other.slowCounts(), other.detectorTemperature(), other.accumulationTime(), other.liveTime(), other.realTime(), other.generalPurposeCounter(), other.dwellStartDateTime(), other.dwellEndDateTime(), other.dwellReplyDateTime());
	}
	return *this;
}

int AMDSDwellStatusData::replyMSecs() {
	QTime replyTime = dwellReplyDateTime().time();

	int dwellReplyHours = replyTime.hour();
	int dwellReplyMinutes = replyTime.minute();
	int dwellReplySeconds = replyTime.second();
	int dwellReplyMSecs = replyTime.msec();

	return dwellReplyMSecs + dwellReplySeconds*1000 + dwellReplyMinutes*1000*60 + dwellReplyHours*1000*60*60;
}

void AMDSDwellStatusData::setStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QDateTime &dwellStartDateTime, const QDateTime &dwellEndDateTime, const QDateTime &dwellReplyDateTime)
{
	fastCounts_ = fastCounts ;
	slowCounts_ = slowCounts ;
	detectorTemperature_ = detectorTemperature ;
	accumulationTime_ = accumulationTime ;
	liveTime_ = liveTime ;
	realTime_ = realTime ;
	generalPurposeCounter_ = generalPurposeCounter ;

	dwellStartDateTime_ = dwellStartDateTime;
	dwellEndDateTime_ = dwellEndDateTime;
	dwellReplyDateTime_ = dwellReplyDateTime;
}
