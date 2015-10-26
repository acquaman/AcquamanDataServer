#include "AmptekSDD123DetectorGroupSGM.h"

#include "DataHolder/AMDSDataHolder.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/SGM/AmptekSDD123EPICSDetectorManager.h"

AmptekSDD123DetectorGroupSGM::AmptekSDD123DetectorGroupSGM(QList<AmptekSDD123ConfigurationMap *> configurationMaps, QObject *parent) :
	QObject(parent)
{
	QThread *amptekDetectorManagerThread;
	AmptekSDD123DetectorManager *amptekDetectorManager;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		amptekDetectorManagerThread = new QThread;

		amptekDetectorManager = new AmptekSDD123EPICSDetectorManager(configurationMaps.at(x));
		amptekDetectorManager->moveToThread(amptekDetectorManagerThread);

		connect(amptekDetectorManagerThread, SIGNAL(finished()), amptekDetectorManager, SLOT(deleteLater()));

		detectorManagerThreads_.insert(amptekDetectorManager->detectorName(), amptekDetectorManagerThread);
		detectorManagers_.insert(amptekDetectorManager->detectorName(), amptekDetectorManager);

		amptekDetectorManagerThread->start();
	}
}

AmptekSDD123DetectorGroupSGM::~AmptekSDD123DetectorGroupSGM()
{
	foreach (QThread *thread, detectorManagerThreads_.values()) {
		if (thread->isRunning())
			thread->quit();

		thread->deleteLater();
	}

	detectorManagerThreads_.clear();
	detectorManagers_.clear();
}

QList<AmptekSDD123DetectorManager*> AmptekSDD123DetectorGroupSGM::detectorManagers(){
	return detectorManagers_.values();
}
