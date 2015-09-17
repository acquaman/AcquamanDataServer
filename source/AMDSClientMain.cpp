#include <QApplication>

#include "source/ui/AMDSClientUi.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/DataElement/AMDSEventDataSupport.h"
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
	AMDSClientUi client;
	client.show();
	
	return a.exec();
}
