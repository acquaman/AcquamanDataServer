#ifndef ALLCOMMANDS_H
#define ALLCOMMANDS_H

#include <QObject>
#include <QStringList>

#include "util/AMBiHash.h"

class AmptekCommand;

class AmptekCommandManager: public QObject
{
	Q_OBJECT
public:
	virtual ~AmptekCommandManager() {}

	/// returns the AmptekCommand with given command text
	AmptekCommand amptekCommand(const QString &commandText) const;
	/// returns the AmptekCommand with given command ID
	AmptekCommand amptekCommand(int commandID) const;

protected:
	/// constructor of AmptekCommandManager class, which is protected since AmptekCommandManager should be used as singleton
	AmptekCommandManager() { }

	/// pure virtual function to initialize the amptek commands and the hex mappling
	virtual void initiateAmptekCommands() = 0;

protected:
	/// the list of the amptek commands
	QList<AmptekCommand> amptekCommands_;

	/// the helper hash table for looking up the commands
	QHash<int, AmptekCommand> amptekCommandHash_;
	/// the helper hash table for looking up the commands
	AMBiHash<QString, AmptekCommand> commandHexMapping_;

};

/// this is the deinition of an AmptekCommand
class AmptekCommand
{
public:
	/// the definition of an Amptek command
	AmptekCommand(const int commandId=-1, const QString &hex = "", const QString &command = "", int timeout = -1, int retries = 0, QStringList responseHexes = QStringList());

	/// the commandId value of the amptek command
	int commandId() const;
	/// the hex value of the amptek command
	QString hex() const;
	/// the string (command) of the amptek command
	QString command() const;
	/// the predefined timeout of the amptek command
	int timeout() const;
	/// the predefined retries of the amptek command
	int retries() const;
	/// the predefined response of the amptek command
	QStringList responseHexes() const;

protected:
	/// the ID of the AmptekCommand
	int commandId_;
	/// the hex of the AmptekCommand
	QString hex_;
	/// the command text of the AmptekCommand
	QString command_;
	/// the timeout of the AmptekCommand
	int timeout_;
	/// the retries of the AmptekCommand
	int retries_;
	/// the response hex of the AmptekCommand
	QStringList responseHexes_;
};

inline bool operator==(const AmptekCommand &cp1, const AmptekCommand &cp2){
	if(cp1.hex() == cp2.hex())
		return true;
	return false;
}

inline uint qHash(const AmptekCommand &amptekSDDCommand){
	return qHash(amptekSDDCommand.hex());
}

#endif // ALLCOMMANDS_H
