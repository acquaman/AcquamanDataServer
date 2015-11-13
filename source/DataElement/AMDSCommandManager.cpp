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
AMDSCommand::AMDSCommand(int commandId, const QString &hex, const QString &command, int timeout, int retries, QStringList responseHexes)
{
	commandId_= commandId;
	hex_ = hex;
	command_ = command;
	timeout_ = timeout;
	retries_ = retries;
	responseHexes_ = responseHexes;
}

QString AMDSCommand::toString() const
{
	return QString("%1 --- %2 : %3").arg(commandId_).arg(hex_).arg(command_);
}
