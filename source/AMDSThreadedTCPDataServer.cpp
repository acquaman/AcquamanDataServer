#include "source/AMDSThreadedTCPDataServer.h"

#include <QCoreApplication>

#include "source/util/AMDSErrorMonitor.h"

AMDSThreadedTCPDataServer::AMDSThreadedTCPDataServer(QObject *parent) :
	QObject(parent)
{
	thread_ = new QThread;
	server_ = new AMDSTCPDataServer;

	connect(thread_, SIGNAL(started()), this, SLOT(onThreadStarted()));
	connect(this, SIGNAL(startServer(QString,quint16)), server_, SLOT(start(QString,quint16)));

	server_->moveToThread(thread_);
	thread_->start(QThread::LowPriority);
}

AMDSThreadedTCPDataServer::~AMDSThreadedTCPDataServer()
{
	if (thread_->isRunning())
		thread_->terminate();

	server_->deleteLater();
	thread_->deleteLater();
}

AMDSTCPDataServer* AMDSThreadedTCPDataServer::server()
{
	return server_;
}

void AMDSThreadedTCPDataServer::onThreadStarted()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "AcquamanDataServer", "AcquamanDataServer");
	settings.beginGroup("Networking");
	QString interface = settings.value("interface").toString();
	quint16 port = settings.value("port").toUInt();
	settings.endGroup();

	AMDSErrorMon::information(this, 0, QString("Starting TCP data server at (%1, %2)").arg(interface).arg(port));

	emit startServer(interface, port);
}
