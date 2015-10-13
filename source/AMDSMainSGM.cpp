#include <QtCore/QCoreApplication>

#include <QStringList>

#include "AMDSMainCommon.h"
#include "application/AMDSCentralServerSGM.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = "";
	quint16 port = 28044;
	bool result = AMDSMain_Common::parseArgument("AcquamanDataServerSGM", args, &interfaceType, &port);

	if (result) {
		AMDSMain_Common::initializeAppSettings(interfaceType, port);
		AMDSMain_Common::initializeRegisteredClasses();

		AMDSCentralServerSGM *dataServer = new AMDSCentralServerSGM();
		dataServer->initializeAndStartServices();
		return app.exec();
	}

	return 0;
}
