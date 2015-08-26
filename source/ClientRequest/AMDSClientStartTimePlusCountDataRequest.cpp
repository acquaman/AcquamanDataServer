#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::StartTimePlusCount);

	setStartTime(QDateTime());
	setCount(1);
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimePlusCount, responseType, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
{
	setStartTime(startTime);
	setCount(count);
}

AMDSClientStartTimePlusCountDataRequest::~AMDSClientStartTimePlusCountDataRequest()
{
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(const AMDSClientStartTimePlusCountDataRequest &other) :
	AMDSClientDataRequest(other)
{
	(*this) = other;
}

AMDSClientStartTimePlusCountDataRequest& AMDSClientStartTimePlusCountDataRequest::operator =(const AMDSClientStartTimePlusCountDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		setStartTime(other.startTime());
		setCount(other.count());
	}
	return (*this);
}

int AMDSClientStartTimePlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientDataRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << startTime();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME;
	*dataStream << count();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientStartTimePlusCountDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QDateTime readStartTime;
	quint64 readCount;

	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_START_TIME;
	*dataStream >> readCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT;

	setStartTime(readStartTime);
	setCount(readCount);

	return AMDS_CLIENTREQUEST_SUCCESS;
}
