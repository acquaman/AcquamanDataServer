#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::RelativeCountPlusCount);

	setRelativeCount(0);
	setCount(1);
}

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const quint64 relativeCount, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent):
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::RelativeCountPlusCount, responseType, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
{
	setRelativeCount(relativeCount);
	setCount(count);
}

AMDSClientRelativeCountPlusCountDataRequest::~AMDSClientRelativeCountPlusCountDataRequest()
{

}

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(const AMDSClientRelativeCountPlusCountDataRequest &other):
	AMDSClientDataRequest(other)
{
	(*this) = other;
}

AMDSClientRelativeCountPlusCountDataRequest& AMDSClientRelativeCountPlusCountDataRequest::operator =(const AMDSClientRelativeCountPlusCountDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		setRelativeCount(other.relativeCount());
		setCount(other.count());
	}
	return (*this);
}

bool AMDSClientRelativeCountPlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << relativeCount();
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << count();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientRelativeCountPlusCountDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	quint64 readRelativeCount;
	quint64 readCount;

	*dataStream >> readRelativeCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setRelativeCount(readRelativeCount);
	setCount(readCount);

	return true;
}
