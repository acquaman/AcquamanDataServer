#include "AMDSClientRequest.h"

#include "AMDSDataStream.h"

#include <QDebug>

AMDSClientRequest::AMDSClientRequest(QObject *parent) :
	QObject(parent)
{
	socketKey_ = QString();
	errorMessage_ = QString();
	requestType_ = AMDSClientRequestDefinitions::InvalidRequest;
	responseType_ = AMDSClientRequest::InvalidResponse;
}

AMDSClientRequest::AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent) :
	QObject(parent)
{
	socketKey_ = socketKey;
	errorMessage_ = errorMessage;
	requestType_ = requestType;
	responseType_ = responseType;
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
		socketKey_ = other.socketKey();
		errorMessage_ = other.errorMessage();
		requestType_ = other.requestType();
		responseType_ = other.responseType();
	}
	return (*this);
}

bool AMDSClientRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientRequest::writeToDataStream";
//	*dataStream << (quint8)requestType_;
//	if(dataStream->status() != QDataStream::Ok)
//		return false;
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
	qDebug() << "Calling AMDSClientRequest::readFromDataStream";
//	quint8 readRequestType;
	QString readSocketKey;
	QString readErrorMessage;
	quint8 readResponseType;

//	dataStream >> readRequestType;
//	if(dataStream->status() != QDataStream::Ok)
//		return false;
	*dataStream >> readSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readErrorMessage;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readResponseType;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setSocketKey(readSocketKey);
	setErrorMessage(readErrorMessage);
	setResponseType((AMDSClientRequest::ResponseType)readResponseType);

	return true;
}
