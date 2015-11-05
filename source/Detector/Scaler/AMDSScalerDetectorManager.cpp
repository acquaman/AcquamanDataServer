#include "AMDSScalerDetectorManager.h"

#include "Detector/Scaler/AMDSScalerDetector.h"

AMDSScalerDetectorManager::AMDSScalerDetectorManager(const QString &scalerName, const QString &basePVName, const QList<quint8> &enabledChannelIdList, QObject *parent)
{
	detectorManagerThread_ = new QThread();

	scalerDetector_ = new AMDSScalerDetector(scalerName, basePVName, enabledChannelIdList);
	scalerDetector_->moveToThread(detectorManagerThread_);

	connect(detectorManagerThread_, SIGNAL(finished()), scalerDetector_, SLOT(deleteLater()));

	detectorManagerThread_->start();
}

AMDSScalerDetectorManager::~AMDSScalerDetectorManager()
{
	if (detectorManagerThread_->isRunning())
		detectorManagerThread_->terminate();

	detectorManagerThread_->deleteLater();
	detectorManagerThread_ = 0;
}
