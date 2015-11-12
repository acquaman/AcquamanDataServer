#include "AMDSScalerDetectorManager.h"

#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"

AMDSScalerDetectorManager::AMDSScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
{
	detectorManagerThread_ = new QThread();

	scalerDetector_ = new AMDSScalerDetector(scalerConfiguration);
	scalerDetector_->moveToThread(detectorManagerThread_);

	connect(detectorManagerThread_, SIGNAL(finished()), scalerDetector_, SLOT(deleteLater()));

	detectorManagerThread_->start();
}

AMDSScalerDetectorManager::~AMDSScalerDetectorManager()
{
	if (detectorManagerThread_->isRunning())
		detectorManagerThread_->quit();

	detectorManagerThread_->deleteLater();
	detectorManagerThread_ = 0;
}
