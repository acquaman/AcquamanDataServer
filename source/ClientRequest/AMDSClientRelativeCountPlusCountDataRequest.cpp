#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::RelativeCountPlusCount);

	setRelativeCount(0);
	setCount(1);
}

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const quint64 relativeCount, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent):
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::RelativeCountPlusCount, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
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

int AMDSClientRelativeCountPlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientDataRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << relativeCount();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RELATIVE_COUNT;
	*dataStream << count();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientRelativeCountPlusCountDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	quint64 readRelativeCount;
	quint64 readCount;

	*dataStream >> readRelativeCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RELATIVE_COUNT;
	*dataStream >> readCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT;

	setRelativeCount(readRelativeCount);
	setCount(readCount);

	return AMDS_CLIENTREQUEST_SUCCESS;
}
