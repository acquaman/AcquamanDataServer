#include "AMDSCentralServerSGMAmptek.h"

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "DataElement/AMDSThreadedBufferGroupManager.h"
#include "DataHolder/AMDSSpectralDataHolder.h"
#include "Detector/AMDSDetectorServer.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123DetectorManager.h"
#include "Detector/Amptek/AmptekSDD123ServerGroup.h"
#include "Detector/Amptek/AmptekSDD123Server.h"
#include "Detector/Scaler/AMDSScalerCommandManager.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"
#include "Detector/Scaler/AMDSScalerDetectorServer.h"
#include "util/AMDSRunTimeSupport.h"

#include "Detector/Amptek/SGM/AmptekCommandManagerSGM.h"
#include "Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.h"


AMDSCentralServerSGMAmptek::AMDSCentralServerSGMAmptek(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 20 * 60 * 20; // 20 minuntes data recording --> 20 minutes * 60 seconds/minute * 20 polls/second (it's polling at 50ms)

	initializeConfiguration();
}

AMDSCentralServerSGMAmptek::~AMDSCentralServerSGMAmptek()
{
	// release the Amptek information
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, amptekConfigurationMaps_) {
		amptekConfiguration->deleteLater();
	}
	amptekConfigurationMaps_.clear();

	amptekThreadedDataServerGroup_->deleteLater();
	amptekThreadedDataServerGroup_ = 0;

	AmptekCommandManagerSGM::releaseAmptekCommands();

}

void AMDSCentralServerSGMAmptek::initializeConfiguration()
{
	// initialize the configuration for SGM amptek

	// Commented out PV and IP details for actual beamline ampteks, so can test on some detectors without interfering with beamline operations
//	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.139"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Unsigned16, 1024, this));

	amptekConfigurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.140"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Unsigned16, 1024, this));
	amptekConfigurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 241", "amptek:sdd3", QHostAddress("192.168.0.241"), QHostAddress("192.168.0.141"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Unsigned16, 1024, this));
	amptekConfigurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 242", "amptek:sdd4", QHostAddress("192.168.0.242"), QHostAddress("192.168.0.142"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Unsigned16, 1024, this));
	amptekConfigurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 243", "amptek:sdd5", QHostAddress("192.168.0.243"), QHostAddress("192.168.0.143"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Unsigned16, 1024, this));
}

void AMDSCentralServerSGMAmptek::initializeBufferGroup()
{
	// initialize buffergroup for Amptek
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, amptekConfigurationMaps_) {

		QList<AMDSAxisInfo> amptekBufferGroupAxes;
		amptekBufferGroupAxes << AMDSAxisInfo("Energy", amptekConfiguration->spectrumCountSize(), "Energy Axis", "eV");

		AMDSBufferGroupInfo amptekBufferGroupInfo(amptekConfiguration->detectorName(), amptekConfiguration->localAddress().toString(), "Counts", amptekConfiguration->dataType(), AMDSBufferGroupInfo::Summary, amptekBufferGroupAxes);

		AMDSThreadedBufferGroupManager *amptekBufferGroupManager = new AMDSThreadedBufferGroupManager(amptekBufferGroupInfo, maxBufferSize_);
		connect(amptekBufferGroupManager->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
		connect(amptekBufferGroupManager->bufferGroup(), SIGNAL(internalRequestProcessed(AMDSClientRequest*)), this, SLOT(onInternalRequestProcessed(AMDSClientRequest*)));

		bufferGroupManagers_.insert(amptekBufferGroupManager->bufferGroupName(), amptekBufferGroupManager);
	}
}

void AMDSCentralServerSGMAmptek::initializeDetectorManager()
{
	// initialize the detector managers for SGM Amptek
	amptekDetectorGroup_ = new AmptekSDD123DetectorGroupSGM(amptekConfigurationMaps_);
	foreach (AmptekSDD123DetectorManager * detectorManager, amptekDetectorGroup_->detectorManagers()) {
		connect(detectorManager, SIGNAL(clearHistrogramData(QString)), this, SLOT(onClearHistrogramData(QString)));
		connect(detectorManager, SIGNAL(newHistrogramReceived(QString, AMDSDataHolder*)), this, SLOT(onNewHistrogramReceived(QString, AMDSDataHolder*)));
		connect(detectorManager, SIGNAL(requestFlattenedSpectrumData(QString,double)), this, SLOT(onAmptekDetectorRequestFlattenedData(QString,double)));
	}
}

void AMDSCentralServerSGMAmptek::initializeAndStartDetectorServer()
{
	// initialize the amptek dataserver
	amptekThreadedDataServerGroup_ = new AmptekSDD123ThreadedDataServerGroup(amptekConfigurationMaps_);

	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToDwellState(int)), this, SLOT(onServerChangedToDwellState(int)));
}

void AMDSCentralServerSGMAmptek::wrappingUpInitialization()
{
	QList<AmptekSDD123DetectorManager*> amptekDetectorManagerList = amptekDetectorGroup_->detectorManagers();

	// connect the events among the amptek data servers and the detectors
	for(int x = 0, size = amptekDetectorManagerList.count(); x < size; x++){
		AmptekSDD123DetectorManager *amptekDetectorManager = amptekDetectorManagerList.at(x);
		AmptekSDD123Server *amptekServer = amptekThreadedDataServerGroup_->serverAt(x);

		amptekServer->setSpectrumPacketReceiver((QObject *)amptekDetectorManager->detector());
		amptekServer->setConfirmationPacketReceiver(amptekDetectorManager);

		amptekDetectorManager->setRequestEventReceiver(amptekServer);
	}
}

void AMDSCentralServerSGMAmptek::fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest)
{
	Q_UNUSED(bufferName) Q_UNUSED(clientRequest)
}

void AMDSCentralServerSGMAmptek::onServerChangedToConfigurationState(int index){
	emit serverChangedToConfigurationState(index);
}

void AMDSCentralServerSGMAmptek::onServerChangedToDwellState(int index){
	emit serverChangedToDwellState(index);
}

void AMDSCentralServerSGMAmptek::onClearHistrogramData(const QString &detectorName)
{
	AMDSThreadedBufferGroupManager * bufferGroupManager = bufferGroupManagers_.value(detectorName);
	if (bufferGroupManager) {
		bufferGroupManager->clearBufferGroup();
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGMAmptek::onNewHistrogramReceived(const QString &detectorName, AMDSDataHolder *dataHolder)
{
	AMDSThreadedBufferGroupManager * bufferGroupManager = bufferGroupManagers_.value(detectorName);
	if (bufferGroupManager) {
		bufferGroupManager->append(dataHolder);

		if(dwellSecondsRequestedForDetector_.contains(detectorName)){
			double dwellSecondsRequested = dwellSecondsRequestedForDetector_.value(detectorName);
			dwellSecondsRequestedForDetector_.remove(detectorName);

			QDateTime endTime = QDateTime::currentDateTimeUtc();
			quint64 asMsecs = quint64(dwellSecondsRequested*1000);
			QDateTime startTime = endTime.addMSecs(-asMsecs);
			AMDSClientStartTimeToEndTimeDataRequest *localRequest = new AMDSClientStartTimeToEndTimeDataRequest(AMDSClientRequest::Binary, endTime, "", bufferGroupManager->bufferGroupName(), false, true, startTime, endTime, bufferGroupManager->bufferGroupInfo(), this);
			bufferGroupManager->bufferGroup()->processClientRequest(localRequest, true);
		}

	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGMAmptek::onAmptekDetectorRequestFlattenedData(const QString &detectorName, double seconds)
{
	dwellSecondsRequestedForDetector_.insert(detectorName, seconds);
}

void AMDSCentralServerSGMAmptek::onInternalRequestProcessed(AMDSClientRequest *clientRequest)
{
		AMDSClientStartTimeToEndTimeDataRequest *returnedRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
		if(returnedRequest && returnedRequest->data().count() > 0){
			AMDSDwellSpectralDataHolder *spectralDataHolder = qobject_cast<AMDSDwellSpectralDataHolder*>(returnedRequest->data().at(0));
			if(spectralDataHolder){
				if(amptekDetectorGroup_->detectorManager(returnedRequest->bufferName()))
					amptekDetectorGroup_->detectorManager(returnedRequest->bufferName())->setFlattenedSpectrumData(spectralDataHolder);
			}
		}

		clientRequest->deleteLater();
}
