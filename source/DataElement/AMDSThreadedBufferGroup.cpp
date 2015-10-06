#include "AMDSThreadedBufferGroup.h"

#include "DataElement/AMDSBufferGroup.h"
#include "ClientRequest/AMDSClientDataRequest.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, bool enableCumulative, QObject *parent) :
	QObject(parent)
{
	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize, enableCumulative);
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
	QWriteLocker writeLock(&lock_);
	bufferGroup_->append(value);
}

void AMDSThreadedBufferGroup::clear() {
	QWriteLocker writeLock(&lock_);
	bufferGroup_->clear();
}

void AMDSThreadedBufferGroup::onBufferGroupThreadStarted(){
	emit bufferGroupReady();
}

void AMDSThreadedBufferGroup::forwardClientRequest(AMDSClientRequest *clientRequest)
{
	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest *>(clientRequest);
	if (clientDataRequest)
		clientDataRequest->setBufferGroupInfo(bufferGroupInfo());

	bufferGroup_->processClientRequest(clientRequest);
}

