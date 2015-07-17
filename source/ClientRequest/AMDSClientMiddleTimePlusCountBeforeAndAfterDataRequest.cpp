#include "AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter);

	setMiddleTime(QDateTime());
	setCountBefore(0);
	setCountAfter(1);
}

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, quint64 countBefore, quint64 countAfter, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::StartTimeToEndTime, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
{
	setMiddleTime(startTime);
	setCountBefore(countBefore);
	setCountAfter(countAfter);
}

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::~AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest()
{
}

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other) :
	AMDSClientDataRequest(0)
{
	(*this) = other;
}

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest& AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::operator =(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		setMiddleTime(other.middleTime());
		setCountBefore(other.countBefore());
		setCountAfter(other.countAfter());
	}
	return (*this);
}

bool AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << middleTime();
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << countBefore();
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << countAfter();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	QDateTime readMiddleTime;
	quint64 countBefore;
	quint64 countAfter;

	*dataStream >> readMiddleTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> countBefore;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> countAfter;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setMiddleTime(readMiddleTime);
	setCountBefore(countBefore);
	setCountAfter(countAfter);

	return true;
}
