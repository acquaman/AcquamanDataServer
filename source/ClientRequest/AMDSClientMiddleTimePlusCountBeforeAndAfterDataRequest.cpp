#include "AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"

#include "source/Connection/AMDSDataStream.h"

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter);

	setMiddleTime(QDateTime());
	setCountBefore(0);
	setCountAfter(1);
}

AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, quint64 countBefore, quint64 countAfter, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter, responseType, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
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

void AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::setAttributesValues(QString &bufferName, bool includeStatusData, bool flattenResultData, QDateTime &middleTime, quint64 countBefore, quint64 countAfter)
{
	setBufferName(bufferName);
	setIncludeStatusData(includeStatusData);
	setFlattenResultData(flattenResultData);
	setMiddleTime(middleTime);
	setCountBefore(countBefore);
	setCountAfter(countAfter);
}

int AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientDataRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << middleTime();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MIDDLE_TIME;
	*dataStream << countBefore();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_BEFORE;
	*dataStream << countAfter();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_AFTER;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QDateTime readMiddleTime;
	quint64 countBefore;
	quint64 countAfter;

	*dataStream >> readMiddleTime;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MIDDLE_TIME;
	*dataStream >> countBefore;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_BEFORE;
	*dataStream >> countAfter;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_COUNT_AFTER;

	setMiddleTime(readMiddleTime);
	setCountBefore(countBefore);
	setCountAfter(countAfter);

	return AMDS_CLIENTREQUEST_SUCCESS;
}
