#include "AMDSCentralServerSGMScaler.h"

#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "Detector/AMDSDetectorServer.h"
#include "Detector/Scaler/AMDSScalerCommandManager.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"
#include "Detector/Scaler/AMDSScalerDetectorServer.h"
#include "util/AMDSRunTimeSupport.h"

AMDSCentralServerSGMScaler::AMDSCentralServerSGMScaler(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 1 * 60 * 1000; // 20 minuntes

	initializeConfiguration();
}

AMDSCentralServerSGMScaler::~AMDSCentralServerSGMScaler()
{
	scalerConfigurationMap_->deleteLater();
	scalerDetectorManager_->deleteLater();
	scalerDetectorServerManager_->deleteLater();
	AMDSScalerCommandManager::releaseScalerCommands();
}

void AMDSCentralServerSGMScaler::initializeConfiguration()
{
	// initialize the detector manager for SGM scaler
	QList<quint8> configuredChannelIds = QList<quint8>();
//	for (int i = 0; i < 32; i++)
//		configuredChannelIds << i;

	// These are the channels that actually need to be enabled right now.
	configuredChannelIds << 0 << 1 << 2 << 3 << 6 << 7 << 8 << 9 << 10 << 11 << 14 << 15;

	scalerConfigurationMap_ = new AMDSScalerConfigurationMap("Scaler (BL1611-ID-1)", "BL1611-ID-1:mcs", AMDSDataTypeDefinitions::Signed32, configuredChannelIds);
}

void AMDSCentralServerSGMScaler::initializeBufferGroup()
{
	// initialize bufferGroup for scaler
	QList<AMDSAxisInfo> scalerBufferGroupAxes;
	scalerBufferGroupAxes << AMDSAxisInfo("Channel", scalerConfigurationMap_->enabledChannels().count(), "Channel Axis", "");
	AMDSBufferGroupInfo scalerBufferGroupInfo(scalerConfigurationMap_->scalerName(), scalerConfigurationMap_->scalerName(), "Counts", scalerConfigurationMap_->dataType(), AMDSBufferGroupInfo::NoFlatten, scalerBufferGroupAxes);

	AMDSThreadedBufferGroup *scalerThreadedBufferGroup = new AMDSThreadedBufferGroup(scalerBufferGroupInfo, maxBufferSize_, false);
	connect(scalerThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

	bufferGroupManagers_.insert(scalerThreadedBufferGroup->bufferGroupName(), scalerThreadedBufferGroup);
}

void AMDSCentralServerSGMScaler::initializeDetectorManager()
{
	// initialize the detector manager for SGM scaler
	scalerDetectorManager_ = new AMDSScalerDetectorManager(scalerConfigurationMap_);
	connect(scalerDetectorManager_->scalerDetector(), SIGNAL(newScalerScanDataReceived(AMDSDataHolderList)), this, SLOT(onNewScalerScanDataReceivedd(AMDSDataHolderList)));
}

void AMDSCentralServerSGMScaler::initializeAndStartDetectorServer()
{
	// initialize the scaler detector server
	AMDSScalerDetectorServer *scalerDetectorServer = new AMDSScalerDetectorServer(scalerConfigurationMap_->scalerName());
	scalerDetectorServerManager_ = new AMDSDetectorServerManager(scalerDetectorServer);

	connect(scalerDetectorServerManager_->detectorServer(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
	connect(this, SIGNAL(configurationRequestReceived(AMDSClientRequest*)), scalerDetectorServerManager_->detectorServer(), SLOT(onConfigurationRequestReceived(AMDSClientRequest*)));
}

void AMDSCentralServerSGMScaler::wrappingUpInitialization()
{
	// connect scaler detector with the scaler detector server
	AMDSScalerDetectorServer *scalerServer = qobject_cast<AMDSScalerDetectorServer *>(scalerDetectorServerManager_->detectorServer());
	if (scalerServer) {
		// when we start/restart dwelling, we need to clear the exiting buffer since the existing data might NOT match the current configuration
		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), this, SLOT(onDetectorServerStartDwelling(QString)));

		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerGoingToStartDwelling()));
		connect(scalerServer, SIGNAL(serverChangedToConfigurationMode(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerStopDwelling()));
		connect(scalerServer, SIGNAL(enableScalerChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onEnableChannel(int)));
		connect(scalerServer, SIGNAL(disableScalerChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onDisableChannel(int)));

		connect(scalerDetectorManager_->scalerDetector(), SIGNAL(detectorScanModeChanged(int)), scalerServer, SLOT(onDetectorServerModeChanged(int)));
	}
}

void AMDSCentralServerSGMScaler::fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest)
{
	if (bufferName == scalerConfigurationMap_->scalerName()) {
		AMDSCommandManager *commandMananger = AMDSScalerCommandManager::scalerCommandManager();
		foreach(AMDSCommand commandDef, commandMananger->commands()) {
			clientRequest->appendCommandDef(commandDef);
		}
	}
}

void AMDSCentralServerSGMScaler::onServerChangedToConfigurationState(int index){
	emit serverChangedToConfigurationState(index);
}

void AMDSCentralServerSGMScaler::onServerChangedToDwellState(int index){
	emit serverChangedToDwellState(index);
}

void AMDSCentralServerSGMScaler::onNewScalerScanDataReceivedd(const AMDSDataHolderList &scalerScanCountsDataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(scalerConfigurationMap_->scalerName());
	if (bufferGroup) {
		bufferGroup->append(scalerScanCountsDataHolder);
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(scalerConfigurationMap_->scalerName()));
	}
}
