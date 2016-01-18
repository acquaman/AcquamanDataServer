#include "AMDSScalerDetectorManager.h"

#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"
#include "Detector/Scaler/AMDSDwellScalerDetector.h"

/// ==================== implementation of AMDSThreadedScalerDetectorManager ============================
AMDSThreadedScalerDetectorManager::AMDSThreadedScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
	: QObject(parent)
{
	detectorManagerThread_ = new QThread();

	scalerDetectorManager_ = new AMDSScalerDetectorManager(scalerConfiguration);
	scalerDetectorManager_->moveToThread(detectorManagerThread_);

	connect(detectorManagerThread_, SIGNAL(finished()), scalerDetectorManager_, SLOT(deleteLater()));

	detectorManagerThread_->start();
}

AMDSThreadedScalerDetectorManager::~AMDSThreadedScalerDetectorManager()
{
	if (detectorManagerThread_->isRunning())
		detectorManagerThread_->quit();

	detectorManagerThread_->deleteLater();
	detectorManagerThread_ = 0;
}


/// ==================== implementation of AMDSScalerDetectorManager ============================
AMDSScalerDetectorManager::AMDSScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent) :
    QObject(parent)
{
	scalerDetector_ = new AMDSScalerDetector(scalerConfiguration);
	dwellScalerDetector_ = new AMDSDwellScalerDetector(scalerConfiguration);

	connect(scalerDetector_, SIGNAL(detectorScanModeChanged(int)), this, SIGNAL(detectorScanModeChanged(int)));
	connect(scalerDetector_, SIGNAL(newScalerScanDataReceived(const AMDSDataHolderList)), this, SIGNAL(newScalerScanDataReceived(const AMDSDataHolderList)));
	connect(dwellScalerDetector_, SIGNAL(requestFlattenedData(double)), this, SIGNAL(requestFlattenedData(double)));
}

AMDSScalerDetectorManager::~AMDSScalerDetectorManager()
{
	scalerDetector_->deleteLater();
	dwellScalerDetector_->deleteLater();
}

void AMDSScalerDetectorManager::onStartScalerDwelling()
{
	scalerDetector_->onStartDwelling();
}

void AMDSScalerDetectorManager::onScalerStopDwelling()
{
	scalerDetector_->onStopDwelling();
}

void AMDSScalerDetectorManager::onEnableScalerChannel(int channelId)
{
	scalerDetector_->onEnableChannel(channelId);
}

void AMDSScalerDetectorManager::onDisableScalerChannel(int channelId)
{
	scalerDetector_->onDisableChannel(channelId);
}

void AMDSScalerDetectorManager::setFlattenedData(const QVector<double> &data)
{
	dwellScalerDetector_->setFlattenedData(data);
}

