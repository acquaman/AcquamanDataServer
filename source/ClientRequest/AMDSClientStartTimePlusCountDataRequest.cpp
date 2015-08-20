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

bool AMDSClientStartTimePlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << startTime();
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << count();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientStartTimePlusCountDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	QDateTime readStartTime;
	quint64 readCount;

	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setStartTime(readStartTime);
	setCount(readCount);

	return true;
}
