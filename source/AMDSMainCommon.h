#ifndef AMDSMAIN_COMMON_H
#define AMDSMAIN_COMMON_H

#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QSettings>

#include "ClientRequest/AMDSClientRequestSupport.h"
#include "DataElement/AMDSEventDataSupport.h"
#include "DataHolder/AMDSDataHolderSupport.h"

namespace AMDSMain_Common {
	/*
	 * Print the usage of the application
	 */
	void printUsage(const QString &appName)
	{
		qDebug() << "\nUsages:\n"
				 << QString("	%1 --intf=[interface name] [--port=[portnumber]]\n").arg(appName)
				 << QString("	%1 --help\n").arg(appName)
				 << "\n";
	}

	/*
	 * parse the argument, return TRUE if parsed the arugments successfully
	 */
	bool parseArgument(const QString &appName, QStringList args, QString *interfaceType, quint16 *port) {
		QRegExp argInterfaceType("--intf=([a-z0-9]{1,})");
		QRegExp argPortType("--port=([0-9]{1,})");
		QRegExp argHelp("--help");

		*port = 28040;

		bool startServer = true;
		for (int i = 1; i < args.size(); ++i) {
			if (argInterfaceType.indexIn(args.at(i)) != -1) {
				*interfaceType = argInterfaceType.cap(1);
			} else if (argPortType.indexIn(args.at(i)) != -1) {
				QString value = argPortType.cap(1);
				*port = value.toUInt();
			} else if (argHelp.indexIn(args.at(i)) != -1) {
				startServer = false;
			}
		}

		if ((*interfaceType).length() == 0)
			startServer = false;

		if (!startServer)  {
			qDebug() << "\nERROR: Missing or invalid interface argument. \n";
			printUsage(appName);
		}

		return startServer;
	}

	/*
	 * Initialize the application settings
	 */
	void initializeAppSettings(QString interface, quint16 port)
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "AcquamanDataServer", "AcquamanDataServer");
		settings.beginGroup("Networking");
		if(!settings.contains("interface") || interface != settings.value("interface"))
			settings.setValue("interface", interface);

		if(!settings.contains("port") || port != settings.value("port").toUInt())
			settings.setValue("port", port);

		settings.endGroup();
	}

	void initializeRegisteredClasses() {
		AMDSClientRequestSupport::registerClientRequestClass();
		AMDSDataHolderSupport::registerDataHolderClass();
		AMDSEventDataSupport::registerEventDataObjectClass();
	}
}

#endif // AMDSMAIN_COMMON_H
