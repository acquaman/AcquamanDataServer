#include <QtCore/QCoreApplication>

#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QSettings>

#include "application/AMDSCentralServerSGM.h"
#include "ClientRequest/AMDSClientRequestSupport.h"
#include "DataElement/AMDSEventDataSupport.h"
#include "DataHolder/AMDSDataHolderSupport.h"

/*
 * Print the usage of the application
 */
void printUsage()
{
	qDebug() << "\nUsages:" << "\n"
			 << "	AcquamanDataServer --intf=[interface name] [--port=[portnumber]]\n"
			 << "	AcquamanDataServer --help\n"
			 << "\n";
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

int main(int argc, char *argv[])
{
	QRegExp argInterfaceType("--intf=([a-z0-9]{1,})");
	QRegExp argPortType("--port=([0-9]{1,})");
	QRegExp argHelp("--help");

	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = 0;
	quint16 port = 28044;
	for (int i = 1; i < args.size(); ++i) {
		if (argInterfaceType.indexIn(args.at(i)) != -1) {
			interfaceType = argInterfaceType.cap(1);
		} else if (argPortType.indexIn(args.at(i)) != -1) {
			QString value = argPortType.cap(1);
			port = value.toUInt();
		} else if (argHelp.indexIn(args.at(i)) != -1) {
			printUsage();
			return 0;
		}
	}

	if (interfaceType == 0) {
		qDebug() << "\nERROR: Missing or invalid interface argument. \n";
		printUsage();
	} else {
		initializeAppSettings(interfaceType, port);
		initializeRegisteredClasses();

		AMDSCentralServerSGM *dataServer = new AMDSCentralServerSGM();
		dataServer->startDataServerUpdate();
		return app.exec();
	}

	return 0;
}
