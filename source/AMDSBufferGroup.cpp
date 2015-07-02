#include "source/AMDSBufferGroup.h"

#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
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
	case AMDSClientRequestDefinitions::StartTimePlusCount:{
		AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
		if(clientStartTimePlusCountDataRequest)
			populateData(clientStartTimePlusCountDataRequest);
		break;}
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
//		populateData(request, request->count1(), request->count2());
		break;
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
//		populateData(request, request->time1(), request->time2());
		break;
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
//		populateData(request, request->time1(), request->count1(), request->count2());
		break;
	case AMDSClientRequestDefinitions::Continuous:
//		populateData(request, request->time1());
		break;
	case AMDSClientRequestDefinitions::InvalidRequest:
	default:
		break;
	}
	emit clientRequestProcessed(clientRequest);
}

void AMDSBufferGroup::populateData(AMDSClientStartTimePlusCountDataRequest* clientStartTimePlusCountDataRequest)
{
	QDateTime startTime = clientStartTimePlusCountDataRequest->startTime();
	quint64 count = clientStartTimePlusCountDataRequest->count();

	int startIndex = lowerBound(startTime);

	if(startIndex == -1)
		clientStartTimePlusCountDataRequest->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
	else
	{
		count = startIndex + count;
		int countAsInt = count;
		clientStartTimePlusCountDataRequest->clearData();
		clientStartTimePlusCountDataRequest->setBufferGroupInfo(bufferGroupInfo_);
		for (int iCurrent = startIndex, limit = dataHolders_.count(); iCurrent < countAsInt && iCurrent < limit; iCurrent++)
		{
			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
			AMDSFlatArray oneFlatArray;
			dataHolder->data(&oneFlatArray);
			clientStartTimePlusCountDataRequest->appendData(dataHolder);
//			request->histogramData()->append(dataHolders_[iCurrent]);
		}
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
