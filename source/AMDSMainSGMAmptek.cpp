#include <QtCore/QCoreApplication>

#include <QStringList>

#include "AMDSMainCommon.h"
#include "application/SGM/AMDSCentralServerSGMAmptek.h"
#include "util/AMDSRunTimeSupport.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = "";
	quint16 port = 28044;
	QString cfgFileName = "";

	bool startServer = AMDSMain_Common::parseArgument("AcquamanDataServerSGMAmptek", args, &interfaceType, &port, &cfgFileName);
	if (startServer) {
		AMDSMain_Common::initializeAppSettings(interfaceType, port);
		AMDSMain_Common::initializeRegisteredClasses();

		AMDSCentralServerSGMAmptek *dataServer = new AMDSCentralServerSGMAmptek();
		dataServer->initializeAndStartServices();
		return app.exec();
	}

	return 0;
}
