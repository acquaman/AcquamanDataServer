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
	void printUsage(const QString &appName, const QString &errMsg="")
	{
		if (errMsg != "") {
			qDebug() << "\nERROR: " << errMsg << "\n";
		}

		qDebug() << "\nUsages:\n"
				 << QString("	%1 --intf=[interface name] [--port=[portnumber]] [--cfg=[cfgFileName]]\n").arg(appName)
				 << QString("	%1 --help\n").arg(appName)
				 << "\n";
	}

	/*
	 * parse the argument, return TRUE if parsed the arugments successfully
	 */
	bool parseArgument(const QString &appName, QStringList args, QString *interfaceType, quint16 *port, QString *cfgFileName) {
		QRegExp argInterfaceType("--intf=([a-zA-Z0-9]{1,})");
		QRegExp argPortNumber("--port=([0-9]{1,})");
		QRegExp argCfgFileName("--cfg=([[a-zA-Z0-9._/]{1,})");
		QRegExp argHelp("--help");

		*port = 28040;
		*cfgFileName = "";

		bool startServer = true;
		for (int i = 1; i < args.size(); ++i) {
			if (argInterfaceType.indexIn(args.at(i)) != -1) {
				*interfaceType = argInterfaceType.cap(1);
			} else if (argPortNumber.indexIn(args.at(i)) != -1) {
				QString value = argPortNumber.cap(1);
				*port = value.toUInt();
			} else if (argCfgFileName.indexIn(args.at(i)) != -1) {
				*cfgFileName = argCfgFileName.cap(1);
			} else if (argHelp.indexIn(args.at(i)) != -1) {
				startServer = false;
			}
		}

		if ((*interfaceType).length() == 0)
			startServer = false;

		if (!startServer)  {
			printUsage(appName, "Missing or invalid interface argument.");
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
