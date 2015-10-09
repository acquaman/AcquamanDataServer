#ifndef AMDSTHREADEDBUFFERGROUP_H
#define AMDSTHREADEDBUFFERGROUP_H

#include <QObject>
#include <QThread>

#include "DataElement/AMDSStatusData.h"
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

signals:
	/// signal to indicate the bufferGroupManager is ready to use
	void bufferGroupReady();
	/// Signal which indicates that a request for data has been processed and is ready to be sent back to the client
	void clientRequestProcessed(AMDSClientRequest *clientRequest);

	/// signal to indicate that the new data added for conitunous monitor
	void continuousDataUpdate(AMDSDataHolder *continuousDataHolder);
	/// signal to indicate that the new status update for conitunous monitor
	void continuousStatusDataUpdate(AMDSStatusData statusData, int count);
	/// signal to indicate that the new update for conitunous monitor
	void continuousAllDataUpdate(AMDSDataHolder *continuousDataHolder, AMDSStatusData statusData, int count, double elapsedTime);

	/// signal to indicate that dwell update is finished, with the elapsedTime
	void dwellFinishedTimeUpdate(double elapsedTime);
	/// signal to indicate that dwell update is finished, with the dataHolder
	void dwellFinishedDataUpdate(AMDSDataHolder *accumulateDataHolder);
	/// signal to indicate that dwell update is finished, with the statusData
	void dwellFinishedStatusDataUpdate(AMDSStatusData statusData, int count);
	/// signal to indicate that dwell update is finished, with all data
	void dwellFinishedAllDataUpdate(AMDSDataHolder *accumlatedDataHolder, AMDSStatusData statusData, int count, double elapsedTime);

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
