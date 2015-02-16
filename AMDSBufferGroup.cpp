#include "AMDSBufferGroup.h"

AMDSBufferGroup::AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, QObject *parent) :
	QObject(parent), dataHolders_(maxSize)
{
	bufferGroupInfo_ = bufferGroupInfo;
}

AMDSBufferGroup::AMDSBufferGroup(const AMDSBufferGroup& other):
	QObject(other.parent()), dataHolders_(other.dataHolders_)
{
}

void AMDSBufferGroup::requestData(AMDSClientDataRequest *request)
{
	QReadLocker readLock(&lock_);
	switch(request->requestType()){
	case AMDSClientDataRequest::Introspection:
		//something here
		break;
	case AMDSClientDataRequest::Continuous:
		populateData(request, request->time1());
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		populateData(request, request->time1(), request->count1());
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		populateData(request, request->count1(), request->count2());
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		populateData(request, request->time1(), request->time2());
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		populateData(request, request->time1(), request->count1(), request->count2());
		break;
	}
	emit dataRequestReady(request);
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest *request, const QDateTime &lastFetch)
{
	int startIndex = lowerBound(lastFetch);

	if(startIndex == -1)
		request->setErrorMessage(QString("Could not locate data for time %1").arg(lastFetch.toString()));
	else
	{
		// Since the last fetch actually included the data at the given time, we need to increment the index
		// by one, to start from the one following:
		startIndex++;
		for(int iCurrent = startIndex, size = dataHolders_.count(); iCurrent < size; iCurrent++)
		{
			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//			request->histogramData()->append(dataHolder);
		}
	}
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest* request, const QDateTime& startTime, int count)
{
	int startIndex = lowerBound(startTime);

	if(startIndex == -1)
		request->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
	else
	{
		count = startIndex + count;
		for (int iCurrent = startIndex, limit = dataHolders_.count(); iCurrent < count && iCurrent < limit; iCurrent++)
		{
			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//			request->histogramData()->append(dataHolders_[iCurrent]);
		}
	}
}
void AMDSBufferGroup::populateData(AMDSClientDataRequest* request, int relativeCount, int count)
{
	int startIndex = dataHolders_.count() - 1 - relativeCount - count;
	int endIndex = dataHolders_.count() - 1 - relativeCount + count;
	if(startIndex < 0)
		startIndex = 0;

	if(endIndex >= dataHolders_.count())
		endIndex = dataHolders_.count() - 1;

	for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
	{
		AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//		request->histogramData()->append(dataHolders_[iCurrent]);
	}
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest* request, const QDateTime& startTime, const QDateTime& endTime)
{
	int startIndex = lowerBound(startTime);
	int endIndex = lowerBound(endTime);

	if(startIndex == -1)
		request->setErrorMessage(QString("Could not locate data for time %1").arg(startTime.toString()));
	else if(endIndex == -1)
		request->setErrorMessage(QString("Could not locate data for time %1").arg(endTime.toString()));
	else
	{
		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
		{
			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//			request->histogramData()->append(dataHolders_[iCurrent]);
		}
	}
}

void AMDSBufferGroup::populateData(AMDSClientDataRequest* request, const QDateTime& middleTime, int countBefore, int countAfter)
{
	int middleIndex = lowerBound(middleTime);
	if(middleIndex == -1)
		request->setErrorMessage(QString("Could not locate data for time %1").arg(middleTime.toString()));
	else
	{
		int startIndex = middleIndex - countBefore;
		if(startIndex < 0)
			startIndex = 0;

		int endIndex = middleIndex + countAfter;
		if(endIndex >= dataHolders_.count())
			endIndex = dataHolders_.count() - 1;

		for (int iCurrent = startIndex; iCurrent < endIndex; iCurrent++)
		{
			AMDSDataHolder* dataHolder = dataHolders_[iCurrent];
//			request->histogramData()->append(dataHolders_[iCurrent]);
		}
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
