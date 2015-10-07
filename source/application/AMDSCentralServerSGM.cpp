#include "AMDSCentralServerSGM.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "Detector/Amptek/AmptekSDD123Application.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123DetectorManager.h"
#include "Detector/Amptek/AmptekSDD123ServerGroup.h"
#include "Detector/Amptek/AmptekSDD123Server.h"
#include "Detector/Amptek/SGM/AmptekSDD123DetectorGroupSGM.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServerSGM::AMDSCentralServerSGM(QObject *parent) :
	AMDSCentralServer(parent)
{
	maxBufferSize_ = 20 * 60 * 1000; // 20 minuntes

	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:iain:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.100"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:iain:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.101"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));

	/* Commented out PV and IP details for actual beamline ampteks, so can test on some detectors without interfering with beamline operations
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.139"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.140"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 241", "amptek:sdd3", QHostAddress("192.168.0.241"), QHostAddress("192.168.0.141"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 242", "amptek:sdd4", QHostAddress("192.168.0.242"), QHostAddress("192.168.0.142"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 243", "amptek:sdd5", QHostAddress("192.168.0.243"), QHostAddress("192.168.0.143"), 12004, QHostAddress("192.168.10.104"), AMDSDataTypeDefinitions::Double, 1024, this));
	*/
}

AMDSCentralServerSGM::~AMDSCentralServerSGM()
{
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, configurationMaps_) {
		amptekConfiguration->deleteLater();
	}
	configurationMaps_.clear();

	foreach (AMDSThreadedBufferGroup *bufferGroup, dwellBufferGroupManagers_) {
		bufferGroup->deleteLater();
	}
	dwellBufferGroupManagers_.clear();

	amptekThreadedDataServerGroup_->deleteLater();
	amptekThreadedDataServerGroup_ = 0;
}

void AMDSCentralServerSGM::initializeBufferGroup()
{
	foreach (AmptekSDD123ConfigurationMap *amptekConfiguration, configurationMaps_) {

		QList<AMDSAxisInfo> amptekBufferGroupAxes;
		amptekBufferGroupAxes << AMDSAxisInfo("Energy", amptekConfiguration->spectrumCountSize(), "Energy Axis", "eV");

		AMDSBufferGroupInfo amptekBufferGroupInfo(amptekConfiguration->detectorName(), amptekConfiguration->localAddress().toString(), "Counts", amptekConfiguration->dataType(), amptekConfiguration->spectrumCountSize(), AMDSBufferGroupInfo::NoFlatten, amptekBufferGroupAxes);
		AMDSThreadedBufferGroup *amptekThreadedBufferGroup = new AMDSThreadedBufferGroup(amptekBufferGroupInfo, maxBufferSize_);
		connect(amptekThreadedBufferGroup, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_, SLOT(onClientRequestProcessed(AMDSClientRequest*)));

		AMDSThreadedBufferGroup *amptekDwellThreadedBufferGroup = new AMDSThreadedBufferGroup(amptekBufferGroupInfo, maxBufferSize_);

		bufferGroupManagers_.insert(amptekThreadedBufferGroup->bufferGroupName(), amptekThreadedBufferGroup);
		dwellBufferGroupManagers_.insert(amptekDwellThreadedBufferGroup->bufferGroupName(), amptekDwellThreadedBufferGroup);
	}


	detectorGroup_ = new AmptekSDD123DetectorGroupSGM(configurationMaps_);
	connect(detectorGroup_, SIGNAL(clearHistrogramData(QString)), this, SLOT(onClearHistrogramData(QString)));
	connect(detectorGroup_, SIGNAL(clearDwellHistrogramData(QString)), this, SLOT(onClearDwellHistrogramData(QString)));
	connect(detectorGroup_, SIGNAL(newHistrogramReceived(QString, AMDSDataHolder*)), this, SLOT(onNewHistrogramReceived(QString, AMDSDataHolder*)));
	connect(detectorGroup_, SIGNAL(newDwellHistrogramReceived(QString, AMDSDataHolder*)), this, SLOT(onNewDwellHistrogramReceived(QString, AMDSDataHolder*)));
}

void AMDSCentralServerSGM::initializeAndStartDataServer()
{
	amptekThreadedDataServerGroup_ = new AmptekSDD123ThreadedDataServerGroup(configurationMaps_);

	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToDwellState(int)), this, SLOT(onServerChangedToDwellState(int)));
}

void AMDSCentralServerSGM::wrappingUpInitialization()
{
	// connect the event among the amptek data servers and the detectors
	for(int x = 0, size = detectorGroup_->detectorManagers().count(); x < size; x++){
		AmptekSDD123DetectorManager *amptekDetectorManager = detectorGroup_->detectorManagers().at(x);
		AmptekSDD123Server *amptekServer = amptekThreadedDataServerGroup_->serverAt(x);

		amptekServer->setSpectrumPacketReceiver((QObject *)amptekDetectorManager->detector());
		amptekServer->setConfirmationPacketReceiver(amptekDetectorManager);

		amptekDetectorManager->setRequestEventReceiver(amptekServer);
	}
}

void AMDSCentralServerSGM::onServerChangedToConfigurationState(int index){
	if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
		qDebug() << "Heard that the server " << index << " is now in the configuration state";

	emit serverChangedToConfigurationState(index);
}

void AMDSCentralServerSGM::onServerChangedToDwellState(int index){
	if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
		qDebug() << "Heard that the server " << index << " group is now in the dwell state";

	emit serverChangedToDwellState(index);
}

void AMDSCentralServerSGM::onClearHistrogramData(QString detectorName)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(detectorName);
	bufferGroup->clear();
}

void AMDSCentralServerSGM::onClearDwellHistrogramData(QString detectorName)
{
	AMDSThreadedBufferGroup * bufferGroup = dwellBufferGroupManagers_.value(detectorName);
	bufferGroup->clear();
}

void AMDSCentralServerSGM::onNewHistrogramReceived(QString detectorName, AMDSDataHolder *dataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = bufferGroupManagers_.value(detectorName);
	bufferGroup->append(dataHolder);
}

void AMDSCentralServerSGM::onNewDwellHistrogramReceived(QString detectorName, AMDSDataHolder *dataHolder)
{
	AMDSThreadedBufferGroup * bufferGroup = dwellBufferGroupManagers_.value(detectorName);
	bufferGroup->append(dataHolder);
}
