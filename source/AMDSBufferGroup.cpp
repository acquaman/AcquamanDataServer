#include "source/AMDSBufferGroup.h"

#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"

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
	case AMDSClientRequestDefinitions::Continuous:
//		populateData(request, request->time1());
		break;
	case AMDSClientRequestDefinitions::InvalidRequest:
	default:
		break;
	}
	emit clientRequestProcessed(clientRequest);
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest* clientDataRequest, int startIndex, int endIndex)
{
	clientDataRequest->clearData();
	clientDataRequest->setBufferGroupInfo(bufferGroupInfo_);

	int dataStartIndex = (startIndex < 0 ? 0 : startIndex);
	int dataEndIndex = (endIndex < dataHolders_.count() ? endIndex : dataHolders_.count() - 1);

	for (int iCurrent = dataStartIndex; iCurrent < dataEndIndex; iCurrent++)
	{
		AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
		AMDSFlatArray oneFlatArray;
		dataHolder->data(&oneFlatArray);
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

void AMDSBufferGroup::populateData(AMDSClientContinuousDataRequest *clientContinuousDataRequest, const QDateTime &lastFetch)
{
	Q_UNUSED(clientContinuousDataRequest)
	Q_UNUSED(lastFetch)
//	int startIndex = lowerBound(lastFetch);

//	if(startIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(lastFetch.toString()));
//	else
//	{
//		// Since the last fetch actually included the data at the given time, we need to increment the index
//		// by one, to start from the one following:
//		startIndex++;
//		for(int iCurrent = startIndex, size = dataHolders_.count(); iCurrent < size; iCurrent++)
//		{
//			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
////			request->histogramData()->append(dataHolder);
//		}
//	}
}

//void AMDSBufferGroup::populateData(AMDSClientDataRequestV1 *request, const QDateTime &lastFetch)
//{
//	int startIndex = lowerBound(lastFetch);

//	if(startIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(lastFetch.toString()));
//	else
//	{
//		// Since the last fetch actually included the data at the given time, we need to increment the index
//		// by one, to start from the one following:
//		startIndex++;
//		for(int iCurrent = startIndex, size = dataHolders_.count(); iCurrent < size; iCurrent++)
//		{
//			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
////			request->histogramData()->append(dataHolder);
//		}
//	}
//}

//void AMDSBufferGroup::populateData(AMDSClientDataRequestV1* request, const QDateTime& startTime, int count)
//{
//	qDebug() << "Received request to populate data for start time plus count as " << startTime << count;

//	int startIndex = lowerBound(startTime);

//	if(startIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
//	else
//	{
//		count = startIndex + count;
//		request->clearData();
//		request->clearBufferGroupInfos();
//		request->appendBufferGroupInfo(bufferGroupInfo_);
//		qDebug() << "Located data starting at index " << startIndex << " going to index " << count;
//		for (int iCurrent = startIndex, limit = dataHolders_.count(); iCurrent < count && iCurrent < limit; iCurrent++)
//		{
//			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//			request->appendData(dataHolder);
////			request->histogramData()->append(dataHolders_[iCurrent]);
//		}
//	}
//}
//void AMDSBufferGroup::populateData(AMDSClientDataRequestV1* request, int relativeCount, int count)
//{
//	int startIndex = dataHolders_.count() - 1 - relativeCount - count;
//	int endIndex = dataHolders_.count() - 1 - relativeCount + count;
//	if(startIndex < 0)
//		startIndex = 0;

//	if(endIndex >= dataHolders_.count())
//		endIndex = dataHolders_.count() - 1;

//	for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//	{
//		AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
////		request->histogramData()->append(dataHolders_[iCurrent]);
//	}
//}

//void AMDSBufferGroup::populateData(AMDSClientDataRequestV1* request, const QDateTime& startTime, const QDateTime& endTime)
//{
//	int startIndex = lowerBound(startTime);
//	int endIndex = lowerBound(endTime);

//	if(startIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
//	else if(endIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(endTime.toString()));
//	else
//	{
//		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//		{
//			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
////			request->histogramData()->append(dataHolders_[iCurrent]);
//		}
//	}
//}

//void AMDSBufferGroup::populateData(AMDSClientDataRequestV1* request, const QDateTime& middleTime, int countBefore, int countAfter)
//{
//	int middleIndex = lowerBound(middleTime);
//	if(middleIndex == -1)
//		request->setErrorMessage(QString("Could not locate data for time %1").arg(middleTime.toString()));
//	else
//	{
//		int startIndex = middleIndex - countBefore;
//		if(startIndex < 0)
//			startIndex = 0;

//		int endIndex = middleIndex + countAfter;
//		if(endIndex >= dataHolders_.count())
//			endIndex = dataHolders_.count() - 1;

//		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
//		{
//			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
////			request->histogramData()->append(dataHolders_[iCurrent]);
//		}
//	}
//}

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
