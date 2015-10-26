#ifndef AMDSTHREADEDBUFFERGROUP_H
#define AMDSTHREADEDBUFFERGROUP_H

#include <QObject>
#include <QThread>

#include "DataElement/AMDSDwellStatusData.h"
#include "DataElement/AMDSBufferGroup.h"

class AMDSClientRequest;

class AMDSThreadedBufferGroup : public QObject
{
	Q_OBJECT
public:
	AMDSThreadedBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, bool enableCumulative = false, QObject *parent = 0);
	~AMDSThreadedBufferGroup();

	/// returns the bufferGroupInfo of the bufferGroupManager
	AMDSBufferGroupInfo bufferGroupInfo() const;
	/// returns the bufferGroup name
	QString bufferGroupName() const;
	/// appends a data to the bufferGroup
	void append(AMDSDataHolder *value, bool elapsedDwellTime=0);
	/// to clear the data of the buffer group
	void clear();

	/// returns the buffergroup
	inline AMDSBufferGroup *bufferGroup() { return bufferGroup_; }

signals:
	/// signal to indicate the bufferGroupManager is ready to use
	void bufferGroupReady();

public slots:
	/// slot to finish dwell data update
	void finishDwellDataUpdate(double elapsedTime=0);
	/// slot to forward clientRequest to the bufferGroup
	void forwardClientRequest(AMDSClientRequest *clientRequest);

protected slots:
	/// Slot to handle the threated started signal
	void onBufferGroupThreadStarted();

protected:
	/// the instance of the bufferGroup
	AMDSBufferGroup *bufferGroup_;

	/// the instance of thread to manage the bufferGroup
	QThread *bufferGroupThread_;
	/// the RW lock of the thread
	mutable QReadWriteLock lock_;
};

#endif // AMDSTHREADEDBUFFERGROUP_H
