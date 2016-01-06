#ifndef AMDSTHREADEDBUFFERGROUPMANAGER_H
#define AMDSTHREADEDBUFFERGROUPMANAGER_H

#include <QObject>
#include <QThread>

#include "DataElement/AMDSDwellStatusData.h"
#include "DataElement/AMDSBufferGroup.h"

class AMDSClientRequest;

class AMDSThreadedBufferGroupManager : public QObject
{
	Q_OBJECT
public:
	AMDSThreadedBufferGroupManager(const AMDSBufferGroupInfo &bufferGroupInfo, quint64 maxCountSize, QObject *parent = 0);
	~AMDSThreadedBufferGroupManager();

	/// returns the bufferGroupInfo of the bufferGroupManager
	AMDSBufferGroupInfo bufferGroupInfo() const;
	/// returns the bufferGroup name
	QString bufferGroupName() const;
	/// appends a list of dataHolder to the bufferGroup
	void append(const AMDSDataHolderList &dataHolderList);
	/// appends a data to the bufferGroup
	void append(AMDSDataHolder *value);

	/// returns the buffergroup
	inline AMDSBufferGroup *bufferGroup() { return bufferGroup_; }

signals:
	/// signal to indicate the bufferGroupManager is ready to use
	void bufferGroupReady();

public slots:
	/// to clear the data of the buffer group
	void clearBufferGroup();

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

#endif // AMDSTHREADEDBUFFERGROUPMANAGER_H
