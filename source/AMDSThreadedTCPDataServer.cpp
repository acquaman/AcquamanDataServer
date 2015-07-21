#include "source/AMDSThreadedTCPDataServer.h"

#include <QCoreApplication>

AMDSThreadedTCPDataServer::AMDSThreadedTCPDataServer(QString hwType, QObject *parent) :
	QObject(parent)
{
	hwType_ = hwType;

	thread_ = new QThread;
	server_ = new AMDSTCPDataServer;

	connect(thread_, SIGNAL(started()), this, SLOT(onThreadStarted()));
	connect(this, SIGNAL(startServer(QString,quint16)), server_, SLOT(start(QString,quint16)));
	connect(server_, SIGNAL(error(quint8,quint16,QString)), this, SIGNAL(error(quint8,quint16,QString)));

	server_->moveToThread(thread_);
	thread_->start(QThread::LowPriority);
}

AMDSThreadedTCPDataServer::~AMDSThreadedTCPDataServer()
{
	if (thread_->isRunning())
		thread_->quit();

	thread_->deleteLater();
	server_->deleteLater();
}

AMDSTCPDataServer* AMDSThreadedTCPDataServer::server()
{
	return server_;
}

void AMDSThreadedTCPDataServer::onThreadStarted()
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
