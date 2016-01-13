#include "AMDSCentralServerSGMScaler.h"

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

	internalRequestActive_ = false;
	dwellSecondsRequested_ = -1;

	initializeConfiguration();
}

AMDSCentralServerSGMScaler::~AMDSCentralServerSGMScaler()
{
	scalerConfigurationMap_->deleteLater();
	threadedScalerDetector_->deleteLater();
	scalerDetectorServerManager_->deleteLater();
	AMDSScalerCommandManager::releaseScalerCommandManager();
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
	AMDSBufferGroupInfo scalerBufferGroupInfo(scalerConfigurationMap_->scalerName(), scalerConfigurationMap_->scalerName(), "Counts", scalerConfigurationMap_->dataType(), AMDSBufferGroupInfo::Summary, scalerBufferGroupAxes);

	AMDSThreadedBufferGroup *scalerThreadedBufferGroup = new AMDSThreadedBufferGroup(scalerBufferGroupInfo, maxBufferSize_);
	connect(scalerThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

	connect(scalerThreadedBufferGroup->bufferGroup(), SIGNAL(internalRequestProcessed(AMDSClientRequest*)), this, SLOT(onInternalRequestProcessed(AMDSClientRequest*)));

	bufferGroupManagers_.insert(scalerThreadedBufferGroup->bufferGroupName(), scalerThreadedBufferGroup);
}

void AMDSCentralServerSGMScaler::initializeDetectorManager()
{
	// initialize the detector manager for SGM scaler
	threadedScalerDetector_ = new AMDSThreadedScalerDetector(scalerConfigurationMap_);
	connect(threadedScalerDetector_->scalerDetector(), SIGNAL(newScalerScanDataReceived(AMDSDataHolderList)), this, SLOT(onNewScalerScanDataReceived(AMDSDataHolderList)));
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

		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), threadedScalerDetector_->scalerDetector(), SLOT(onServerGoingToStartDwelling()));
		connect(scalerServer, SIGNAL(serverChangedToConfigurationMode(QString)), threadedScalerDetector_->scalerDetector(), SLOT(onServerStopDwelling()));
		connect(scalerServer, SIGNAL(enableScalerChannel(int)), threadedScalerDetector_->scalerDetector(), SLOT(onEnableChannel(int)));
		connect(scalerServer, SIGNAL(disableScalerChannel(int)), threadedScalerDetector_->scalerDetector(), SLOT(onDisableChannel(int)));

		connect(threadedScalerDetector_->scalerDetector(), SIGNAL(detectorScanModeChanged(int)), scalerServer, SLOT(onDetectorServerModeChanged(int)));

		connect(threadedScalerDetector_->scalerDetector(), SIGNAL(requestFlattenedData(double)), this, SLOT(onScalerDetectorRequestFlattenedData(double)));
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

#include "ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
void AMDSCentralServerSGMScaler::onNewScalerScanDataReceived(const AMDSDataHolderList &scalerScanCountsDataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(scalerConfigurationMap_->scalerName());
	if (bufferGroup) {
		bufferGroup->append(scalerScanCountsDataHolder);

		if(internalRequestActive_){
			internalRequestActive_ = false;

			QDateTime endTime = QDateTime::currentDateTime();
			quint64 asMsecs = quint64(dwellSecondsRequested_*1000);
			QDateTime startTime = endTime.addMSecs(-asMsecs);
			AMDSClientStartTimeToEndTimeDataRequest *localRequest = new AMDSClientStartTimeToEndTimeDataRequest(AMDSClientRequest::Binary, endTime, "", scalerConfigurationMap_->scalerName(), false, true, startTime, endTime, bufferGroup->bufferGroupInfo(), this);
			bufferGroup->bufferGroup()->processClientRequest(localRequest, true);
		}
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(scalerConfigurationMap_->scalerName()));
	}
}

void AMDSCentralServerSGMScaler::onScalerDetectorRequestFlattenedData(double seconds)
{
	internalRequestActive_ = true;
	dwellSecondsRequested_ = seconds;
}

#include "DataHolder/AMDSScalarDataHolder.h"
void AMDSCentralServerSGMScaler::onInternalRequestProcessed(AMDSClientRequest *clientRequest)
{
	AMDSClientStartTimeToEndTimeDataRequest *returnedRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
	if(returnedRequest && returnedRequest->data().count() > 0){
//		qDebug() << "It's the right type, what is the count " << returnedRequest->data().count();
//		qDebug() << "Data type is " << returnedRequest->data().at(0)->metaObject()->className();
		AMDSLightWeightScalarDataHolder *scalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(returnedRequest->data().at(0));
		if(scalarDataHolder){
//			qDebug() << "It's the right type of data holder with count " << scalarDataHolder->dataArray().asConstVectorDouble().count();
//			qDebug() << scalarDataHolder->dataArray().asConstVectorDouble();

			threadedScalerDetector_->scalerDetector()->setFlattenedData(scalarDataHolder->dataArray().asConstVectorDouble());
		}
	}
}
