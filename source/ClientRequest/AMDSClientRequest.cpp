#include "source/ClientRequest/AMDSClientRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientRequest::AMDSClientRequest(QObject *parent) :
	QObject(parent)
{
	setAttributesValues(QString(), QString(), AMDSClientRequestDefinitions::InvalidRequest, AMDSClientRequest::InvalidResponse);
}

AMDSClientRequest::AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent) :
	QObject(parent)
{
	setAttributesValues(socketKey, errorMessage, requestType, responseType);
}

AMDSClientRequest::~AMDSClientRequest()
{
}

AMDSClientRequest::AMDSClientRequest(const AMDSClientRequest &other) :
	QObject(0)
{
	(*this) = other;
}

AMDSClientRequest& AMDSClientRequest::operator =(const AMDSClientRequest &other)
{
	if(this != &other){
		setAttributesValues(other.socketKey(), other.errorMessage(), other.requestType(), other.responseType());
	}
	return (*this);
}

int AMDSClientRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	*dataStream << socketKey_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_SOCKET_KEY;
	*dataStream << errorMessage_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE;
	*dataStream << (quint8)responseType_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	QString readSocketKey;
	QString readErrorMessage;
	quint8 readResponseType;

	*dataStream >> readSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_STATUS;
	*dataStream >> readErrorMessage;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE;
	*dataStream >> readResponseType;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE;

	//NOTE: we won't change the requestType :)
	setAttributesValues(readSocketKey, readErrorMessage, requestType(), (AMDSClientRequest::ResponseType)readResponseType);

	return AMDS_CLIENTREQUEST_SUCCESS;
}

void AMDSClientRequest::setAttributesValues(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType)
{
	setSocketKey(socketKey);
	setErrorMessage(errorMessage);
	setRequestType(requestType);
	setResponseType(responseType);
}
