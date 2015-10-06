#include "AmptekSDD123DetectorGroup.h"

#include "ClientRequest/AMDSClientDataRequest.h"

#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123Detector.h"
#include "Detector/Amptek/AmptekSDD123EPICSDetectorManager.h"
#include "DataHolder/AMDSDataHolder.h"

AmptekSDD123DetectorGroup::AmptekSDD123DetectorGroup(QList<AmptekSDD123ConfigurationMap *> configurationMaps, quint64 maxCountSize, QObject *parent) :
	QObject(parent)
{
	AmptekSDD123Detector *amptekDetector;
	AmptekSDD123DetectorManager *amptekDetectorManager;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		AmptekSDD123ConfigurationMap *amptekConfiguration = configurationMaps.at(x);
		//TODO
//		AMDSBufferGroupInfo amptekBufferGroupInfo(amptekConfiguration->detectorName(), amptekConfiguration->localAddress().toString(), "Counts", amptekConfiguration->dataType(), amptekConfiguration->spectrumCountSize(), AMDSBufferGroupInfo::NoFlatten, amptekBufferGroupAxes);

		amptekDetector = new AmptekSDD123Detector(configurationMaps.at(x)->detectorName(), configurationMaps.at(x)->detectorBasePVName(), amptekConfiguration->dataType(), amptekConfiguration->spectrumCountSize());

		amptekDetectorManager = new AmptekSDD123EPICSDetectorManager(amptekDetector, maxCountSize);
		connect(amptekDetectorManager, SIGNAL(clientRequestProcessed(AMDSClientDataRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientDataRequest*)));

		detectorManagers_.insert(amptekDetector->name(), amptekDetectorManager);
	}
}

QList<AmptekSDD123DetectorManager*> AmptekSDD123DetectorGroup::detectorManagers(){
	return detectorManagers_.values();
}

void AmptekSDD123DetectorGroup::requestData(AMDSClientDataRequest *data)
{
	AmptekSDD123DetectorManager *amptekDetectorManager = detectorManagers_.value(data->bufferName());
	if (amptekDetectorManager) {
		data->setErrorMessage(QString("No Amptek detector with name %1").arg(data->bufferName()));
		emit clientRequestProcessed(data);
	} else {
		amptekDetectorManager->forwardDataRequest(data);
	}
}
