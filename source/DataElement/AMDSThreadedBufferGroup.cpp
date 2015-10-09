#include "AMDSThreadedBufferGroup.h"

#include "DataElement/AMDSBufferGroup.h"
#include "DataHolder/AMDSDwellSpectralDataHolder.h"
#include "ClientRequest/AMDSClientDataRequest.h"

AMDSThreadedBufferGroup::AMDSThreadedBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, bool enableCumulative, QObject *parent) :
	QObject(parent)
{
	bufferGroupThread_= new QThread();

	bufferGroup_ = new AMDSBufferGroup(bufferGroupInfo, maxCountSize, enableCumulative);
	bufferGroup_->moveToThread(bufferGroupThread_);

	connect(bufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
	connect(bufferGroupThread_, SIGNAL(started()), this, SLOT(onBufferGroupThreadStarted()));

	bufferGroupThread_->start();
}

AMDSThreadedBufferGroup::~AMDSThreadedBufferGroup()
{
	QWriteLocker writeLock(&lock_);

	if (bufferGroupThread_->isRunning())
		bufferGroupThread_->quit();

	bufferGroup_->deleteLater();
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
	bufferGroup_->append(value);

	if (bufferGroup_->cumulativeEnabled()) {
		AMDSDwellSpectralDataHolder *cumulativeDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(bufferGroup_->cumulativeDataHolder());
		AMDSStatusData cumulativeStatusData = cumulativeDataHolder->dwellStatusData();

		emit continuousDataUpdate(cumulativeDataHolder);
		emit continuousStatusDataUpdate(cumulativeStatusData, bufferGroup_->count());
		emit continuousAllDataUpdate(cumulativeDataHolder, cumulativeStatusData, bufferGroup_->count(), elapsedDwellTime);
	}
}

void AMDSThreadedBufferGroup::clear() {
	QWriteLocker writeLock(&lock_);
	bufferGroup_->clear();
}

void AMDSThreadedBufferGroup::finishDwellDataUpdate(double elapsedTime)
{
	QReadLocker readLock(&lock_);

	if (bufferGroup_->cumulativeEnabled()) {
		AMDSDwellSpectralDataHolder *cumulativeDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(bufferGroup_->cumulativeDataHolder());
		AMDSStatusData cumulativeStatusData = cumulativeDataHolder->dwellStatusData();

		emit dwellFinishedTimeUpdate(elapsedTime);
		emit dwellFinishedDataUpdate(cumulativeDataHolder);
		emit dwellFinishedStatusDataUpdate(cumulativeStatusData, bufferGroup_->count());
		emit dwellFinishedAllDataUpdate(cumulativeDataHolder, cumulativeStatusData, bufferGroup_->count(), elapsedTime);
	}
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

