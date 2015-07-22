#include "source/ClientRequest/AMDSClientRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientRequest::AMDSClientRequest(QObject *parent) :
	QObject(parent)
{
	setProperties(QString(), QString(), AMDSClientRequestDefinitions::InvalidRequest, AMDSClientRequest::InvalidResponse);
}

AMDSClientRequest::AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent) :
	QObject(parent)
{
	setProperties(socketKey, errorMessage, requestType, responseType);
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
		setProperties(other.socketKey(), other.errorMessage(), other.requestType(), other.responseType());
	}
	return (*this);
}

bool AMDSClientRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	*dataStream << socketKey_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << errorMessage_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << (quint8)responseType_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	QString readSocketKey;
	QString readErrorMessage;
	quint8 readResponseType;

	*dataStream >> readSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readErrorMessage;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readResponseType;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	//NOTE: we won't change the requestType :)
	setProperties(readSocketKey, readErrorMessage, requestType(), (AMDSClientRequest::ResponseType)readResponseType);

	return true;
}

void AMDSClientRequest::setProperties(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType)
{
	setSocketKey(socketKey);
	setErrorMessage(errorMessage);
	setRequestType(requestType);
	setResponseType(responseType);
}
