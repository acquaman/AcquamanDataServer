#ifndef AMDSDWELLSTATUSDATA_H
#define AMDSDWELLSTATUSDATA_H

#include <QTime>

class AMDSDwellStatusData
{
public:
	AMDSDwellStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QDateTime &dwellStartDateTime = QDateTime(), const QDateTime &dwellEndDateTime = QDateTime(), const QDateTime &dwellReplyDateTime = QDateTime());
	AMDSDwellStatusData(const AMDSDwellStatusData &other);

	/// the + operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator +(const AMDSDwellStatusData &other);
	/// the / operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator /(quint32 divisor);

	/// the assign ( = ) operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator =(const AMDSDwellStatusData &other);

	/// returns the total MSecs to reply the message
	int replyMSecs();

	/// returns the vlaue of fastCounts
	inline int fastCounts() const { return fastCounts_; }
	/// returns the value of slowCounts
	inline int slowCounts() const { return slowCounts_; }
	/// returns the value of detectorTemperature
	inline double detectorTemperature() const { return detectorTemperature_; }
	/// returns the value of accumulationTime
	inline double accumulationTime() const { return accumulationTime_; }
	/// returns the value of liveTime
	inline double liveTime() const { return liveTime_; }
	/// returns the value of realTime
	inline double realTime() const { return realTime_; }
	/// returns the value of generalPurposeCounter
	inline int generalPurposeCounter() const { return generalPurposeCounter_; }
	/// returns the value of dwellStartDateTime
	inline QDateTime dwellStartDateTime() const { return dwellStartDateTime_; }
	/// returns the value of dwellEndDateTime
	inline QDateTime dwellEndDateTime() const { return dwellEndDateTime_; }
	/// returns the value of dwellReplyDateTime
	inline QDateTime dwellReplyDateTime() const { return dwellReplyDateTime_; }

	/// returns the string value of dwellStartTime
	inline QString dwellStartTime(const QString &format="hh:mm:ss.zzz") const { return dwellStartDateTime_.time().toString(format); }
	/// returns the string value of dwellEndTime
	inline QString dwellEndTime(const QString &format="hh:mm:ss.zzz") const { return dwellEndDateTime_.time().toString(format); }
	/// returns the string value of dwellReplyTime
	inline QString dwellReplyTime(const QString &format="hh:mm:ss.zzz") const { return dwellReplyDateTime_.time().toString(format); }

protected:
	/// helper function to update all the properties
	void setStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QDateTime &dwellStartDateTime, const QDateTime &dwellEndDateTime, const QDateTime &dwellReplyDateTime);

protected:
	/// the value of fastCounts
	int fastCounts_;
	/// the value of slowCounts
	int slowCounts_;
	/// the value of detectorTemperature
	double detectorTemperature_;
	/// the value of accumulationTime
	double accumulationTime_;
	/// the value of liveTime
	double liveTime_;
	/// the value of realTime
	double realTime_;
	/// the value of generalPurposeCounter
	int generalPurposeCounter_;
	/// the value of dwellStartTime
	QDateTime dwellStartDateTime_;
	/// the value of dwellEndDateTime
	QDateTime dwellEndDateTime_;
	/// the value of dwellReplyDateTime
	QDateTime dwellReplyDateTime_;
};

#endif // AMDSDWELLSTATUSDATA_H
