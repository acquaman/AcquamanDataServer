#include "AMDSBufferGroupManager.h"

#include "source/DataElement/AMDSBufferGroup.h"

AMDSBufferGroupManager::AMDSBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize);
	bufferGroupThread_= new QThread();
	bufferGroup_->moveToThread(bufferGroupThread_);

	connect(bufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));

	bufferGroupThread_->start();
}

AMDSBufferGroupManager::~AMDSBufferGroupManager()
{
	if (bufferGroupThread_->isRunning())
		bufferGroupThread_->quit();

	bufferGroup_->deleteLater();
	bufferGroupThread_->deleteLater();
}

AMDSBufferGroupInfo AMDSBufferGroupManager::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->bufferGroupInfo();
}

AMDSBufferGroup * AMDSBufferGroupManager::bufferGroup() const
{
	return bufferGroup_;
}

QString AMDSBufferGroupManager::bufferGroupName() const
{
	return bufferGroupInfo().name();
}

void AMDSBufferGroupManager::append(AMDSDataHolder *value)
{
	bufferGroup_->append(value);
}

void AMDSBufferGroupManager::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}
