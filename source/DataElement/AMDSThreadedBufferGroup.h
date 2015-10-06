#ifndef AMDSTHREADEDBUFFERGROUP_H
#define AMDSTHREADEDBUFFERGROUP_H

#include <QObject>
#include <QThread>

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
	/// returns the bufferGroup of the bufferGroupManager
	AMDSBufferGroup * bufferGroup() const;
	/// returns the bufferGroup name
	QString bufferGroupName() const;
	/// appends a data to the bufferGroup of the bufferGroupManager
	void append(AMDSDataHolder *value);
	/// to clear the data of the buffer group
	void clear();


signals:
	/// signal to indicate the bufferGroupManager is ready to use
	void bufferGroupReady();
	/// Signal which indicates that a request for data has been processed and is ready to be sent back to the client
	void clientRequestProcessed(AMDSClientRequest *clientRequest);

public slots:
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
