#include "AMDSCentralServerSGMPV.h"

#include <QTimer>
#include <QFile>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "Detector/AMDSDetectorServer.h"
#include "Detector/AMDSPVConfigurationMap.h"

//#include "Detector/Scaler/AMDSScalerCommandManager.h"
//#include "Detector/Scaler/AMDSScalerDetector.h"
//#include "Detector/Scaler/AMDSScalerDetectorServer.h"
#include "util/AMDSRunTimeSupport.h"

AMDSCentralServerSGMPV::AMDSCentralServerSGMPV(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 20 * 60 * 1000; // 20 minuntes

	initializeConfiguration();
}

AMDSCentralServerSGMPV::~AMDSCentralServerSGMPV()
{
	foreach(AMDSPVConfigurationMap *pvConfiguration, pvConfigurationMaps_) {
		pvConfiguration->deleteLater();
	}
	pvConfigurationMaps_.clear();

//	scalerConfigurationMap_->deleteLater();
//	scalerDetectorManager_->deleteLater();
//	scalerDetectorServerManager_->deleteLater();
//	AMDSScalerCommandManager::releaseScalerCommands();
}

void AMDSCentralServerSGMPV::initializeConfiguration()
{
	// initialize the configurations of the PVs
	QRegExp rx("(\\,|\\t)"); //RegEx for ',' or '\t'
	QFile configurationFile("./AMDSPVConfiguration.txt");

	if (configurationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream dataStream(&configurationFile);

		QString pvConfiguration = dataStream.readLine(); // skip the first line, which is the title line
		while (!pvConfiguration.isNull())
		{
			pvConfiguration = dataStream.readLine();

			QStringList pvDefintions = pvConfiguration.split(rx);
			if (pvDefintions.size() == 5) {
				int dataType = pvDefintions.at(4).toInt();
				AMDSPVConfigurationMap *pvConfiguration = new AMDSPVConfigurationMap(pvDefintions.at(0), pvDefintions.at(1), pvDefintions.at(2), pvDefintions.at(3), (AMDSDataTypeDefinitions::DataType)dataType);
				pvConfigurationMaps_.append(pvConfiguration);
			} else {
				AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, 0, QString("AMDS PV: the pv configuration is invalid. (%1)").arg(pvConfiguration));
			}
		}

		configurationFile.close();
	}
}

void AMDSCentralServerSGMPV::initializeBufferGroup()
{
//	// initialize bufferGroup for scaler
//	QList<AMDSAxisInfo> scalerBufferGroupAxes;
//	scalerBufferGroupAxes << AMDSAxisInfo("Channel", scalerConfigurationMap_->enabledChannels().count(), "Channel Axis", "");
//	AMDSBufferGroupInfo scalerBufferGroupInfo(scalerConfigurationMap_->scalerName(), scalerConfigurationMap_->scalerName(), "Counts", scalerConfigurationMap_->dataType(), AMDSBufferGroupInfo::NoFlatten, scalerBufferGroupAxes);

//	AMDSThreadedBufferGroup *scalerThreadedBufferGroup = new AMDSThreadedBufferGroup(scalerBufferGroupInfo, maxBufferSize_, false);
//	connect(scalerThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

//	bufferGroupManagers_.insert(scalerThreadedBufferGroup->bufferGroupName(), scalerThreadedBufferGroup);
}

void AMDSCentralServerSGMPV::initializeDetectorManager()
{
//	// initialize the detector manager for SGM scaler
//	scalerDetectorManager_ = new AMDSScalerDetectorManager(scalerConfigurationMap_);
//	connect(scalerDetectorManager_->scalerDetector(), SIGNAL(newScalerScanDataReceived(AMDSDataHolderList)), this, SLOT(onNewScalerScanDataReceivedd(AMDSDataHolderList)));
}

void AMDSCentralServerSGMPV::initializeAndStartDetectorServer()
{
//	// initialize the scaler detector server
//	AMDSScalerDetectorServer *scalerDetectorServer = new AMDSScalerDetectorServer(scalerConfigurationMap_->scalerName());
//	scalerDetectorServerManager_ = new AMDSDetectorServerManager(scalerDetectorServer);

//	connect(scalerDetectorServerManager_->detectorServer(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
//	connect(this, SIGNAL(configurationRequestReceived(AMDSClientRequest*)), scalerDetectorServerManager_->detectorServer(), SLOT(onConfigurationRequestReceived(AMDSClientRequest*)));
}

void AMDSCentralServerSGMPV::wrappingUpInitialization()
{
//	// connect scaler detector with the scaler detector server
//	AMDSScalerDetectorServer *scalerServer = qobject_cast<AMDSScalerDetectorServer *>(scalerDetectorServerManager_->detectorServer());
//	if (scalerServer) {
//		// when we start/restart dwelling, we need to clear the exiting buffer since the existing data might NOT match the current configuration
//		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), this, SLOT(onDetectorServerStartDwelling(QString)));

//		connect(scalerServer, SIGNAL(serverGoingToStartDwelling(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerGoingToStartDwelling()));
//		connect(scalerServer, SIGNAL(serverChangedToConfigurationMode(QString)), scalerDetectorManager_->scalerDetector(), SLOT(onServerStopDwelling()));
//		connect(scalerServer, SIGNAL(enableScalerChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onEnableChannel(int)));
//		connect(scalerServer, SIGNAL(disableScalerChannel(int)), scalerDetectorManager_->scalerDetector(), SLOT(onDisableChannel(int)));

//		connect(scalerDetectorManager_->scalerDetector(), SIGNAL(detectorScanModeChanged(int)), scalerServer, SLOT(onDetectorServerModeChanged(int)));
//	}
}

void AMDSCentralServerSGMPV::fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest)
{
//	if (bufferName == scalerConfigurationMap_->scalerName()) {
//		AMDSCommandManager *commandMananger = AMDSScalerCommandManager::scalerCommandManager();
//		foreach(AMDSCommand commandDef, commandMananger->commands()) {
//			clientRequest->appendCommandDef(commandDef);
//		}
//	}
}

void AMDSCentralServerSGMPV::onNewScalerScanDataReceivedd(const AMDSDataHolderList &scalerScanCountsDataHolder)
{
//	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(scalerConfigurationMap_->scalerName());
//	if (bufferGroup) {
//		bufferGroup->append(scalerScanCountsDataHolder);
//	} else {
//		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(scalerConfigurationMap_->scalerName()));
//	}
}
