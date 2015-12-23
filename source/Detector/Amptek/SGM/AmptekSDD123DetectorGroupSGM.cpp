#include "AmptekSDD123DetectorGroupSGM.h"

#include "DataHolder/AMDSDataHolder.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.h"

AmptekSDD123DetectorGroupSGM::AmptekSDD123DetectorGroupSGM(const QList<AmptekSDD123ConfigurationMap *> &configurationMaps, QObject *parent) :
	QObject(parent)
{
	AmptekSDD123DetectorManager *amptekDetectorManager;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		amptekDetectorManager = new AmptekSDD123EPICSDetectorManager(configurationMaps.at(x));
		detectorManagers_.insert(amptekDetectorManager->detectorName(), amptekDetectorManager);
	}
}

AmptekSDD123DetectorGroupSGM::~AmptekSDD123DetectorGroupSGM()
{
	foreach(QString detectorName, detectorManagers_.keys()) {
		detectorManagers_.value(detectorName)->deleteLater();
	}

	detectorManagers_.clear();
}

QList<AmptekSDD123DetectorManager*> AmptekSDD123DetectorGroupSGM::detectorManagers() const
{
	return detectorManagers_.values();
}
