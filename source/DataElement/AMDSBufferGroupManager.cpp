#include "AMDSBufferGroupManager.h"

#include "source/DataElement/AMDSBufferGroup.h"

AMDSBufferGroupManager::AMDSBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize);
	bufferGroupThread_= new QThread(this);
	bufferGroup_->moveToThread(bufferGroupThread_);
	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));
	bufferGroupThread_->start();
}

AMDSBufferGroupInfo AMDSBufferGroupManager::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->bufferGroupInfo();
}

AMDSBufferGroup * AMDSBufferGroupManager::bufferGroup() const
{
	return bufferGroup_;
}

void AMDSBufferGroupManager::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}
