#ifndef AMDSCOMMANDMANAGER_H
#define AMDSCOMMANDMANAGER_H

#include <QObject>
#include <QStringList>

#include "util/AMBiHash.h"

class AMDSCommand;

class AMDSCommandManager: public QObject
{
	Q_OBJECT
public:
	enum AMDSCommandDef{
		// acknowledge type
		AcknowledgeOk = 0,

		// request type
		RequestStartDwell,

		// customized commands
		CustomizedCommand = 1000
	};

	virtual ~AMDSCommandManager();

	/// returns the AMDSCommand with given command ID
	AMDSCommand amdsCommand(int commandID) const;
	/// returns the AMDSCommand with given command text
	AMDSCommand amdsCommand(const QString &commandText) const;
	/// returns the AMDSCommand with given command hex
	AMDSCommand amdsCommandFromHex(const QString &hex) const;

	/// returns the commandId with a given command
	int commandId(const QString &command) const;
	/// returns the command string from the command hex code
	QString commandFromHex(const QString &hex) const;

protected:
	/// constructor of AMDSCommandManager class, which is protected since AMDSCommandManager should be used as singleton
	AMDSCommandManager() { }

	/// pure virtual function to initialize the AMDS commands and the hex mappling
	virtual void initiateAMDSCommands() = 0;

protected:
	/// the list of the AMDS commands
	QList<AMDSCommand> commands_;

	/// the helper hash table for looking up the commands
	QHash<int, AMDSCommand> commandHash_;
	/// the helper hash table for looking up the commands
	AMBiHash<QString, AMDSCommand> commandHexMapping_;

};

/// this is the deinition of an AMDSCommand
class AMDSCommand
{
public:
	/// the definition of an AMDS command
	AMDSCommand(const int commandId=-1, const QString &hex = "", const QString &command = "", int timeout = -1, int retries = 0, QStringList responseHexes = QStringList());

	/// returns a string of the command
	QString toString() const;

	/// the commandId value of the AMDS command
	inline int commandId() const { return commandId_; }
	/// the hex value of the AMDS command
	inline QString hex() const { return hex_; }
	/// the string (command) of the AMDS command
	inline QString command() const { return command_; }
	/// the predefined timeout of the AMDS command
	inline int timeout() const { return timeout_; }
	/// the predefined retries of the AMDS command
	inline int retries() const  { return retries_; }
	/// the predefined response of the AMDS command
	inline QStringList responseHexes() const { return responseHexes_; }

protected:
	/// the ID of the AMDSCommand
	int commandId_;
	/// the hex of the AMDSCommand
	QString hex_;
	/// the command text of the AMDSCommand
	QString command_;
	/// the timeout of the AMDSCommand
	int timeout_;
	/// the retries of the AMDSCommand
	int retries_;
	/// the response hex of the AMDSCommand
	QStringList responseHexes_;
};

inline bool operator==(const AMDSCommand &cp1, const AMDSCommand &cp2){
	if(cp1.hex() == cp2.hex())
		return true;
	return false;
}

inline uint qHash(const AMDSCommand &command){
	return qHash(command.hex());
}

#endif // AMDSCOMMANDMANAGER_H
