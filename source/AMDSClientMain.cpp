#include <QApplication>

#include "AMDSClient.h"
#include "source/AMDSEventDataSupport.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"

void initializeRegisteredClasses() {
	AMDSClientRequestSupport::registerClientRequestClass();
	AMDSDataHolderSupport::registerDataHolderClass();
	AMDSEventDataSupport::registerEventDataObjectClass();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	initializeRegisteredClasses();
	AMDSClient client;
	client.show();
	
	return a.exec();
}