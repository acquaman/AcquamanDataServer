#include <QCoreApplication>

#include "source/AMDSCentralServer.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	AMDSCentralServer *centralServer = new AMDSCentralServer();

	return a.exec();
}
