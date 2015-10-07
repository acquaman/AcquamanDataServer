#include "AmptekSDD123DetectorGroupSGM.h"

#include "DataHolder/AMDSDataHolder.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.h"

AmptekSDD123DetectorGroupSGM::AmptekSDD123DetectorGroupSGM(QList<AmptekSDD123ConfigurationMap *> configurationMaps, QObject *parent) :
	QObject(parent)
{
	AmptekSDD123DetectorManager *amptekDetectorManager;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		amptekDetectorManager = new AmptekSDD123EPICSDetectorManager(configurationMaps.at(x));

		connect(amptekDetectorManager, SIGNAL(clearHistrogramData(QString)), this, SIGNAL(clearHistrogramData(QString)));
		connect(amptekDetectorManager, SIGNAL(clearDwellHistrogramData(QString)), this, SIGNAL(clearDwellHistrogramData(QString)));
		connect(amptekDetectorManager, SIGNAL(newHistrogramReceived(QString, AMDSDataHolder *)), this, SIGNAL(onNewHistrogramData(QString, AMDSDataHolder*)));
		connect(amptekDetectorManager, SIGNAL(newDwellHistrogramReceived(QString, AMDSDataHolder *)), this, SIGNAL(onNewDwellHistrogramData(QString, AMDSDataHolder*)));

		detectorManagers_.insert(amptekDetectorManager->detectorName(), amptekDetectorManager);
	}
}

QList<AmptekSDD123DetectorManager*> AmptekSDD123DetectorGroupSGM::detectorManagers(){
	return detectorManagers_.values();
}
