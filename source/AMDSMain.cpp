#include <QtCore/QCoreApplication>

#include <QStringList>
#include <QRegExp>
#include <QDebug>

#include "source/AMDSCentralServer.h"

void printUsage()
{
	qDebug() << "\nUsages:" << "\n"
			 << "	AcquamanDataServer --hw=[HW type]\n"
			 << "	AcquamanDataServer --help\n"
			 << "\n";
}

int main(int argc, char *argv[])
{
	QRegExp argHWType("--hw=([a-z0-9]{1,})");
	QRegExp argHelp("--help");

	QCoreApplication app(argc, argv);
	QStringList args = app.arguments();

	bool missionAccomplished = false;
	QString hwType = 0;
	for (int i = 1; i < args.size(); ++i) {
		if (argHWType.indexIn(args.at(i)) != -1) {
			hwType = argHWType.cap(1);
		} else if (argHelp.indexIn(args.at(i)) != -1) {
			printUsage();
			missionAccomplished = true;
		}
	}

	if (missionAccomplished) {
		return 0;
	} else if (hwType == 0) {
		qDebug() << "\nERROR: Missing or invalid HW type argument. \n";
		printUsage();
	} else {
		new AMDSCentralServer(hwType);
		return app.exec();
	}

	return 0;
}
