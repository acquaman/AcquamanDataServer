#include "AMDSClientConfigurationRequest.h"

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(QObject *parent)
	: AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Configuration);

	setBufferName("Invalid");
}

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(const QString &bufferName, const QString &socketKey, const QString &errorMessage, AMDSClientRequest::ResponseType responseType, QObject *parent)
	: AMDSClientRequest(socketKey, errorMessage, AMDSClientRequestDefinitions::Configuration, responseType, parent)
{
	setBufferName(bufferName);
}

AMDSClientConfigurationRequest::~AMDSClientConfigurationRequest()
{

}

void AMDSClientConfigurationRequest::appendCommand(int commandId, const QVariant &value)
{
	configurationCommands_.insert(commandId, value);
}

int AMDSClientConfigurationRequest::writeToDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	*dataStream << configurationCommands_.size();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	for (int index = 0; index < configurationCommands_.size(); index++) {
		int commandId = configurationCommands_.key(index);
		*dataStream << commandId;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		*dataStream << configurationCommands_.value(commandId);
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientConfigurationRequest::readFromDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QString readBufferName;
	int configurationCommandSize;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	*dataStream >> configurationCommandSize;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	configurationCommands_.clear();
	int commandId;
	QVariant commandValue;
	for (int index = 0; index < configurationCommandSize; index++) {
		*dataStream >> commandId;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		*dataStream >> commandValue;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		appendCommand(commandId, commandValue);
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

