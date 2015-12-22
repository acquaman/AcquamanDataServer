#include "AMDSBufferGroup.h"

#include "ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"

#include "DataHolder/AMDSSpectralDataHolder.h"

#include "util/AMDSRunTimeSupport.h"

AMDSBufferGroup::AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, QObject *parent) :
	QObject(parent), dataHolders_(maxSize)
{
	bufferGroupInfo_ = bufferGroupInfo;

	initializeDwellControls();
}

AMDSBufferGroup::AMDSBufferGroup(const AMDSBufferGroup& other):
	QObject(other.parent()), dataHolders_(other.dataHolders_.maxSize())
{
	bufferGroupInfo_ = other.bufferGroupInfo();

	initializeDwellControls(other.dwellActive_, other.dwellDataCount_, other.dwellStartTime_, other.dwellCumulativeDataHolder_);
}

AMDSBufferGroup::~AMDSBufferGroup()
{
	clear();
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

	if (dwellCumulativeDataHolder_) {
		dwellCumulativeDataHolder_->deleteLater();
		dwellCumulativeDataHolder_ = 0;
	}
}

void AMDSBufferGroup::append(const AMDSDataHolderList &dataHolderList)
{
	//!!! IMPORTANT: don't put the write lock here ... ...
	foreach (AMDSDataHolder *dataHolder, dataHolderList) {
		append(dataHolder);
	}
}

void AMDSBufferGroup::append(AMDSDataHolder *newData)
{
	QWriteLocker writeLock(&lock_);

	AMDSDataHolder* dataHolderRemoved = dataHolders_.append(newData);
	if(dataHolderRemoved)
		dataHolderRemoved->deleteLater();

	if (dwellActive_) {
		dwellDataCount_ ++;

		if (dwellCumulativeDataHolder_) {
			AMDSDataHolder *tempDataHolder = dwellCumulativeDataHolder_;

			dwellCumulativeDataHolder_ = (*dwellCumulativeDataHolder_) + (*newData);
			tempDataHolder->deleteLater();
		} else {
			dwellCumulativeDataHolder_ = AMDSDataHolderSupport::instantiateDataHolderFromInstance(newData);
			dwellCumulativeDataHolder_->cloneData(newData);
		}

		emit continuousDwellDataUpdate(dwellCumulativeDataHolder_, dwellDataCount_, dwellStartTime_.elapsed());
	}
}

void AMDSBufferGroup::onDwellStarted()
{
	if (dwellActive_) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_BUFFER_GROUP_ALREADY_DWELL_STARTED, "Received dwellStarted signal when already in dwell active mode.");
	} else {
		initializeDwellControls();

		dwellActive_ = true;
	}
}

void AMDSBufferGroup::onDwellStopped()
{
	if (dwellActive_) {
		dwellActive_ = false;

		AMDSDataHolder *dwellDataHolder;
		if (bufferGroupInfo_.flattenMethod() == AMDSBufferGroupInfo::Average) {
			dwellDataHolder = (*dwellCumulativeDataHolder_) / dwellDataCount_;
		} else {
			dwellDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromInstance(dwellCumulativeDataHolder_);
			dwellDataHolder->cloneData(dwellCumulativeDataHolder_);
		}

		emit finalDwellDataUpdate(dwellDataHolder, dwellDataCount_, dwellStartTime_.elapsed());
	} else {
//		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_BUFFER_GROUP_ALREADY_DWELL_STOPPED, "Received dwellStop signal when already in dwell stop mode.");
	}
}

void AMDSBufferGroup::processClientRequest(AMDSClientRequest *clientRequest, bool internalRequest){
	QReadLocker readLock(&lock_);

	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest *>(clientRequest);
	if (clientDataRequest)
		clientDataRequest->setBufferGroupInfo(bufferGroupInfo());

	switch(clientRequest->requestType()){
	case AMDSClientRequestDefinitions::Introspection:
		break;
	case AMDSClientRequestDefinitions::Statistics:
		break;
	case AMDSClientRequestDefinitions::Configuration:
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

	// Handle the trigger/dwell flattening request differently than usual requests
	if(internalRequest)
		emit internalRequestProcessed(clientRequest);
	else
		emit clientRequestProcessed(clientRequest);
}

bool AMDSBufferGroup::flattenData(QList<AMDSDataHolder *> *dataArray)
{
	if (!bufferGroupInfo_.isFlattenEnabled() || bufferGroupInfo_.flattenMethod() == AMDSBufferGroupInfo::NoFlatten) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_BUFFER_GROUP_DISABLE_FLATTEN, QString("The given buffergroup (%1) doesn't enable flatten feature or the flatten method is %2.").arg(bufferGroupInfo_.name()).arg(bufferGroupInfo_.flattenMethod()));
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

void AMDSBufferGroup::initializeDwellControls(bool activateDwell, int dataCount, const QTime &startTime, AMDSDataHolder *dataHolder)
{
	dwellActive_ = activateDwell;
	dwellDataCount_ = dataCount;

	dwellStartTime_.restart();
	if (startTime.isValid() && !startTime.isNull()) {
		dwellStartTime_.addMSecs(startTime.elapsed());
	}

	if (dwellCumulativeDataHolder_) {
		dwellCumulativeDataHolder_->deleteLater();
		dwellCumulativeDataHolder_ = 0;
	}

	if (dataHolder) {
		dwellCumulativeDataHolder_ = AMDSDataHolderSupport::instantiateDataHolderFromInstance(dataHolder);
		dwellCumulativeDataHolder_->cloneData(dataHolder);
	}
}
