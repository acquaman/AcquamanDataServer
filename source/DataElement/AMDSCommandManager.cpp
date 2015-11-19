#include "AMDSCommandManager.h"

AMDSCommandManager::~AMDSCommandManager()
{
	commands_.clear();
	commandHash_.clear();
	commandHexMapping_.clear();
}

AMDSCommand AMDSCommandManager::amdsCommand(int commandID) const {
	return commandHash_.value(commandID);
}

AMDSCommand AMDSCommandManager::amdsCommand(const QString &commandString) const{
	return commandHexMapping_.valueF(commandString);
}

AMDSCommand AMDSCommandManager::amdsCommandFromHex(const QString &hex) const
{
	QString command = commandFromHex(hex);
	return amdsCommand(command);
}

int AMDSCommandManager::commandId(const QString &command) const
{
	return amdsCommand(command).commandId();
}

QString AMDSCommandManager::commandFromHex(const QString &hexString) const{
	AMDSCommand other(-1, hexString, "", -1);
	return commandHexMapping_.valueR(other);
}

/// ===================== implementation of AMDSCommand ==========================

AMDSCommand AMDSCommand::decodeAndInstantiateAMDSCommand(QDataStream *dataStream)
{
	AMDSCommand commandDef;
	commandDef.readFromDataStream(dataStream);
	return commandDef;
}

bool AMDSCommand::encodeAndwriteAMDSCommand(QDataStream *dataStream, AMDSCommand command)
{
	return command.writeToDataStream(dataStream);
}

AMDSCommand::AMDSCommand(int commandId, const QString &hex, const QString &command, int timeout, int retries, const QStringList &responseHexes)
{
	commandId_= commandId;
	hex_ = hex;
	command_ = command;
	timeout_ = timeout;
	retries_ = retries;
	responseHexes_ = responseHexes;
}

bool AMDSCommand::writeToDataStream(QDataStream *dataStream)
{
	// we only need to read/write commandId and command string for the AMDSCommand
	*dataStream << commandId();
//	*dataStream << hex();
	*dataStream << command();
//	*dataStream << timeout();
//	*dataStream << retries();
//	*dataStream << responseHexes();

	return true;
}

bool AMDSCommand::readFromDataStream(QDataStream *dataStream)
{
	// we only need to read/write commandId and command string for the AMDSCommand
	int readCommandId;
//	QString readHex;
	QString readCommand;
//	int readTimeout;
//	int readRetries;
//	QStringList readResponseHexes;

	*dataStream >> readCommandId;
//	*dataStream >> readHex;
	*dataStream >> readCommand;
//	*dataStream >> readTimeout;
//	*dataStream >> readRetries;
//	*dataStream >> readResponseHexes;

	commandId_ =readCommandId;
//	hex_ = readHex;
	command_ = readCommand;
//	timeout_ = readTimeout;
//	retries_ = readRetries;
//	responseHexes_ = readResponseHexes;

	return true;
}

QString AMDSCommand::toString() const
{
	return QString("%1 --- %2 : %3").arg(commandId_).arg(hex_).arg(command_);
}
