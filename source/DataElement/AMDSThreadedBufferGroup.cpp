#include "AMDSThreadedBufferGroup.h"

#include "DataElement/AMDSBufferGroup.h"
#include "DataHolder/AMDSSpectralDataHolder.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "util/AMDSRunTimeSupport.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(const AMDSBufferGroupInfo &bufferGroupInfo, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	bufferGroupThread_= new QThread();

	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize);
	bufferGroup_->moveToThread(bufferGroupThread_);

	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));
	connect(bufferGroupThread_, SIGNAL(finished()), bufferGroup_, SLOT(deleteLater()));

	bufferGroupThread_->start();
}

AMDSThreadedBufferGroup::~AMDSThreadedBufferGroup()
{
	if (bufferGroupThread_->isRunning())
		bufferGroupThread_->quit();

	bufferGroupThread_->deleteLater();
}

AMDSBufferGroupInfo AMDSThreadedBufferGroup::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->bufferGroupInfo();
}

QString AMDSThreadedBufferGroup::bufferGroupName() const
{
	return bufferGroupInfo().name();
}

void AMDSThreadedBufferGroup::append(const AMDSDataHolderList &dataHolderList)
{
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(dataHolderList);
}

void AMDSThreadedBufferGroup::append(AMDSDataHolder *value)
{
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(value);
}

void AMDSThreadedBufferGroup::clearBufferGroup() {
	QWriteLocker writeLock(&lock_);

	if (bufferGroup_)
		bufferGroup_->clear();

}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}

void AMDSThreadedBufferGroup::forwardClientRequest(AMDSClientRequest *clientRequest)
{
	bufferGroup_->processClientRequest(clientRequest);
}

