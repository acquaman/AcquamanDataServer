#include "ClientRequest/AMDSClientRequest.h"

#include "appController/AMDSAppController.h"
#include "ClientRequest/AMDSClientRequestSupport.h"
#include "util/AMDSRunTimeSupport.h"

AMDSClientRequest* AMDSClientRequest::decodeAndInstantiateClientRequest(QDataStream *dataStream)
{
	AMDSClientRequest* clientRequest = 0;
	int parseResult = AMDS_CLIENTREQUEST_SUCCESS;

	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::InvalidRequest;

	quint8 clientRequestTypeAsInt;
	*dataStream >>(clientRequestTypeAsInt);
	if(dataStream->status() != QDataStream::Ok) {
		parseResult = AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MESSAGE_TYPE;
	}

	if (parseResult == AMDS_CLIENTREQUEST_SUCCESS) {
		clientRequestType = (AMDSClientRequestDefinitions::RequestType)clientRequestTypeAsInt;
		clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
		if (!clientRequest) {
			parseResult = AMDS_CLIENTREQUEST_INVALID_MESSAGE_TYPE;
		} else {
			parseResult = clientRequest->readFromDataStream(dataStream);
		}
	}

	if (parseResult != AMDS_CLIENTREQUEST_SUCCESS) {
		clientRequest = 0;

		QString errMessage = AMDSClientRequestDefinitions::errorMessage(parseResult, AMDSClientRequestDefinitions::Read, clientRequestType);
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, 0, parseResult, QString("AMDSClientRequest::Failed to parse clientRequest. Error: %1").arg(errMessage));
	}

	return clientRequest;
}

bool AMDSClientRequest::encodeAndwriteClientRequest(QDataStream *dataStream, AMDSClientRequest *clientRequest)
{
	int result = clientRequest->writeToDataStream(dataStream);
	if ( result != AMDS_CLIENTREQUEST_SUCCESS ) {
		QString errorMsg = AMDSClientRequestDefinitions::errorMessage(result, AMDSClientRequestDefinitions::Write, clientRequest->requestType());
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, 0, result, errorMsg);

		return false;
	}

	return true;
}

AMDSClientRequest::AMDSClientRequest(QObject *parent) :
	QObject(parent)
{
	setBaseAttributesValues(QString(), QString(), AMDSClientRequestDefinitions::InvalidRequest, AMDSClientRequest::InvalidResponse, QDateTime::currentDateTimeUtc());
}

AMDSClientRequest::AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QDateTime &localTime, QObject *parent) :
	QObject(parent)
{
	setBaseAttributesValues(socketKey, errorMessage, requestType, responseType, localTime);
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
		setBaseAttributesValues(other.socketKey(), other.errorMessage(), other.requestType(), other.responseType(), other.clientLocalTime());
	}

	return (*this);
}

bool AMDSClientRequest::isDataClientRequest() {
	return     requestType() == AMDSClientRequestDefinitions::StartTimePlusCount
			|| requestType() == AMDSClientRequestDefinitions::RelativeCountPlusCount
			|| requestType() == AMDSClientRequestDefinitions::StartTimeToEndTime
			|| requestType() == AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter
			|| requestType() == AMDSClientRequestDefinitions::Continuous ;
}

void AMDSClientRequest::printData()
{
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_CLIENTREQUEST_INFO_REQUEST_DATA, toString());
}

int AMDSClientRequest::calculateTimeDelta() const
{
	QDateTime serverLocalTime = QDateTime::currentDateTimeUtc();

	int timeDelta = serverLocalTime.toMSecsSinceEpoch() - clientLocalTime().toMSecsSinceEpoch();
	return timeDelta;
}

int AMDSClientRequest::writeToDataStream(QDataStream *dataStream)
{
	*dataStream <<((quint8)requestType());
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_MESSAGE_TYPE;

	*dataStream << socketKey_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_SOCKET_KEY;

	*dataStream << errorMessage_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE;

	*dataStream << (quint8)responseType_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE;
	*dataStream << clientLocalTime_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CLIENT_LOCAL_TIME;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientRequest::readFromDataStream(QDataStream *dataStream)
{

	QString readSocketKey;
	QString readErrorMessage;
	quint8 readResponseType;
	QDateTime localTime;

	*dataStream >> readSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_STATUS;

	*dataStream >> readErrorMessage;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_ERROR_MESSAGE;

	*dataStream >> readResponseType;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_RESPONSE_TYPE;
	*dataStream >> localTime;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CLIENT_LOCAL_TIME;

	//NOTE: we won't change the requestType :)
	setBaseAttributesValues(readSocketKey, readErrorMessage, requestType(), (AMDSClientRequest::ResponseType)readResponseType, localTime);

	return AMDS_CLIENTREQUEST_SUCCESS;
}

void AMDSClientRequest::setBaseAttributesValues(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QDateTime &localTime)
{
	setSocketKey(socketKey);
	setErrorMessage(errorMessage);
	setRequestType(requestType);
	setResponseType(responseType);
	setClientLocalTime(localTime);

	AMDSAppController *appController = AMDSAppController::appController();
	if (appController && appController->appType() == AMDSAppController::Server)
		timeDelta_ = calculateTimeDelta();
	else
		timeDelta_ = 0;
}
