#include "AMDSClientStartTimePlusCountDataRequest.h"

#include "AMDSDataStream.h"

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	requestType_ = AMDSClientRequestDefinitions::StartTimePlusCount;

	startTime_ = QDateTime();
	count_ = 0;
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimePlusCount, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
{
	startTime_ = startTime;
	count_ = count;
}

AMDSClientStartTimePlusCountDataRequest::~AMDSClientStartTimePlusCountDataRequest()
{
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(const AMDSClientStartTimePlusCountDataRequest &other) :
	AMDSClientDataRequest(0)
{
	(*this) = other;
}

AMDSClientStartTimePlusCountDataRequest& AMDSClientStartTimePlusCountDataRequest::operator =(const AMDSClientStartTimePlusCountDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		startTime_ = other.startTime();
		count_ = other.count();
	}
	return (*this);
}

bool AMDSClientStartTimePlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << startTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << count_;
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
