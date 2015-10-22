#include "AmptekCommandManager.h"

AmptekCommand AmptekCommandManager::amptekCommand(int commandID) const {
	return amptekCommandHash_.value(commandID);
}

AmptekCommand AmptekCommandManager::amptekCommand(const QString &commandString) const{
	return commandHexMapping_.valueF(commandString);
}

AmptekCommand AmptekCommandManager::amptekCommandFromHex(const QString &hex) const
{
	QString command = commandFromHex(hex);
	return amptekCommand(command);
}

int AmptekCommandManager::commandId(const QString &command)
{
	return amptekCommand(command).commandId();
}

QString AmptekCommandManager::commandFromHex(const QString &hexString) const{
	AmptekCommand other(-1, hexString, "", -1);
	return commandHexMapping_.valueR(other);
}

/// ===================== implementation of AmptekCommand ==========================
AmptekCommand::AmptekCommand(int commandId, const QString &hex, const QString &command, int timeout, int retries, QStringList responseHexes)
{
	commandId_= commandId;
	hex_ = hex;
	command_ = command;
	timeout_ = timeout;
	retries_ = retries;
	responseHexes_ = responseHexes;
}

QString AmptekCommand::toString() const
{
	return QString("%1 --- %2 : %3").arg(commandId_).arg(hex_).arg(command_);
}
