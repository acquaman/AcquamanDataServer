#ifndef AMDSDWELLSTATUSDATA_H
#define AMDSDWELLSTATUSDATA_H

#include <QTime>
#include <QDataStream>

class AMDSDwellStatusData
{
public:
	AMDSDwellStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime());
	AMDSDwellStatusData(const AMDSDwellStatusData &other);
	~AMDSDwellStatusData();

	/// the + operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator +(const AMDSDwellStatusData &other);
	/// the / operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator /(int divisor);

	/// the assign ( = ) operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator =(const AMDSDwellStatusData &other);

	/// read data from dataStream and write to the dwellStatusData
	bool readFromDataStream(QDataStream *dataStream);
	/// write status data of the dataArray to dataStream
	bool writeToDataStream(QDataStream *dataStream);

	/// print the result of DwellStatusData to a string
	QString toString() const;

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
	/// returns the value of dwellStartTime
	inline QTime dwellStartTime() const { return dwellStartTime_; }
	/// returns the value of dwellEndTime
	inline QTime dwellEndTime() const { return dwellEndTime_; }
	/// returns the value of dwellReplyTime
	inline QTime dwellReplyTime() const { return dwellReplyTime_; }

protected:
	/// helper function to update all the properties
	void setStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime);

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
	QTime dwellStartTime_;
	/// the value of dwellEndTime
	QTime dwellEndTime_;
	/// the value of dwellReplyTime
	QTime dwellReplyTime_;
};

#endif // AMDSDWELLSTATUSDATA_H
