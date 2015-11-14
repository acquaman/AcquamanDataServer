#include "AMDSBufferGroup.h"

#include "ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"

#include "DataHolder/AMDSSpectralDataHolder.h"

#include "util/AMErrorMonitor.h"
#include "util/AMDSRunTimeSupport.h"

AMDSBufferGroup::AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, bool enableCumulative, QObject *parent) :
	QObject(parent), dataHolders_(maxSize)
{
	bufferGroupInfo_ = bufferGroupInfo;

	enableCumulative_ = enableCumulative;
	cumulativeDataHolder_ = 0;
}

AMDSBufferGroup::AMDSBufferGroup(const AMDSBufferGroup& other):
	QObject(other.parent()), dataHolders_(other.dataHolders_.maxSize())
{
	bufferGroupInfo_ = other.bufferGroupInfo();

	enableCumulative_ = other.enableCumulative_;
	cumulativeDataHolder_ = other.cumulativeDataHolder_;
}

AMDSBufferGroup::~AMDSBufferGroup()
{
	clear();
}


AMDSDataHolder * AMDSBufferGroup::cumulativeDataHolder() const
{
	QReadLocker readLock(&lock_);
	return cumulativeDataHolder_;
}

int AMDSBufferGroup::count() const
{
	QReadLocker readLock(&lock_);
	return dataHolders_.count();
}

void AMDSBufferGroup::clear()
{
	QWriteLocker writeLock(&lock_);

	for(int iElement = 0, elementCount = dataHolders_.count(); iElement < elementCount; iElement++) {
		dataHolders_[iElement]->deleteLater();
	}
	dataHolders_.clear();

	if (enableCumulative_ && cumulativeDataHolder_) {
		cumulativeDataHolder_->deleteLater();
		cumulativeDataHolder_ = 0;
	}
}

void AMDSBufferGroup::append(AMDSDataHolder *newData, bool elapsedDwellTime)
{
	QWriteLocker writeLock(&lock_);

	AMDSDataHolder* dataHolderRemoved = dataHolders_.append(newData);
	if(dataHolderRemoved)
		dataHolderRemoved->deleteLater();

	if (enableCumulative_) {
		if (cumulativeDataHolder_) {
			AMDSDataHolder *tempDataHolder = cumulativeDataHolder_;

			cumulativeDataHolder_ = (*cumulativeDataHolder_) + (*newData);
			tempDataHolder->deleteLater();
		} else {
			cumulativeDataHolder_ = AMDSDataHolderSupport::instantiateDataHolderFromClassName(newData->metaObject()->className());
			cumulativeDataHolder_->cloneData(newData);
		}

		AMDSDwellSpectralDataHolder *specturalCumulativeDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(cumulativeDataHolder_);
		if (specturalCumulativeDataHolder) {
			AMDSDwellStatusData cumulativeStatusData = specturalCumulativeDataHolder->dwellStatusData();

//			emit continuousDataUpdate(specturalCumulativeDataHolder);
//			emit continuousStatusDataUpdate(cumulativeStatusData, count());
//			emit continuousAllDataUpdate(specturalCumulativeDataHolder, cumulativeStatusData, count(), elapsedDwellTime);
		} else {
			if(AMDSRunTimeSupport::debugAtLevel(1))
				AMErrorMon::alert(this, AMDS_ALERT_DATA_HOLDER_TYPE_NOT_SUPPORT, QString("The cumulative dataHolder type (%1) is NOT supported at this moment.").arg(cumulativeDataHolder()->metaObject()->className()));
		}
	}
}

void AMDSBufferGroup::finishDwellDataUpdate(double elapsedTime)
{
	if (cumulativeEnabled()) {
		AMDSDwellSpectralDataHolder *specturalCumulativeDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(cumulativeDataHolder());
		if (specturalCumulativeDataHolder) {
			AMDSDwellStatusData cumulativeStatusData = specturalCumulativeDataHolder->dwellStatusData();

			emit dwellFinishedTimeUpdate(elapsedTime);
			emit dwellFinishedDataUpdate(specturalCumulativeDataHolder);
			emit dwellFinishedStatusDataUpdate(cumulativeStatusData, count());
			emit dwellFinishedAllDataUpdate(specturalCumulativeDataHolder, cumulativeStatusData, count(), elapsedTime);
		} else {
			if(AMDSRunTimeSupport::debugAtLevel(1))
				AMErrorMon::alert(this, AMDS_ALERT_DATA_HOLDER_TYPE_NOT_SUPPORT, QString("The cumulative dataHolder type (%1) is NOT supported at this moment.").arg(cumulativeDataHolder()->metaObject()->className()));
		}
	}
}

void AMDSBufferGroup::processClientRequest(AMDSClientRequest *clientRequest){
	QReadLocker readLock(&lock_);

	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest *>(clientRequest);
	if (clientDataRequest)
		clientDataRequest->setBufferGroupInfo(bufferGroupInfo());

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

bool AMDSBufferGroup::flattenData(QList<AMDSDataHolder *> *dataArray)
{
	if (!bufferGroupInfo_.isFlattenEnabled() || bufferGroupInfo_.flattenMethod() == AMDSBufferGroupInfo::NoFlatten) {
		if(AMDSRunTimeSupport::debugAtLevel(1))
			AMErrorMon::alert(this, AMDS_SERVER_ALT_BUFFER_GROUP_DISABLE_FLATTEN, QString("The given buffergroup (%1) doesn't enable flatten feature or the flatten method is %2.").arg(bufferGroupInfo_.name()).arg(bufferGroupInfo_.flattenMethod()));
		return false;
	}

	// make the summarization operation
	int totalDataSize = dataArray->size();
	if (totalDataSize > 0) {
		// make a copy of the first data holder
		AMDSDataHolder *flattenDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromInstance( dataArray->at(0) );
		(*flattenDataHolder) = (*dataArray->at(0));
		for (int i = 1; i < totalDataSize; i++) {
			AMDSDataHolder * dataHolder = dataArray->at(i);
			if (flattenDataHolder && dataHolder) {
				AMDSDataHolder *tempDataHolder = flattenDataHolder;
				flattenDataHolder = (*flattenDataHolder) + (*dataHolder);
				tempDataHolder->deleteLater();
			}
		}

		if (flattenDataHolder) {
			switch (bufferGroupInfo_.flattenMethod()) {
			case AMDSBufferGroupInfo::Summary:
				break; // do nothing, since we already summrized the data
			case AMDSBufferGroupInfo::Average: {
				AMDSDataHolder *tempDataHolder = flattenDataHolder;
				flattenDataHolder = (*flattenDataHolder) / totalDataSize;
				tempDataHolder->deleteLater();
				break;
			}
			default:
				break;
			}
		}

		dataArray->clear();
		dataArray->append(flattenDataHolder);
	}

	return true;
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

	// when doing flattening, the data holders in the targetDataArray are new instances, which need to be released after usage
	// if not, the array hosts original data holder, which should NOT be released
	bool releaseDataHolders = false;
	if (clientDataRequest->flattenResultData()) {
		releaseDataHolders = flattenData(&targetDataArray);
	}

	foreach (AMDSDataHolder * dataHolder, targetDataArray) {
		clientDataRequest->copyAndAppendData(dataHolder);
	}

	if (releaseDataHolders) {
		foreach (AMDSDataHolder *dataHolder, targetDataArray) {
			dataHolder->deleteLater();
		}
		targetDataArray.clear();
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
		populateData(clientDataRequest, startIndex, dataHolders_.count());

		startIndex++;
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
