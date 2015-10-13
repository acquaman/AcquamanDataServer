#include <QtCore/QCoreApplication>

#include <QStringList>

#include "AMDSMainCommon.h"
#include "application/AMDSCentralServerSample.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	QString interfaceType = "";
	quint16 port = 28044;
	bool result = AMDSMain_Common::parseArgument("AcquamanDataServerSample", args, &interfaceType, &port);

	if (result) {
		AMDSMain_Common::initializeAppSettings(interfaceType, port);
		AMDSMain_Common::initializeRegisteredClasses();

		AMDSCentralServerSample *dataServer = new AMDSCentralServerSample();
		dataServer->initializeAndStartServices();
		return app.exec();
	}

	return 0;
}
