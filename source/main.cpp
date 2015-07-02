#include <QtCore/QCoreApplication>

#include "source/AMDSCentralServer.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	new AMDSCentralServer();

	return a.exec();
}
