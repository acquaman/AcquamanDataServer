#ifndef AMPTEKSDD123BUFFERGROUPMANAGER_H
#define AMPTEKSDD123BUFFERGROUPMANAGER_H

#include <QTime>

#include "DataElement/AMDSThreadedBufferGroup.h"

class AmptekSDD123BufferGroupManager : public AMDSThreadedBufferGroup
{
	Q_OBJECT
public:
	AmptekSDD123BufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent = 0);

	bool event(QEvent *e);

public slots:
	void setRequestEventReceiver(QObject *requestEventReceiver);

protected:
	void spectrumEventHelper(QEvent *e);
	void configurationValuesEventHelper(QEvent *e);
	void configurationModeConfirmationEventHelper(QEvent *e);

protected:
	QObject *requestEventReceiver_;

	bool dwellActive_;
	double dwellTime_;
	bool setPresetDwellEndTimeOnNextEvent_;
	QTime presetDwellEndTime_;
	QTime presetDwellLocalStartTime_;
	QTime presetDwellLocalEndTime_;

	/// the instance of the bufferGroup
	AMDSBufferGroup *dwellData_;

};

#endif // AMPTEKSDD123BUFFERGROUPMANAGER_H
