#include "AMDSThreadedTcpDataServer.h"

#include <QCoreApplication>

AMDSThreadedTcpDataServer::AMDSThreadedTcpDataServer(QObject *parent) :
	QObject(parent)
{
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
		settings.setValue("interface", "lo0");
//		settings.setValue("interface", "eth0");
	QString interfaceName = settings.value("interface").toString();
	if(!settings.contains("port"))
		settings.setValue("port", "28044");
	quint16 port = settings.value("port").toInt();
	settings.endGroup();
	qDebug() << "Going to request server to start";
	//emit startServer(interfaceName, port);
	emit startServer("en3", port);
}
