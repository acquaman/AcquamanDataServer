#ifndef AMDSSTATUSDATA_H
#define AMDSSTATUSDATA_H

#include <QTime>

class AMDSStatusData
{
public:
	AMDSStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime());
	AMDSStatusData(const AMDSStatusData &other);
	AMDSStatusData& operator =(const AMDSStatusData &other);

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

#endif // AMDSSTATUSDATA_H
