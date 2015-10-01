#include "AMDSCentralServerSGM.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "Detector/Amptek/AmptekSDD123Application.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
#include "Detector/Amptek/AmptekSDD123DetectorGroup.h"
#include "Detector/Amptek/AmptekSDD123DetectorManager.h"
#include "Detector/Amptek/AmptekSDD123ServerGroup.h"
#include "Detector/Amptek/AmptekSDD123Server.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServerSGM::AMDSCentralServerSGM(QObject *parent) :
	AMDSCentralServer(parent)
{
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:iain:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.100"), 12004, QHostAddress("192.168.10.104"), this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:iain:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.101"), 12004, QHostAddress("192.168.10.104"), this));

	/* Commented out PV and IP details for actual beamline ampteks, so can test on some detectors without interfering with beamline operations
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 239", "amptek:sdd1", QHostAddress("192.168.0.239"), QHostAddress("192.168.0.139"), 12004, QHostAddress("192.168.10.104"), this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 240", "amptek:sdd2", QHostAddress("192.168.0.240"), QHostAddress("192.168.0.140"), 12004, QHostAddress("192.168.10.104"), this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 241", "amptek:sdd3", QHostAddress("192.168.0.241"), QHostAddress("192.168.0.141"), 12004, QHostAddress("192.168.10.104"), this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 242", "amptek:sdd4", QHostAddress("192.168.0.242"), QHostAddress("192.168.0.142"), 12004, QHostAddress("192.168.10.104"), this));
	configurationMaps_.append(new AmptekSDD123ConfigurationMap("Amptek SDD 243", "amptek:sdd5", QHostAddress("192.168.0.243"), QHostAddress("192.168.0.143"), 12004, QHostAddress("192.168.10.104"), this));
	*/
}

AMDSCentralServerSGM::~AMDSCentralServerSGM()
{
	foreach (AmptekSDD123ConfigurationMap *configuration, configurationMaps_) {
		configuration->deleteLater();
	}
	configurationMaps_.clear();


	amptekThreadedDataServerGroup_->deleteLater();
	amptekThreadedDataServerGroup_ = 0;
}

void AMDSCentralServerSGM::initializeBufferGroup(quint64 maxCountSize)
{
	detectorGroup_ = new AmptekSDD123DetectorGroup(configurationMaps_, maxCountSize);
	connect(detectorGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_, SLOT(onClientRequestProcessed(AMDSClientRequest*)));
}

void AMDSCentralServerSGM::initializeAndStartDataServer()
{
	amptekThreadedDataServerGroup_ = new AmptekSDD123ThreadedServerGroup(configurationMaps_);

	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(amptekThreadedDataServerGroup_, SIGNAL(serverChangedToDwellState(int)), this, SLOT(onServerChangedToDwellState(int)));
}

void AMDSCentralServerSGM::wrappingUpInitialization()
{
	// connect the event among the amptek data servers and the detectors
	for(int x = 0, size = detectorGroup_->detectorManagers().count(); x < size; x++){
		AmptekSDD123DetectorManager *amptekDetectorManager = detectorGroup_->detectorManagers().at(x);
		AmptekSDD123Server *amptekServer = amptekThreadedDataServerGroup_->servers().at(x);

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
