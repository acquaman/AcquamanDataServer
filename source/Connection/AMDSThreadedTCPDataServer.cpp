#include "AMDSThreadedTCPDataServer.h"

#include <QCoreApplication>

#include "ClientRequest/AMDSClientDataRequest.h"
#include "util/AMDSRunTimeSupport.h"

AMDSThreadedTCPDataServer::AMDSThreadedTCPDataServer(QObject *parent) :
	QObject(parent)
{
	thread_ = new QThread;

	server_ = new AMDSTCPDataServer;
	server_->moveToThread(thread_);

	connect(thread_, SIGNAL(started()), this, SLOT(onThreadStarted()));
	connect(thread_, SIGNAL(finished()), server_, SLOT(deleteLater()));

	connect(this, SIGNAL(startServer(QString,quint16)), server_, SLOT(start(QString,quint16)));

	thread_->start(QThread::LowPriority);
}

AMDSThreadedTCPDataServer::~AMDSThreadedTCPDataServer()
{
	if (thread_->isRunning())
		thread_->quit();

	thread_->deleteLater();
	thread_ = 0;
}

void AMDSThreadedTCPDataServer::onThreadStarted()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "AcquamanDataServer", "AcquamanDataServer");
	settings.beginGroup("Networking");
	QString interface = settings.value("interface").toString();
	quint16 port = settings.value("port").toUInt();
	settings.endGroup();

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::InformationMsg, this, AMDS_SERVER_INFO_STARTING_TCP_DATA_SERVER, QString("Starting TCP data server at (%1, %2)").arg(interface).arg(port));

	emit startServer(interface, port);
}
