#include "AMDSClientStartTimeToEndTimeDataRequest.h"

AMDSClientStartTimeToEndTimeDataRequest::AMDSClientStartTimeToEndTimeDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::StartTimeToEndTime);

	setStartTime(QDateTime());
	setEndTime(QDateTime());
}

AMDSClientStartTimeToEndTimeDataRequest::AMDSClientStartTimeToEndTimeDataRequest(ResponseType responseType, const QDateTime &localDateTime, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, const QDateTime &endTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimeToEndTime, responseType, localDateTime, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
{
	setStartTime(startTime);
	setEndTime(endTime);
}

AMDSClientStartTimeToEndTimeDataRequest::~AMDSClientStartTimeToEndTimeDataRequest()
{
}

AMDSClientStartTimeToEndTimeDataRequest::AMDSClientStartTimeToEndTimeDataRequest(const AMDSClientStartTimeToEndTimeDataRequest &other) :
	AMDSClientDataRequest(0)
{
	(*this) = other;
}

AMDSClientStartTimeToEndTimeDataRequest& AMDSClientStartTimeToEndTimeDataRequest::operator =(const AMDSClientStartTimeToEndTimeDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		setStartTime(other.startTime());
		setEndTime(other.endTime());
	}
	return (*this);
}

void AMDSClientStartTimeToEndTimeDataRequest::setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, const QDateTime &endTime)
{
	setBufferName(bufferName);
	setIncludeStatusData(includeStatusData);
	setFlattenResultData(flattenResultData);
	setStartTime(startTime);
	setEndTime(endTime);
}

int AMDSClientStartTimeToEndTimeDataRequest::writeToDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << startTime();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME;
	*dataStream << endTime();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_END_TIME;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientStartTimeToEndTimeDataRequest::readFromDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QDateTime readStartTime;
	QDateTime readEndTime;

	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME;
	*dataStream >> readEndTime;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_END_TIME;

	setStartTime(readStartTime);
	setEndTime(readEndTime);

	return AMDS_CLIENTREQUEST_SUCCESS;
}
