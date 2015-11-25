#include "AMDSCentralServerSGMPV.h"

#include <QTimer>
#include <QFile>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"

#include "Detector/AMDSDetectorServer.h"
#include "Detector/PVController/AMDSPVCommandManager.h"
#include "Detector/PVController/AMDSPVConfigurationMap.h"
#include "Detector/PVController/AMDSPVController.h"
#include "Detector/PVController/AMDSPVControllerServer.h"

#include "util/AMDSRunTimeSupport.h"

AMDSCentralServerSGMPV::AMDSCentralServerSGMPV(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 20 * 60 * 1000; // 20 minuntes

	initializeConfiguration();
}

AMDSCentralServerSGMPV::~AMDSCentralServerSGMPV()
{
	foreach(AMDSPVConfigurationMap *pvConfiguration, pvConfigurationMaps_.values()) {
		pvConfigurationMaps_.remove(pvConfiguration->pvName());
		pvConfiguration->deleteLater();
	}
	pvConfigurationMaps_.clear();

	pvControllerManager_->deleteLater();
	pvControllerServerManager_->deleteLater();

	AMDSPVCommandManager::releasePVCommandManager();
}

void AMDSCentralServerSGMPV::onNewPVDataReceived(const QString &bufferName, AMDSDataHolder *newData)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(bufferName);
	if (bufferGroup) {
		bufferGroup->append(newData);
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(bufferName));
	}
}

void AMDSCentralServerSGMPV::initializeConfiguration()
{
	// initialize the configurations of the PVs
	QRegExp rx("(\\,|\\t)"); //RegEx for ',' or '\t'
	int totalParameters = 6;
//	int dataTypeParamIndex = totalParameters - 1;

	QFile configurationFile("../db/AMDSPVConfiguration.txt");

	if (configurationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream dataStream(&configurationFile);

		QString pvConfiguration = dataStream.readLine(); // skip the first line, which is the title line
		while (!pvConfiguration.isNull())
		{
			pvConfiguration = dataStream.readLine();
			if (!pvConfiguration.isNull()) {
				QStringList pvDefintions = pvConfiguration.split(rx);
				if (pvDefintions.size() == totalParameters) {
	//				int dataType = pvDefintions.at(dataTypeParamIndex).toInt();
					AMDSPVConfigurationMap *pvConfiguration = new AMDSPVConfigurationMap(pvDefintions.at(0).trimmed(), //enabled
																						 pvDefintions.at(1).trimmed(), //name
																						 pvDefintions.at(2).trimmed(), //description
																						 pvDefintions.at(3).trimmed(), // epics PV
																						 pvDefintions.at(4).trimmed(), // units
																						 AMDSDataTypeDefinitions::Double // we will do double so far since the AMPVControll will always returns double
	//																					 (AMDSDataTypeDefinitions::DataType)dataType //dataType
																						 );
					pvConfigurationMaps_.insert(pvConfiguration->pvName(), pvConfiguration);
				} else {
					AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, 0, QString("AMDS PV: the pv configuration is invalid. (%1)").arg(pvConfiguration));
				}
			}
		}

		configurationFile.close();
	}
}

void AMDSCentralServerSGMPV::initializeBufferGroup()
{
	// initialize bufferGroup for PV
	foreach (AMDSPVConfigurationMap *pvConfiguration, pvConfigurationMaps_.values()) {
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
	// initialize the pv controller server
	AMDSPVControllerServer *pvControllerServer = new AMDSPVControllerServer("AMDSPV_SGM");
	pvControllerServerManager_ = new AMDSDetectorServerManager(pvControllerServer);

	connect(pvControllerServerManager_->detectorServer(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)));
	connect(this, SIGNAL(configurationRequestReceived(AMDSClientRequest*)), pvControllerServerManager_->detectorServer(), SLOT(onConfigurationRequestReceived(AMDSClientRequest*)));
}

void AMDSCentralServerSGMPV::wrappingUpInitialization()
{
	// connect PV controller with the pv controller server
	AMDSPVControllerServer *pvControllerServer = qobject_cast<AMDSPVControllerServer *>(pvControllerServerManager_->detectorServer());
	if (pvControllerServer) {
		connect(pvControllerServer, SIGNAL(enablePVController(QString)), pvControllerManager_->pvController(), SLOT(onEnablePVController(QString)));
		connect(pvControllerServer, SIGNAL(disablePVController(QString)), pvControllerManager_->pvController(), SLOT(onDisablePVController(QString)));
	}
}

void AMDSCentralServerSGMPV::fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest)
{
	Q_UNUSED(bufferName)

	AMDSCommandManager *commandMananger = AMDSPVCommandManager::pvCommandManager();
	foreach(AMDSCommand commandDef, commandMananger->commands()) {
		clientRequest->appendCommandDef(commandDef);
	}
}

void AMDSCentralServerSGMPV::processClientDataRequest(AMDSClientRequest *clientRequest)
{
	// make sure that we only continue AMDS PV data request when the PV is enabled
	QString bufferName = "";
	bool continueProcessDataRequest = false;
	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest*>(clientRequest);
	if(clientDataRequest){
		bufferName = clientDataRequest->bufferName();
		AMDSPVConfigurationMap *pvConfiguration = pvConfigurationMaps_.value(bufferName);
		if (pvConfiguration && pvConfiguration->enabled()) {
			continueProcessDataRequest = true;
		}
	}

	// only when the PV is enabled, we will move forward
	if (continueProcessDataRequest) {
		AMDSCentralServer::processClientDataRequest(clientRequest);
	} else {
		QString errorMessage = QString("ERROR: %1 - Request data from a disabled bufferGroup (%2) or invalid data request type (%3).").arg(AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME).arg(bufferName).arg(clientRequest->requestType());
		clientRequest->setErrorMessage(errorMessage);
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME, errorMessage);
		emit clientRequestProcessed(clientRequest);
	}
}

