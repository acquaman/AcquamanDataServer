#ifndef AMPTEKSDD123THREADEDBUFFERGROUPMANAGER_H
#define AMPTEKSDD123THREADEDBUFFERGROUPMANAGER_H

#include <QTime>

#include "DataElement/AMDSThreadedBufferGroup.h"

class AmptekSDD123ThreadedBufferGroupManager : public AMDSThreadedBufferGroup
{
	Q_OBJECT
public:
	AmptekSDD123ThreadedBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, bool enableCumulative=false, QObject *parent = 0);

	/// appends a data to the Dwell bufferGroup of the bufferGroupManager
	void appendDwellData(AMDSDataHolder *value) { dwellData_->append(value); }
	void clearDwellData() { dwellData_->clear(); }

	AMDSBufferGroup *dwellData() { return dwellData_; }

public slots:
	void onSpectrumEvent(QEvent *e);

protected:

	bool dwellActive_;
	double dwellTime_;
	bool setPresetDwellEndTimeOnNextEvent_;
	QTime presetDwellEndTime_;
	QTime presetDwellLocalStartTime_;
	QTime presetDwellLocalEndTime_;

	/// the instance of the bufferGroup
	AMDSBufferGroup *dwellData_;

};

#endif // AMPTEKSDD123THREADEDBUFFERGROUPMANAGER_H
