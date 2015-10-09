#ifndef AMDSDWELLSTATUSDATA_H
#define AMDSDWELLSTATUSDATA_H

#include <QTime>

class AMDSDwellStatusData
{
public:
	AMDSDwellStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime());
	AMDSDwellStatusData(const AMDSDwellStatusData &other);
	AMDSDwellStatusData& operator =(const AMDSDwellStatusData &other);

	int fastCounts_;
	int slowCounts_;
	double detectorTemperature_;
	double accumulationTime_;
	double liveTime_;
	double realTime_;
	int generalPurposeCounter_;
	QTime dwellStartTime_;
	QTime dwellEndTime_;
	QTime dwellReplyTime_;
};

#endif // AMDSDWELLSTATUSDATA_H
