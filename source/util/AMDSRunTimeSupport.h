#ifndef AMDSRUNTIMESUPPORT_H
#define AMDSRUNTIMESUPPORT_H

#include <QtGlobal>

namespace AMDSRunTimeSupport {
	/// Support variable for the current debug level
	extern quint8 debugLevel_;

	/// Returns the current debug level
	quint8 debugLevel() const;

	/// Sets the debug level
	void setDebugLevel(quint8 debugLevel);

	/// Returns true if the requested level is less than or equal to the current level. So, you can pass in the relative level of your information (higher number is a higher level of debugging information) and get a bool as to whether you should print or not
	bool debugAtLevel(quint8 requestedLevel) const;
}

#endif // AMDSRUNTIMESUPPORT_H
