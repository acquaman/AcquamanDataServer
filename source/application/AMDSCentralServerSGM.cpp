#include "AMDSCentralServerSGM.h"

#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123DetectorManager.h"
#include "Detector/Amptek/AmptekSDD123ServerGroup.h"
#include "Detector/Amptek/AmptekSDD123Server.h"
#include "Detector/Scaler/AMDSScalerCommandManager.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"
#include "Detector/Scaler/AMDSScalerDetectorServer.h"
#include "Detector/AMDSDetectorServer.h"

#include "util/AMErrorMonitor.h"

#include "Detector/Amptek/SGM/AmptekCommandManagerSGM.h"
#include "Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.h"

AMDSCentralServerSGM::AMDSCentralServerSGM(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 1 * 60 * 1000; // 20 minuntes data recording

	initializeConfiguration();
}

AMDSCentralServerSGM::~AMDSCentralServerSGM()
{
	// release the Amptek information
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, amptekConfigurationMaps_) {
		amptekConfiguration->deleteLater();
	}
	amptekConfigurationMaps_.clear();

	foreach (AMDSThreadedBufferGroup *bufferGroup, dwellBufferGroupManagers_) {
		bufferGroup->deleteLater();
	}
	dwellBufferGroupManagers_.clear();

	amptekThreadedDataServerGroup_->deleteLater();
	amptekThreadedDataServerGroup_ = 0;

	AmptekCommandManagerSGM::releaseAmptekCommands();

	// release the Scaler information
	scalerConfigurationMap_->deleteLater();
	scalerDetectorManager_->deleteLater();
	scalerDetectorServerManager_->deleteLater();
	AMDSScalerCommandManager::releaseScalerCommands();
}

void AMDSCentralServerSGM::initializeConfiguration()
{
	// initialize the configuration for SGM amptek

	// Commented out PV and IP details for actual beamline ampteks, so can test on some detectors without interfering with beamline operations
//	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.139"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	amptekConfigurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.140"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
//	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 241", "amptek:sdd3", QHostAddress("192.168.0.241"), QHostAddress("192.168.0.141"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
//	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 242", "amptek:sdd4", QHostAddress("192.168.0.242"), QHostAddress("192.168.0.142"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
//	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 243", "amptek:sdd5", QHostAddress("192.168.0.243"), QHostAddress("192.168.0.143"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));

	// initialize the detector manager for SGM scaler
	QList<quint8> enabledChannelIds = QList<quint8>() << 10 << 11 << 12 << 13 << 14 << 15;
	scalerConfigurationMap_ = new AMDSScalerConfigurationMap("Scaler (BL1611-ID-1)", "BL1611-ID-1:mcs", AMDSDataTypeDefinitions::Signed32, enabledChannelIds);
}

void AMDSCentralServerSGM::initializeBufferGroup()
{
	// initialize buffergroup for Amptek
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, amptekConfigurationMaps_) {

		QList<AMDSAxisInfo> amptekBufferGroupAxes;
		amptekBufferGroupAxes << AMDSAxisInfo("Energy", amptekConfiguration->spectrumCountSize(), "Energy Axis", "eV");

		AMDSBufferGroupInfo amptekBufferGroupInfo(amptekConfiguration->detectorName(), amptekConfiguration->localAddress().toString(), "Counts", amptekConfiguration->dataType(), AMDSBufferGroupInfo::NoFlatten, amptekBufferGroupAxes);
		amptekBufferGroupInfo.setConfigurationCommandManager(AmptekCommandManagerSGM::amptekCommandManager());

		AMDSThreadedBufferGroup *amptekThreadedBufferGroup = new AMDSThreadedBufferGroup(amptekBufferGroupInfo, maxBufferSize_, false);
		connect(amptekThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

		AMDSThreadedBufferGroup *amptekDwellThreadedBufferGroup = new AMDSThreadedBufferGroup(amptekBufferGroupInfo, maxBufferSize_, true);

		bufferGroupManagers_.insert(amptekThreadedBufferGroup->bufferGroupName(), amptekThreadedBufferGroup);
		dwellBufferGroupManagers_.insert(amptekDwellThreadedBufferGroup->bufferGroupName(), amptekDwellThreadedBufferGroup);
	}

	// initialize bufferGroup for scaler
	QList<AMDSAxisInfo> scalerBufferGroupAxes;
	scalerBufferGroupAxes << AMDSAxisInfo("Channel", scalerConfigurationMap_->enabledChannels().count(), "Channel Axis", "");
	AMDSBufferGroupInfo scalerBufferGroupInfo(scalerConfigurationMap_->scalerName(), scalerConfigurationMap_->scalerName(), "Counts", scalerConfigurationMap_->dataType(), AMDSBufferGroupInfo::NoFlatten, scalerBufferGroupAxes);
	scalerBufferGroupInfo.setConfigurationCommandManager(AMDSScalerCommandManager::scalerCommandManager());

	AMDSThreadedBufferGroup *scalerThreadedBufferGroup = new AMDSThreadedBufferGroup(scalerBufferGroupInfo, maxBufferSize_, false);
	connect(scalerThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

	bufferGroupManagers_.insert(scalerThreadedBufferGroup->bufferGroupName(), scalerThreadedBufferGroup);
}

void AMDSCentralServerSGM::initializeDetectorManager()
{
	// initialize the detector managers for SGM Amptek
	amptekDetectorGroup_ = new AmptekSDD123DetectorGroupSGM(amptekConfigurationMaps_);
	foreach (AmptekSDD123DetectorManager * detectorManager, amptekDetectorGroup_->detectorManagers()) {
		connect(detectorManager, SIGNAL(clearHistrogramData(QString)), this, SLOT(onClearHistrogramData(QString)));
		connect(detectorManager, SIGNAL(clearDwellHistrogramData(QString)), this, SLOT(onClearDwellHistrogramData(QString)));
		connect(detectorManager, SIGNAL(newHistrogramReceived(QString, AMDSDataHolder*)), this, SLOT(onNewHistrogramReceived(QString, AMDSDataHolder*)));
		connect(detectorManager, SIGNAL(newDwellHistrogramReceived(QString, AMDSDataHolder*, double)), this, SLOT(onNewDwellHistrogramReceived(QString, AMDSDataHolder*, double)));
		connect(detectorManager, SIGNAL(dwellFinishedUpdate(QString,double)), this, SLOT(onDwellFinishedUpdate(QString,double)));
	}

	// initialize the detector manager for SGM scaler
	scalerDetectorManager_ = new AMDSScalerDetectorManager(scalerConfigurationMap_);
	connect(scalerDetectorManager_->scalerDetector(), SIGNAL(newScalerScanDataReceived(AMDSDataHolderList)), this, SLOT(onNewScalerScanDataReceivedd(AMDSDataHolderList)));
}

void AMDSCentralServerSGM::initializeAndStartDetectorServer()
{
	// initialize the amptek dataserver
	amptekThreadedDataServerGroup_ = new AmptekSDD123ThreadedDataServerGroup(amptekConfigurationMaps_);

	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToDwellState(int)), this, SLOT(onServerChangedToDwellState(int)));

	// initialize the scaler detector server
	AMDSScalerDetectorServer *scalerDetectorServer = new AMDSScalerDetectorServer(scalerConfigurationMap_->scalerName());
	scalerDetectorServerManager_ = new AMDSDetectorServerManager(scalerDetectorServer);
	connect(this, SIGNAL(scalerConfigurationRequestReceived(const AMDSClientRequest*)), scalerDetectorServerManager_->detectorServer(), SLOT(onConfigurationRequestReceived(AMDSClientRequest*)));
}

void AMDSCentralServerSGM::wrappingUpInitialization()
{
	QList<AmptekSDD123DetectorManager*> amptekDetectorManagerList = amptekDetectorGroup_->detectorManagers();

	// connect the event among the amptek data servers and the detectors
	for(int x = 0, size = amptekDetectorManagerList.count(); x < size; x++){
		AmptekSDD123DetectorManager *amptekDetectorManager = amptekDetectorManagerList.at(x);
		AmptekSDD123Server *amptekServer = amptekThreadedDataServerGroup_->serverAt(x);
//		AMDSThreadedBufferGroup *threadedBufferGroup = bufferGroupManagers_.value(amptekDetectorManager->detectorName());
		AMDSThreadedBufferGroup *threadedBufferGroup = dwellBufferGroupManagers_.value(amptekDetectorManager->detectorName());

		amptekServer->setSpectrumPacketReceiver((QObject *)amptekDetectorManager->detector());
		amptekServer->setConfirmationPacketReceiver(amptekDetectorManager);

		amptekDetectorManager->setRequestEventReceiver(amptekServer);

		connect(threadedBufferGroup->bufferGroup(), SIGNAL(continuousAllDataUpdate(AMDSDataHolder*,AMDSDwellStatusData,int,double)), amptekDetectorManager, SLOT(onContinuousAllDataUpdate(AMDSDataHolder*,AMDSDwellStatusData,int,double)));
		connect(threadedBufferGroup->bufferGroup(), SIGNAL(dwellFinishedAllDataUpdate(AMDSDataHolder *,AMDSDwellStatusData,int,double)), amptekDetectorManager, SLOT(onDwellFinishedAllDataUpdate(AMDSDataHolder *,AMDSDwellStatusData,int,double)));
	}

	// connect scaler detector with the scaler detector server
	AMDSScalerDetectorServer *scalerServer = qobject_cast<AMDSScalerDetectorServer *>(scalerDetectorServerManager_->detectorServer());
	if (scalerServer) {
		// when we start/restart dwelling, we need to clear the exiting buffer since the existing data might NOT match the current configuration
		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), this, SLOT(onDetectorServerStartDwelling()));

		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerGoingToStartDwelling()));
		connect(scalerServer, SIGNAL(serverChangedToConfigurationState(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerStopDwelling()));
		connect(scalerServer, SIGNAL(enableChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onEnableChannel(int)));
		connect(scalerServer, SIGNAL(disableChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onDisableChannel(int)));
	}
}

void AMDSCentralServerSGM::fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest)
{
	if (bufferName == scalerConfigurationMap_->scalerName()) {
		AMDSCommandManager *commandMananger = AMDSScalerCommandManager::scalerCommandManager();
		foreach(AMDSCommand commandDef, commandMananger->commands()) {
			clientRequest->appendCommandDef(commandDef);
		}
	}
}

void AMDSCentralServerSGM::onServerChangedToConfigurationState(int index){
	emit serverChangedToConfigurationState(index);
}

void AMDSCentralServerSGM::onServerChangedToDwellState(int index){
	emit serverChangedToDwellState(index);
}

void AMDSCentralServerSGM::onClearHistrogramData(const QString &detectorName)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(detectorName);
	if (bufferGroup) {
		bufferGroup->clearBufferGroup();
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGM::onClearDwellHistrogramData(const QString &detectorName)
{
	AMDSThreadedBufferGroup * bufferGroup = dwellBufferGroupManagers_.value(detectorName);
	if (bufferGroup) {
		bufferGroup->clearBufferGroup();
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGM::onNewHistrogramReceived(const QString &detectorName, AMDSDataHolder *dataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(detectorName);
	if (bufferGroup) {
		bufferGroup->append(dataHolder);
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGM::onNewDwellHistrogramReceived(const QString &detectorName, AMDSDataHolder *dataHolder, double elapsedTime)
{
	AMDSThreadedBufferGroup * bufferGroup = dwellBufferGroupManagers_.value(detectorName);
	if (bufferGroup) {
		bufferGroup->append(dataHolder, elapsedTime);
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGM::onDwellFinishedUpdate(const QString &detectorName, double elapsedTime)
{
	AMDSThreadedBufferGroup * bufferGroup = dwellBufferGroupManagers_.value(detectorName);
	if (bufferGroup) {
		bufferGroup->finishDwellDataUpdate(elapsedTime);
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(detectorName));
	}
}

void AMDSCentralServerSGM::onNewScalerScanDataReceivedd(const AMDSDataHolderList &scalerScanCountsDataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(scalerConfigurationMap_->scalerName());
	if (bufferGroup) {
		bufferGroup->append(scalerScanCountsDataHolder);
	} else {
		AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(scalerConfigurationMap_->scalerName()));
	}

}
