#include "AMDSThreadedBufferGroupManager.h"

#include "DataElement/AMDSBufferGroup.h"
#include "DataHolder/AMDSSpectralDataHolder.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "util/AMDSRunTimeSupport.h"

AMDSThreadedBufferGroupManager::AMDSThreadedBufferGroupManager(const AMDSBufferGroupInfo &bufferGroupInfo, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	bufferGroupThread_= new QThread();

	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize);
	bufferGroup_->moveToThread(bufferGroupThread_);

	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));
	connect(bufferGroupThread_, SIGNAL(finished()), bufferGroup_, SLOT(deleteLater()));

	bufferGroupThread_->start();
}

AMDSThreadedBufferGroupManager::~AMDSThreadedBufferGroupManager()
{
	if (bufferGroupThread_->isRunning())
		bufferGroupThread_->quit();

	bufferGroupThread_->deleteLater();
}

AMDSBufferGroupInfo AMDSThreadedBufferGroupManager::bufferGroupInfo() const{
	QReadLocker readLock(&lock_);
	return bufferGroup_->bufferGroupInfo();
}

QString AMDSThreadedBufferGroupManager::bufferGroupName() const
{
	return bufferGroupInfo().name();
}

void AMDSThreadedBufferGroupManager::append(const AMDSDataHolderList &dataHolderList)
{
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(dataHolderList);
}

void AMDSThreadedBufferGroupManager::append(AMDSDataHolder *value)
{
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(value);
}

void AMDSThreadedBufferGroupManager::clearBufferGroup() {
	QWriteLocker writeLock(&lock_);

	if (bufferGroup_)
		bufferGroup_->clear();

}

void AMDSThreadedBufferGroupManager::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}

void AMDSThreadedBufferGroupManager::forwardClientRequest(AMDSClientRequest *clientRequest)
{
	bufferGroup_->processClientRequest(clientRequest);
}

