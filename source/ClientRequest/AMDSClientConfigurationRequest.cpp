#include "AMDSClientConfigurationRequest.h"

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(QObject *parent)
	: AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Configuration);

	setBufferName("Invalid");
}

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(const QString &bufferName, const QString &socketKey, const QString &errorMessage, AMDSClientRequest::ResponseType responseType, const QDateTime &localTime, QObject *parent)
	: AMDSClientRequest(socketKey, errorMessage, AMDSClientRequestDefinitions::Configuration, responseType, localTime, parent)
{
	setBufferName(bufferName);
}

AMDSClientConfigurationRequest::~AMDSClientConfigurationRequest()
{
	configurationCommandDefs_.clear();
	configurationCommands_.clear();
}

void AMDSClientConfigurationRequest::appendCommand(const QString &command, const QVariant &value)
{
	configurationCommands_.insert(command, value);
}

int AMDSClientConfigurationRequest::writeToDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	// write the AMDSCommand definitions if there are any
	*dataStream << configurationCommandDefs_.size();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	foreach (AMDSCommand commandDef, configurationCommandDefs_) {
		if (!AMDSCommand::encodeAndwriteAMDSCommand(dataStream, commandDef));
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;
	}

	// write the AMDSCommand configuration if there are any
	*dataStream << configurationCommands_.size();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	for (int index = 0; index < configurationCommands_.size(); index++) {
		QString command = configurationCommands_.key(index);
		*dataStream << command;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		*dataStream << configurationCommands_.value(command);
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
	int configurationCommandDefSize;
	int configurationCommandSize;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	// read the AMDSCommand definitions if there are any
	*dataStream >> configurationCommandDefSize;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	configurationCommandDefs_.clear();
	for (int index = 0; index < configurationCommandSize; index++) {
		AMDSCommand commandDef = AMDSCommand::decodeAndInstantiateAMDSCommand(dataStream);
		if (commandDef.isValid())
			configurationCommandDefs_.append(commandDef);
		else
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;
	}

	// read the AMDSCommand configuration if there are any
	*dataStream >> configurationCommandSize;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	configurationCommands_.clear();
	QString command;
	QVariant commandValue;
	for (int index = 0; index < configurationCommandSize; index++) {
		*dataStream >> command;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		*dataStream >> commandValue;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		appendCommand(command, commandValue);
	}

	// set the local variables
	setBufferName(readBufferName);

	return AMDS_CLIENTREQUEST_SUCCESS;
}

