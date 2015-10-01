#include "AMDSThreadedBufferGroup.h"

#include "source/DataElement/AMDSBufferGroup.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize);
	bufferGroupThread_= new QThread();
	bufferGroup_->moveToThread(bufferGroupThread_);

	connect(bufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));

	bufferGroupThread_->start();
}

AMDSThreadedBufferGroup::~AMDSThreadedBufferGroup()
{
	if (bufferGroupThread_->isRunning())
		bufferGroupThread_->quit();

	bufferGroup_->deleteLater();
	bufferGroupThread_->deleteLater();
}

AMDSBufferGroupInfo AMDSThreadedBufferGroup::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->bufferGroupInfo();
}

AMDSBufferGroup * AMDSThreadedBufferGroup::bufferGroup() const
{
	return bufferGroup_;
}

QString AMDSThreadedBufferGroup::bufferGroupName() const
{
	return bufferGroupInfo().name();
}

void AMDSThreadedBufferGroup::append(AMDSDataHolder *value)
{
	bufferGroup_->append(value);
}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}
