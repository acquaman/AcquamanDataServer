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

AMDSBufferGroup* AMDSThreadedBufferGroup::bufferGroup(){
	return bufferGroup_;
}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}
