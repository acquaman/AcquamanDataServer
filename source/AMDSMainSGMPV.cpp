#include <QtCore/QCoreApplication>

#include <QStringList>

#include "AMDSMainCommon.h"
#include "application/SGM/AMDSCentralServerSGMPV.h"
#include "util/AMDSRunTimeSupport.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = "";
	quint16 port = 28044;
	QString cfgFileName = "";

	bool startServer = AMDSMain_Common::parseArgument("AcquamanDataServerSGMPV", args, &interfaceType, &port, &cfgFileName);
	if (startServer) {
		if (cfgFileName.trimmed().length() == 0) {
			AMDSMain_Common::printUsage("AcquamanDataServerSGMPV", "missing the configuration file name for AMDS PV.");
		} else {
			AMDSMain_Common::initializeAppSettings(interfaceType, port);
			AMDSMain_Common::initializeRegisteredClasses();

			AMDSCentralServerSGMPV *dataServer = new AMDSCentralServerSGMPV(cfgFileName);
			dataServer->initializeAndStartServices();
			return app.exec();
		}
	}

	return 0;
}
