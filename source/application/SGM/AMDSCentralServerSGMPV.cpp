#include "AMDSCentralServerSGMPV.h"

#include <QTimer>
#include <QFile>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"

#include "Detector/AMDSDetectorServer.h"
#include "Detector/PVController/AMDSPVConfigurationMap.h"
#include "Detector/PVController/AMDSPVController.h"

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

	pvControllerManager_->deleteLater();

	//	scalerDetectorServerManager_->deleteLater();
//	AMDSScalerCommandManager::releaseScalerCommands();
}

void AMDSCentralServerSGMPV::initializeConfiguration()
{
	// initialize the configurations of the PVs
	QRegExp rx("(\\,|\\t)"); //RegEx for ',' or '\t'
	int totalParameters = 6;

	QFile configurationFile("./AMDSPVConfiguration.txt");

	if (configurationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream dataStream(&configurationFile);

		QString pvConfiguration = dataStream.readLine(); // skip the first line, which is the title line
		while (!pvConfiguration.isNull())
		{
			pvConfiguration = dataStream.readLine();

			QStringList pvDefintions = pvConfiguration.split(rx);
			if (pvDefintions.size() == totalParameters) {
				int dataType = pvDefintions.at(4).toInt();
				AMDSPVConfigurationMap *pvConfiguration = new AMDSPVConfigurationMap(pvDefintions.at(0), pvDefintions.at(1), pvDefintions.at(2), pvDefintions.at(3), pvDefintions.at(4), (AMDSDataTypeDefinitions::DataType)dataType);
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
	// initialize bufferGroup for PV
	foreach (AMDSPVConfigurationMap *pvConfiguration, pvConfigurationMaps_) {
		QList<AMDSAxisInfo> pvBufferGroupAxes;
		pvBufferGroupAxes << AMDSAxisInfo(pvConfiguration->pvName(), 1, pvConfiguration->pvDescription(), pvConfiguration->pvUnits());
		AMDSBufferGroupInfo pvBufferGroupInfo(pvConfiguration->pvName(), pvConfiguration->pvDescription(), pvConfiguration->pvUnits(), pvConfiguration->dataType(), AMDSBufferGroupInfo::NoFlatten, pvBufferGroupAxes);

		AMDSThreadedBufferGroup *pvThreadedBufferGroup = new AMDSThreadedBufferGroup(pvBufferGroupInfo, maxBufferSize_, false);
		connect(pvThreadedBufferGroup->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

		bufferGroupManagers_.insert(pvThreadedBufferGroup->bufferGroupName(), pvThreadedBufferGroup);
	}
}

void AMDSCentralServerSGMPV::initializeDetectorManager()
{
	// initialize the pv controller manager for AMDS SGM PV
	pvControllerManager_ = new AMDSPVControllerManager(pvConfigurationMaps_);

	connect(pvControllerManager_->pvController(), SIGNAL(newPVDataReceived(QString, AMDSDataHolder*)), this, SLOT(onNewPVDataReceived(QString, AMDSDataHolder*)));
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

void AMDSCentralServerSGMPV::onNewPVDataReceived(const QString &bufferName, AMDSDataHolder *newData)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(bufferName);
	if (bufferGroup) {
		bufferGroup->append(newData);
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(bufferName));
	}
}
