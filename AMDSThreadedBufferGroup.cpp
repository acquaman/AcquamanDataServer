#include "AMDSThreadedBufferGroup.h"

#include "AMDSBufferGroup.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(AMDSBufferGroup *bufferGroup, QObject *parent) :
	QObject(parent)
{
	bufferGroup_ = bufferGroup;
	bufferGroupThread_= new QThread(this);
	bufferGroup_->moveToThread(bufferGroupThread_);
	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));
	bufferGroupThread_->start();
}

AMDSBufferGroupInfo AMDSThreadedBufferGroup::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->info();
}

void AMDSThreadedBufferGroup::requestData(AMDSClientDataRequest *dataRequest){

}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}
