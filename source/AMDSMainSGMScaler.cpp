#include <QtCore/QCoreApplication>

#include <QStringList>

#include "AMDSMainCommon.h"
#include "application/AMDSCentralServerSGMScaler.h"
#include "util/AMDSRunTimeSupport.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = "";
	quint16 port = 28044;

	bool startServer = AMDSMain_Common::parseArgument("AcquamanDataServerSGMScaler", args, &interfaceType, &port);
	if (startServer) {
		AMDSRunTimeSupport::setDebugLevel(AMDSRunTimeSupport::Alert);
		AMDSMain_Common::initializeAppSettings(interfaceType, port);
		AMDSMain_Common::initializeRegisteredClasses();

		AMDSCentralServerSGMScaler *dataServer = new AMDSCentralServerSGMScaler();
		dataServer->initializeAndStartServices();
		return app.exec();
	}

	return 0;
}
