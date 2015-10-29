#include "AMDSThreadedBufferGroup.h"

#include "DataElement/AMDSBufferGroup.h"
#include "DataHolder/AMDSSpectralDataHolder.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "util/AMErrorMonitor.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(const AMDSBufferGroupInfo &bufferGroupInfo, quint64 maxCountSize, bool enableCumulative, QObject *parent) :
	QObject(parent)
{
	bufferGroupThread_= new QThread();

	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize, enableCumulative);
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

void AMDSThreadedBufferGroup::append(AMDSDataHolder *value, bool elapsedDwellTime)
{
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(value, elapsedDwellTime);
}

void AMDSThreadedBufferGroup::clear() {
	QWriteLocker writeLock(&lock_);
	bufferGroup_->clear();
}

void AMDSThreadedBufferGroup::finishDwellDataUpdate(double elapsedTime)
{
	QReadLocker readLock(&lock_);
	bufferGroup_->finishDwellDataUpdate(elapsedTime);
}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}

void AMDSThreadedBufferGroup::forwardClientRequest(AMDSClientRequest *clientRequest)
{
	bufferGroup_->processClientRequest(clientRequest);
}

