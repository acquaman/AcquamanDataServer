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

QString AMDSClientConfigurationRequest::toString() const
{
	QString messageData = QString("Data of Coonfiguration message (%1):").arg(socketKey());

	if (configurationCommandDefs_.size() > 0)
		messageData = messageData + "\nAvailable configuration commands:";
	foreach (AMDSCommand commandDef, configurationCommandDefs_) {
		messageData = QString("%1\n	%2").arg(messageData).arg(commandDef.toString());
	}

	if (configurationCommandDefs_.size() > 0)
		messageData = messageData + "\nAvailable configuration requests:";
	foreach(int commandId, configurationCommands_.keys()) {
		foreach(QString command, configurationCommands_.values(commandId))
		messageData = QString("%1 \n	command (%2) : value (%3)").arg(messageData).arg(commandId).arg(command);
	}

	return messageData;
}

void AMDSClientConfigurationRequest::appendCommand(int commandId, const QString &value)
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

	// write the AMDSCommand definitions if there are any
	*dataStream << configurationCommandDefs_.size();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_DEF_SIZE;

	foreach (AMDSCommand commandDef, configurationCommandDefs_) {
		if (!AMDSCommand::encodeAndwriteAMDSCommand(dataStream, commandDef))
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_DEF;
	}

	// write the AMDSCommand configuration if there are any
	*dataStream << configurationCommands_.size();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND_SIZE;

	foreach (int commandId, configurationCommands_.keys()) {
		// it's a multi-map
		foreach (QString value, configurationCommands_.values(commandId)) {
			*dataStream << commandId;
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

			*dataStream << value;
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;
		}
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
	for (int index = 0; index < configurationCommandDefSize; index++) {
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
	int commandId;
	QString commandValue;
	for (int index = 0; index < configurationCommandSize; index++) {
		*dataStream >> commandId;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		*dataStream >> commandValue;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_CONFIGURATION_COMMAND;

		appendCommand(commandId, commandValue);
	}

	// set the local variables
	setBufferName(readBufferName);

	return AMDS_CLIENTREQUEST_SUCCESS;
}

