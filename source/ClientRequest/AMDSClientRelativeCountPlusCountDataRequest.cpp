#include "AMDSClientRelativeCountPlusCountDataRequest.h"


AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::RelativeCountPlusCount);

	setRelativeCount(0);
	setCount(1);
}

AMDSClientRelativeCountPlusCountDataRequest::AMDSClientRelativeCountPlusCountDataRequest(ResponseType responseType, const QDateTime &localDateTime, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const quint64 relativeCount, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent):
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::RelativeCountPlusCount, responseType, localDateTime, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
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

void AMDSClientRelativeCountPlusCountDataRequest::setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, quint64 relativeCount, quint64 count) {
	setBufferName(bufferName);
	setIncludeStatusData(includeStatusData);
	setFlattenResultData(flattenResultData);
	setRelativeCount(relativeCount);
	setCount(count);
}


int AMDSClientRelativeCountPlusCountDataRequest::writeToDataStream(QDataStream *dataStream)
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

int AMDSClientRelativeCountPlusCountDataRequest::readFromDataStream(QDataStream *dataStream)
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
