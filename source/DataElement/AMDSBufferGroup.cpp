#include "AMDSBufferGroup.h"

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

void AMDSBufferGroup::flattenData(QList<AMDSDataHolder *> *dataArray)
{
	if (!bufferGroupInfo_.flattenEnabled() || bufferGroupInfo_.flattenMethod() == AMDSBufferGroupInfo::NoFlatten) {
		AMDSErrorMon::alert(this, AMDS_SERVER_ALT_BUFFER_GROUP_DISABLE_FLATTEN, QString("The given buffergroup (%1) doesn't enable flatten feature or the flatten method is %2.").arg(bufferGroupInfo_.name()).arg(bufferGroupInfo_.flattenMethod()));
		return;
	}

	// make the summarization operation
	int totalDataSize = dataArray->size();
	AMDSDataHolder *flattenDataHolder = dataArray->at(0);
	for (int i = 1; i < totalDataSize; i++) {
		AMDSDataHolder * dataHolder = dataArray->at(i);
		if (flattenDataHolder && dataHolder) {
			flattenDataHolder = (*flattenDataHolder) + (*dataHolder);
		}
	}

	if (flattenDataHolder) {
		switch (bufferGroupInfo_.flattenMethod()) {
		case AMDSBufferGroupInfo::Summary:
			break; // do nothing, since we already summrized the data
		case AMDSBufferGroupInfo::Average:
			flattenDataHolder = (*flattenDataHolder) / totalDataSize;
			break;
		default:
			break;
		}
	}

	dataArray->clear();
	dataArray->append(flattenDataHolder);
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

	if (clientDataRequest->flattenResultData()) {
		flattenData(&targetDataArray);
	}

	foreach (AMDSDataHolder * dataHolder, targetDataArray) {
		clientDataRequest->appendData(dataHolder);
	}
}

void AMDSBufferGroup::populateData(AMDSClientStartTimePlusCountDataRequest* clientDataRequest)
{
	QDateTime startTime = clientDataRequest->startTime();
	quint64 count = clientDataRequest->count();

	int startIndex = getDataIndexByDateTime(startTime);

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

	int startIndex = getDataIndexByDateTime(startTime);
	int endIndex = getDataIndexByDateTime(endTime);

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

	int middleIndex = getDataIndexByDateTime(middleTime);
	if(middleIndex == -1) {
		clientDataRequest->setErrorMessage(QString("Could not locate data for middle time %1 (ReqType: 5)").arg(middleTime.toString()));
	} else {
		populateData(clientDataRequest, middleIndex - countBefore, middleIndex + countAfter);
	}
}

void AMDSBufferGroup::populateData(AMDSClientContinuousDataRequest *clientDataRequest)
{
	int startIndex = getDataIndexByDateTime(clientDataRequest->lastFetchTime());
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

int AMDSBufferGroup::getDataIndexByDateTime(const QDateTime &dwellTime)
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
