#include <QApplication>

#include "ui/AMDSClientUi.h"
#include "ClientRequest/AMDSClientRequestSupport.h"
#include "DataElement/AMDSEventDataSupport.h"
#include "DataHolder/AMDSDataHolderSupport.h"

void initializeRegisteredClasses() {
	AMDSClientRequestSupport::registerClientRequestClass();
	AMDSDataHolderSupport::registerDataHolderClass();
	AMDSEventDataSupport::registerEventDataObjectClass();
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	initializeRegisteredClasses();
	AMDSClientUi client;
	client.show();
	
	return a.exec();
}
