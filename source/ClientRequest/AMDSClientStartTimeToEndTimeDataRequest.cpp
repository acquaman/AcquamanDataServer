#include "AMDSClientStartTimeToEndTimeDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientStartTimeToEndTimeDataRequest::AMDSClientStartTimeToEndTimeDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::StartTimeToEndTime);

	setStartTime(QDateTime());
	setEndTime(QDateTime());
}

AMDSClientStartTimeToEndTimeDataRequest::AMDSClientStartTimeToEndTimeDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool enableFlattening, const QDateTime &startTime, const QDateTime &endTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimeToEndTime, responseType, bufferName, includeStatusData, enableFlattening, bufferGroupInfo, parent)
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

bool AMDSClientStartTimeToEndTimeDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << startTime();
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << endTime();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientStartTimeToEndTimeDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	QDateTime readStartTime;
	QDateTime readEndTime;

	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readEndTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setStartTime(readStartTime);
	setEndTime(readEndTime);

	return true;
}
