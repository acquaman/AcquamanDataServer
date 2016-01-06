#include "AMDSCentralServerSGMPV.h"

#include <QFile>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "DataElement/AMDSThreadedBufferGroupManager.h"

#include "Detector/AMDSDetectorServer.h"
#include "Detector/PVController/AMDSPVCommandManager.h"
#include "Detector/PVController/AMDSPVConfigurationMap.h"
#include "Detector/PVController/AMDSPVController.h"
#include "Detector/PVController/AMDSPVControllerServer.h"

#include "util/AMDSRunTimeSupport.h"

AMDSCentralServerSGMPV::AMDSCentralServerSGMPV(const QString &configurationFile, QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 20 * 60 * 1000; // 20 minuntes

	initializeConfiguration(configurationFile);
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
	AMDSThreadedBufferGroupManager * bufferGroup = bufferGroupManagers_.value(bufferName);
	if (bufferGroup) {
		bufferGroup->append(newData);
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SERVER_ALT_INVALID_BUFFERGROUP_NAME, QString("Failed to find bufferGroup for %1").arg(bufferName));
	}
}

void AMDSCentralServerSGMPV::initializeConfiguration(const QString &configurationFileName)
{
	// initialize the configurations of the PVs
	QRegExp rx("(\\,|\\t)"); //RegEx for ',' or '\t'
	int totalParameters = 6;
//	int dataTypeParamIndex = totalParameters - 1;

	QFile configurationFile(configurationFileName);

	if (configurationFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QTextStream dataStream(&configurationFile);

		QString pvConfiguration = dataStream.readLine(); // skip the first line, which is the title line
		while (!pvConfiguration.isNull())
		{
			pvConfiguration = dataStream.readLine();
			if (!pvConfiguration.isNull()) {
				QStringList pvDefintions = pvConfiguration.split(rx);
				if (pvDefintions.size() == totalParameters && pvDefintions.at(3).trimmed().length() > 0 ) {
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
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, 0, QString("AMDS PV: Failed to opent he configuration file (%1).").arg(configurationFileName));
	}
}

void AMDSCentralServerSGMPV::initializeBufferGroup()
{
	// initialize bufferGroup for PV
	foreach (AMDSPVConfigurationMap *pvConfiguration, pvConfigurationMaps_.values()) {
		QList<AMDSAxisInfo> pvBufferGroupAxes;
		pvBufferGroupAxes << AMDSAxisInfo(pvConfiguration->pvName(), 1, pvConfiguration->pvDescription(), pvConfiguration->pvUnits());
		AMDSBufferGroupInfo pvBufferGroupInfo(pvConfiguration->pvName(), pvConfiguration->pvDescription(), pvConfiguration->pvUnits(), pvConfiguration->dataType(), AMDSBufferGroupInfo::NoFlatten, pvBufferGroupAxes);

		AMDSThreadedBufferGroupManager *pvThreadedBufferGroup = new AMDSThreadedBufferGroupManager(pvBufferGroupInfo, maxBufferSize_);
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
	bool continueProcessDataRequest = false;
	AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest*>(clientRequest);
	if(clientDataRequest){
		if (clientDataRequest->isContinuousMessage()) {
			AMDSClientContinuousDataRequest *continuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest *>(clientRequest);
			QStringList bufferNames = continuousDataRequest->bufferNames();
			foreach (QString bufferName, bufferNames) {
				AMDSPVConfigurationMap *pvConfiguration = pvConfigurationMaps_.value(bufferName);
				if (!pvConfiguration || !pvConfiguration->enabled()) {
					QString errorMessage = QString("ERROR: %1 - Request data from a disabled bufferGroup (%2).").arg(AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME).arg(bufferName);
					AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME, errorMessage);

					bufferNames.removeOne(bufferName);
				}
			}
			if (bufferNames.length() > 0) {
				continueProcessDataRequest = true;
				continuousDataRequest->setBufferNames(bufferNames);
			}
		} else {
			QString bufferName = clientDataRequest->bufferName();
			AMDSPVConfigurationMap *pvConfiguration = pvConfigurationMaps_.value(bufferName);
			if (pvConfiguration && pvConfiguration->enabled()) {
				continueProcessDataRequest = true;
			} else {
				QString errorMessage = QString("ERROR: %1 - Request data from a disabled bufferGroup (%2).").arg(AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME).arg(bufferName);
				AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME, errorMessage);
			}
		}

	}

	// only when the PV is enabled, we will move forward
	if (continueProcessDataRequest) {
		AMDSCentralServer::processClientDataRequest(clientRequest);
	} else {
		QString errorMessage = QString("ERROR: %1 - Request data from a disabled bufferGroup or invalid data request type (%2.").arg(AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME).arg(clientRequest->requestType());
		clientRequest->setErrorMessage(errorMessage);
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_SGM_SERVER_ALT_DISBLED_BUFFERGROUP_NAME, errorMessage);
		emit clientRequestProcessed(clientRequest);
	}
}

