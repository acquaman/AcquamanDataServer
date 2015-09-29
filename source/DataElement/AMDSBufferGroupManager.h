#ifndef AMDSBUFFERGROUPMANAGER_H
#define AMDSBUFFERGROUPMANAGER_H

#include <QObject>
#include <QThread>

#include "source/DataElement/AMDSBufferGroup.h"

class AMDSClientRequest;

class AMDSBufferGroupManager : public QObject
{
	Q_OBJECT
public:
	AMDSBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent = 0);
	~AMDSBufferGroupManager();

	/// returns the bufferGroupInfo of the bufferGroupManager
	AMDSBufferGroupInfo bufferGroupInfo() const;
	/// returns the bufferGroup of the bufferGroupManager
	AMDSBufferGroup * bufferGroup() const;
	/// returns the bufferGroup name
	QString bufferGroupName() const;
	/// appends a data to the bufferGroup of the bufferGroupManager
	void append(AMDSDataHolder *value);


signals:
	/// signal to indicate the bufferGroupManager is ready to use
	void bufferGroupReady();
	/// Signal which indicates that a request for data has been processed and is ready to be sent back to the client
	void clientRequestProcessed(AMDSClientRequest *clientRequest);

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

#endif // AMDSBUFFERGROUPMANAGER_H
