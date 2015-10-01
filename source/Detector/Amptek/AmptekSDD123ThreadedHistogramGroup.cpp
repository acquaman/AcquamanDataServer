#include "AmptekSDD123ThreadedHistogramGroup.h"
#include "AmptekEventDefinitions.h"
AmptekSDD123ThreadedHistogramGroup::AmptekSDD123ThreadedHistogramGroup(int maxBufferSize, QObject *parent) :
	QObject(parent), allData_(maxBufferSize)
{
	allData_.moveToThread(&thread_);
	thread_.start();
	connect(&allData_, SIGNAL(dataRequestReady(AMDSClientDataRequest*)), this, SIGNAL(dataRequestReady(AMDSClientDataRequest*)));
	connect(this, SIGNAL(requestData(ClientDataRequest*)), &allData_, SLOT(requestData(ClientDataRequest*)));
}

void AmptekSDD123ThreadedHistogramGroup::append(AmptekSDD123Histogram *histogram)
{
	QWriteLocker writeLock(&lock_);
	allData_.append(histogram);
}

void AmptekSDD123ThreadedHistogramGroup::clear()
{
	QWriteLocker writeLock(&lock_);
	allData_.clear();
}

int AmptekSDD123ThreadedHistogramGroup::count() const
{
	QReadLocker readLock(&lock_);
	return allData_.count();
}
