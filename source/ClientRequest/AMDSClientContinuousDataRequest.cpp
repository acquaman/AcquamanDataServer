#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	requestType_ = AMDSClientRequestDefinitions::Continuous;

	startTime_ = QDateTime();
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimePlusCount, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
{
	startTime_ = startTime;
}

AMDSClientContinuousDataRequest::~AMDSClientContinuousDataRequest()
{
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(const AMDSClientContinuousDataRequest &other) :
	AMDSClientDataRequest(other)
{
	(*this) = other;
}

AMDSClientContinuousDataRequest& AMDSClientContinuousDataRequest::operator =(const AMDSClientContinuousDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		startTime_ = other.startTime();
	}
	return (*this);
}

bool AMDSClientContinuousDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << startTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientContinuousDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	QDateTime readStartTime;
	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setStartTime(readStartTime);

	return true;
}
