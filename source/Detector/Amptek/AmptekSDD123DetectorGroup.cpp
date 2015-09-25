#include "AmptekSDD123DetectorGroup.h"

#include "AmptekSDD123ConfigurationMap.h"
#include "AmptekSDD123Detector.h"
//#include "AmptekSDD123EPICSDetectorManager.h"
#include "ClientRequest/AMDSClientDataRequest.h"

AmptekSDD123DetectorGroup::AmptekSDD123DetectorGroup(QList<AmptekSDD123ConfigurationMap *> configurationMaps, QObject *parent) :
	QObject(parent)
{
	AmptekSDD123Detector *oneDetector;
	AmptekSDD123DetectorManager *oneDetectorManager;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		oneDetector = new AmptekSDD123Detector();
		oneDetector->setName(configurationMaps.at(x)->detectorName());

		oneDetectorManager = new AmptekSDD123EPICSDetectorManager(oneDetector, configurationMaps.at(x)->detectorBasePVName());
		connect(oneDetectorManager, SIGNAL(dataRequestReady(ClientDataRequest*)), this, SIGNAL(dataRequestReady(ClientDataRequest*)));
		detectorManagers_.append(oneDetectorManager);
	}
}

QList<AmptekSDD123DetectorManager*> AmptekSDD123DetectorGroup::detectorManagers(){
	return detectorManagers_;
}

//void AmptekSDD123DetectorGroup::requestData(ClientDataRequest *data)
//{
//	if(data->indexOfAmptek() < 0 || data->indexOfAmptek() >= detectorManagers_.count())
//	{
//		data->setError(QString("No Amptek detector at index %1").arg(data->indexOfAmptek()));
//		emit dataRequestReady(data);
//	}
//	else
//		detectorManagers_.at(data->indexOfAmptek())->forwardDataRequest(data);
//}
