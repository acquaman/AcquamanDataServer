#include "AMDSCentralServerSGMScaler.h"

#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "Detector/Scaler/AMDSScalerDetector.h"
#include "Detector/Scaler/AMDSScalerDetectorManager.h"
#include "util/AMErrorMonitor.h"
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
}

void AMDSCentralServerSGMScaler::initializeConfiguration()
{
	// initialize the detector manager for SGM scaler
	QList<quint8> enabledChannelIds = QList<quint8>() << 10 << 11 << 12 << 13 << 14 << 15;
	scalerConfigurationMap_ = new AMDSScalerConfigurationMap("Scaler (BL1611-ID-1)", "BL1611-ID-1:mcs", AMDSDataTypeDefinitions::Signed32, enabledChannelIds);
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

void AMDSCentralServerSGMScaler::initializeAndStartDataServer()
{
	// initialize the scaler dataserver
}

void AMDSCentralServerSGMScaler::wrappingUpInitialization()
{

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
		foreach (AMDSDataHolder *scalerDataHolder, scalerScanCountsDataHolder) {
			bufferGroup->append(scalerDataHolder);
		}
	} else {
		if(AMDSRunTimeSupport::debugAtLevel(1))
			AMErrorMon::alert(this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(scalerConfigurationMap_->scalerName()));
	}

}
