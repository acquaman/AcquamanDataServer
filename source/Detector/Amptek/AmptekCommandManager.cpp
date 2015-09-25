#include "AmptekCommandManager.h"

AmptekCommand AmptekCommandManager::amptekCommand(const QString &commandString) const{
	return commandHexMapping_.valueF(commandString);
}

AmptekCommand AmptekCommandManager::amptekCommand(int commandID) const {
	return amptekCommandHash_.value(commandID);
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

int AmptekCommand::commandId() const
{
	return commandId_;
}

QString AmptekCommand::hex() const{
	return hex_;
}

QString AmptekCommand::command() const{
	return command_;
}

int AmptekCommand::timeout() const{
	return timeout_;
}

int AmptekCommand::retries() const{
	return retries_;
}

QStringList AmptekCommand::responseHexes() const{
	return responseHexes_;
}
