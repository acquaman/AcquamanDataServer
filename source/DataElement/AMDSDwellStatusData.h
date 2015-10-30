#ifndef AMDSDWELLSTATUSDATA_H
#define AMDSDWELLSTATUSDATA_H

#include <QTime>
#include <QDataStream>

class AMDSDwellStatusData
{
public:
	AMDSDwellStatusData(quint32 fastCounts = -1, quint32 slowCounts = -1, quint64 detectorTemperature = -1, quint64 accumulationTime = -1, quint64 liveTime = -1, quint64 realTime = -1, quint32 generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime());
	AMDSDwellStatusData(const AMDSDwellStatusData &other);
	~AMDSDwellStatusData();

	/// the + operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator +(const AMDSDwellStatusData &other);
	/// the / operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator /(quint32 divisor);

	/// the assign ( = ) operation of ADMSDwellStatusData
	AMDSDwellStatusData& operator =(const AMDSDwellStatusData &other);

	/// read data from dataStream and write to the dwellStatusData
	bool readFromDataStream(QDataStream *dataStream);
	/// write status data of the dataArray to dataStream
	bool writeToDataStream(QDataStream *dataStream);

	/// print the result of DwellStatusData to a string
	QString toString() const;

	/// returns the vlaue of fastCounts
	inline quint32 fastCounts() const { return fastCounts_; }
	/// returns the value of slowCounts
	inline quint32 slowCounts() const { return slowCounts_; }
	/// returns the value of detectorTemperature
	inline quint64 detectorTemperature() const { return detectorTemperature_; }
	/// returns the value of accumulationTime
	inline quint64 accumulationTime() const { return accumulationTime_; }
	/// returns the value of liveTime
	inline quint64 liveTime() const { return liveTime_; }
	/// returns the value of realTime
	inline quint64 realTime() const { return realTime_; }
	/// returns the value of generalPurposeCounter
	inline quint32 generalPurposeCounter() const { return generalPurposeCounter_; }
	/// returns the value of dwellStartTime
	inline QTime dwellStartTime() const { return dwellStartTime_; }
	/// returns the value of dwellEndTime
	inline QTime dwellEndTime() const { return dwellEndTime_; }
	/// returns the value of dwellReplyTime
	inline QTime dwellReplyTime() const { return dwellReplyTime_; }

protected:
	/// helper function to update all the properties
	void setStatusData(quint32 fastCounts, quint32 slowCounts, quint64 detectorTemperature, quint64 accumulationTime, quint64 liveTime, quint64 realTime, quint32 generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime);

protected:
	/// the value of fastCounts
	quint32 fastCounts_;
	/// the value of slowCounts
	quint32 slowCounts_;
	/// the value of detectorTemperature
	quint64 detectorTemperature_;
	/// the value of accumulationTime
	quint64 accumulationTime_;
	/// the value of liveTime
	quint64 liveTime_;
	/// the value of realTime
	quint64 realTime_;
	/// the value of generalPurposeCounter
	quint32 generalPurposeCounter_;
	/// the value of dwellStartTime
	QTime dwellStartTime_;
	/// the value of dwellEndTime
	QTime dwellEndTime_;
	/// the value of dwellReplyTime
	QTime dwellReplyTime_;
};

#endif // AMDSDWELLSTATUSDATA_H
