#include "source/AMDSThreadedTcpDataServer.h"

#include <QCoreApplication>

AMDSThreadedTcpDataServer::AMDSThreadedTcpDataServer(QString hwType, QObject *parent) :
	QObject(parent)
{
	hwType_ = hwType;

	server_.moveToThread(&thread_);
	connect(&thread_, SIGNAL(started()), this, SLOT(onThreadStarted()));
	connect(this, SIGNAL(startServer(QString,quint16)), &server_, SLOT(start(QString,quint16)));
	thread_.start(QThread::LowPriority);
}

AMDSTcpDataServer* AMDSThreadedTcpDataServer::server()
{
	return &server_;
}

void AMDSThreadedTcpDataServer::onThreadStarted()
{
	QSettings settings;
	settings.beginGroup("Networking");
	if(!settings.contains("interface"))
		settings.setValue("interface", hwType_);

	if(!settings.contains("port"))
		settings.setValue("port", "28044");

	quint16 port = settings.value("port").toInt();
	settings.endGroup();
	qDebug() << "Going to request server to start";

	emit startServer(hwType_, port);
}
