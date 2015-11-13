#include "AMDSDetectorServer.h"

// =================== implementation of AMDSDetectorServerManager =======================
AMDSDetectorServerManager::AMDSDetectorServerManager(AMDSDetectorServer *detectorServer, QObject *parent )
	:QObject(parent)
{
	detectorServerThread_ = new QThread();

	detectorServer_ = detectorServer;
	detectorServer_->moveToThread(detectorServerThread_);
	connect(detectorServerThread_, SIGNAL(finished()), detectorServer_, SLOT(deleteLater()));

	detectorServerThread_->start();
}

AMDSDetectorServerManager::~AMDSDetectorServerManager()
{
	if (detectorServerThread_->isRunning())
		detectorServerThread_->quit();

	detectorServerThread_->deleteLater();
	detectorServerThread_ = 0;
}


void AMDSDetectorServerManager::forwardConfigurationRequest(AMDSClientRequest *configurationRequest)
{
	detectorServer_->onConfigurationRequestReceived(configurationRequest);
}

// =================== implementation of AMDSDetectorServer =======================
AMDSDetectorServer::AMDSDetectorServer(QObject *parent) :
    QObject(parent)
{
	setDetectorMode(AMDSDetectorServer::Configuration);
}

AMDSDetectorServer::~AMDSDetectorServer()
{

}

void AMDSDetectorServer::setDetectorMode(AMDSDetectorServer::DetectorMode detectorMode)
{
	if (detectorMode_ != detectorMode) {
		detectorMode_ = detectorMode;
		if (detectorMode_ == AMDSDetectorServer::Configuration) {
			emit serverChangedToConfigurationState();
		} else {
			emit serverChangedToDwellState();
		}
	}
}

void AMDSDetectorServer::onConfigurationRequestReceived(AMDSClientRequest *configurationRequest)
{
	Q_UNUSED(configurationRequest)

	setDetectorMode(AMDSDetectorServer::Configuration);
}
