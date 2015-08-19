#include "source/AMDSBufferGroup.h"

#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSBufferGroup::AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, QObject *parent) :
	QObject(parent), dataHolders_(maxSize)
{
	bufferGroupInfo_ = bufferGroupInfo;
}

AMDSBufferGroup::AMDSBufferGroup(const AMDSBufferGroup& other):
	QObject(other.parent()), dataHolders_(other.dataHolders_)
{
}

void AMDSBufferGroup::processClientRequest(AMDSClientRequest *clientRequest){
	QReadLocker readLock(&lock_);

	switch(clientRequest->requestType()){
	case AMDSClientRequestDefinitions::Introspection:
		break;
	case AMDSClientRequestDefinitions::Statistics:
		break;
	case AMDSClientRequestDefinitions::StartTimePlusCount: {
		AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
		if(clientStartTimePlusCountDataRequest) {
			populateData(clientStartTimePlusCountDataRequest);
		}
		break;
	}
	case AMDSClientRequestDefinitions::RelativeCountPlusCount: {
		AMDSClientRelativeCountPlusCountDataRequest *clientRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
		if(clientRelativeCountPlusCountDataRequest) {
			populateData(clientRelativeCountPlusCountDataRequest);
		}
		break;
	}
	case AMDSClientRequestDefinitions::StartTimeToEndTime:{
		AMDSClientStartTimeToEndTimeDataRequest *clientStartTimeToEndTimeDataRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
		if(clientStartTimeToEndTimeDataRequest) {
			populateData(clientStartTimeToEndTimeDataRequest);
		}
		break;
	}
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:{
		AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest *clientMiddleTimePlusCountBeforeAndAfterDataRequest = qobject_cast<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest*>(clientRequest);
		if(clientMiddleTimePlusCountBeforeAndAfterDataRequest) {
			populateData(clientMiddleTimePlusCountBeforeAndAfterDataRequest);
		}
		break;
	}
	case AMDSClientRequestDefinitions::Continuous:{
		AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
		if(clientContinuousDataRequest) {
			populateData(clientContinuousDataRequest);
		}
		break;
	}
	case AMDSClientRequestDefinitions::InvalidRequest:
	default:
		break;
	}
	emit clientRequestProcessed(clientRequest);
}

void AMDSBufferGroup::flattenData(QList<AMDSDataHolder *> dataArray)
{
	if (bufferGroupInfo_.flattenMethod() == AMDSBufferGroupInfo::NoFlatten) {
		AMDSErrorMon::alert(this, 0, "Data flatten method is NoFlatten, but the flatten is enabled.");
		return;
	}

//	AMDSDataHolder flattenDataHolder;
//	foreach (AMDSDataHolder * dataHolder, dataArray) {

//	}

	dataArray.clear();
//	dataArray.append(flattenDataHolder);
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest* clientDataRequest, int startIndex, int endIndex)
{
	clientDataRequest->clearData();
	clientDataRequest->setBufferGroupInfo(bufferGroupInfo_);

	int dataStartIndex = qMax(0, startIndex);
	int dataEndIndex = qMin(endIndex, dataHolders_.count() - 1);

	QList<AMDSDataHolder *> targetDataArray;
	for (int iCurrent = dataStartIndex; iCurrent < dataEndIndex; iCurrent++)
	{
		AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
		AMDSFlatArray oneFlatArray;
		dataHolder->data(&oneFlatArray);

		targetDataArray.append(dataHolder);
	}

	if (bufferGroupInfo_.flattenEnabled()) {
		flattenData(targetDataArray);
	}

	foreach (AMDSDataHolder * dataHolder, targetDataArray) {
		clientDataRequest->appendData(dataHolder);
	}
}

void AMDSBufferGroup::populateData(AMDSClientStartTimePlusCountDataRequest* clientDataRequest)
{
	QDateTime startTime = clientDataRequest->startTime();
	quint64 count = clientDataRequest->count();

	int startIndex = lowerBound(startTime);

	if(startIndex == -1)
		clientDataRequest->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
	else
		populateData(clientDataRequest, startIndex, startIndex + count);
}

void AMDSBufferGroup::populateData(AMDSClientRelativeCountPlusCountDataRequest* clientDataRequest)
{
	int relativeCount = clientDataRequest->relativeCount();
	int count = clientDataRequest->count();

	int startIndex = dataHolders_.count() - 1 - relativeCount;
	populateData(clientDataRequest, startIndex, startIndex+count);
}

void AMDSBufferGroup::populateData(AMDSClientStartTimeToEndTimeDataRequest* clientDataRequest)
{
	QDateTime startTime = clientDataRequest->startTime();
	QDateTime endTime = clientDataRequest->endTime();

	int startIndex = lowerBound(startTime);
	int endIndex = lowerBound(endTime);

	if(startIndex == -1) {
		clientDataRequest->setErrorMessage(QString("Could not locate data for start time %1 (ReqType: 4)").arg(startTime.toString()));
	} else if (endIndex == -1) {
		clientDataRequest->setErrorMessage(QString("Could not locate data for end time %1 (ReqType: 4)").arg(endTime.toString()));
	} else {
		populateData(clientDataRequest, startIndex, endIndex);
	}
}

void AMDSBufferGroup::populateData(AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest* clientDataRequest)
{
	QDateTime middleTime = clientDataRequest->middleTime();
	int countBefore = clientDataRequest->countBefore();
	int countAfter = clientDataRequest->countAfter();

	int middleIndex = lowerBound(middleTime);
	if(middleIndex == -1) {
		clientDataRequest->setErrorMessage(QString("Could not locate data for middle time %1 (ReqType: 5)").arg(middleTime.toString()));
	} else {
		populateData(clientDataRequest, middleIndex - countBefore, middleIndex + countAfter);
	}
}

void AMDSBufferGroup::populateData(AMDSClientContinuousDataRequest *clientDataRequest)
{
	int startIndex = lowerBound(clientDataRequest->lastFetchTime());
	if(startIndex == -1)
		clientDataRequest->setErrorMessage(QString("Could not locate data for time %1").arg(clientDataRequest->lastFetchTime().toString()));
	else {

		QDateTime lastDataTime = dataHolders_[dataHolders_.count()-1]->eventTime();
		clientDataRequest->setLastFetchTime(lastDataTime);
		// Since the last fetch actually included the data at the given time, we need to increment the index
		// by one, to start from the one following:
		populateData(clientDataRequest, startIndex++, dataHolders_.count());
	}
}

int AMDSBufferGroup::lowerBound(const QDateTime &dwellTime)
{
	if (dataHolders_.isEmpty())
		return -1;

	int start = 0;
	int end = dataHolders_.count() -1;
	int middle = (start + end) / 2;

	bool found = false;

	while(!found)
	{
		if(middle == 0 || middle == start)
			found = true;
		else if (*(dataHolders_[middle]) == dwellTime)
		{
			while(*(dataHolders_[middle -1]) > dwellTime && middle > 0)
			{
				middle--;
			}
			found = true;
		}
		else if (*(dataHolders_[middle]) < dwellTime && *(dataHolders_[middle - 1]) > dwellTime)
			found = true;
		else if (*(dataHolders_[middle]) < dwellTime)
		{
			start = middle;
			middle = (start + end) / 2;
		}
		else if (*(dataHolders_[middle]) > dwellTime)
		{
			end = middle;
			middle = (start + end) / 2;
		}
	}

	return middle;
}
