#include "AMDSStatusData.h"

/// =============== implementation of AMDSStatusData ================
AMDSStatusData::AMDSStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime) :
	fastCounts_(fastCounts), slowCounts_(slowCounts), detectorTemperature_(detectorTemperature), accumulationTime_(accumulationTime), liveTime_(liveTime), realTime_(realTime), generalPurposeCounter_(generalPurposeCounter)
{
	dwellStartTime_.setHMS(dwellStartTime.hour(), dwellStartTime.minute(), dwellStartTime.second(), dwellStartTime.msec());
	dwellEndTime_.setHMS(dwellEndTime.hour(), dwellEndTime.minute(), dwellEndTime.second(), dwellEndTime.msec());
	dwellReplyTime_.setHMS(dwellReplyTime.hour(), dwellReplyTime.minute(), dwellReplyTime.second(), dwellReplyTime.msec());
}

AMDSStatusData::AMDSStatusData(const AMDSStatusData &other)
{
	this->operator =(other);
}

AMDSStatusData& AMDSStatusData::operator =(const AMDSStatusData &other)
{
	if(this != &other){
		fastCounts_ = other.fastCounts_;
		slowCounts_ = other.slowCounts_;
		detectorTemperature_ = other.detectorTemperature_;
		accumulationTime_ = other.accumulationTime_;
		liveTime_ = other.liveTime_;
		realTime_ = other.realTime_;
		generalPurposeCounter_ = other.generalPurposeCounter_;
		dwellStartTime_.setHMS(other.dwellStartTime_.hour(), other.dwellStartTime_.minute(), other.dwellStartTime_.second(), other.dwellStartTime_.msec());
		dwellEndTime_.setHMS(other.dwellEndTime_.hour(), other.dwellEndTime_.minute(), other.dwellEndTime_.second(), other.dwellEndTime_.msec());
		dwellReplyTime_.setHMS(other.dwellReplyTime_.hour(), other.dwellReplyTime_.minute(), other.dwellReplyTime_.second(), other.dwellReplyTime_.msec());

	}
	return *this;
}
