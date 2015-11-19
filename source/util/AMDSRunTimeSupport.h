#ifndef AMDSRUNTIMESUPPORT_H
#define AMDSRUNTIMESUPPORT_H

#include <QtGlobal>
#include <QObject>

namespace AMDSRunTimeSupport {
	enum DebugMsgType {
		DebugMsg,
		InformationMsg,
		AlertMsg,
		ErrorMsg
	};

	enum DebugLevel {
		None = 0,
		Error,      // error message only
		Alert,      // alert and error messages
		Information,// alert, error and information message
		Debug,      // equal to all
		All
	};


	/// Support variable for the current debug level
	extern DebugLevel debugLevel_;

	/// Returns the current debug level
	DebugLevel debugLevel();

	/// Sets the debug level
	void setDebugLevel(DebugLevel debugLevel);

	/// Returns true if the requested level is less than or equal to the current level. So, you can pass in the relative level of your information (higher number is a higher level of debugging information) and get a bool as to whether you should print or not
	bool debugAtLevel(DebugLevel requestedLevel);

	/// write the debug message
	void debugMessage(DebugMsgType type, const QObject *src = 0, int code = 0, const QString &desc = "");
}

#endif // AMDSRUNTIMESUPPORT_H
