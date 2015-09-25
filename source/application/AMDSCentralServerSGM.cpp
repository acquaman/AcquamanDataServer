#include "AMDSCentralServerSGM.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "DataElement/AMDSBufferGroup.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "ClientRequest/AMDSClientIntrospectionRequest.h"
#include "ClientRequest/AMDSClientDataRequest.h"
#include "ClientRequest/AMDSClientContinuousDataRequest.h"
#include "Detector/Amptek/AmptekSDD123Application.h"
#include "Detector/Amptek/AmptekSDD123ConfigurationMap.h"
//#include "Detector/Amptek/AmptekSDD123DetectorGroup.h"
#include "Detector/Amptek/AmptekSDD123ServerGroup.h"
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
		configurationMaps_.removeOne(configuration);
	}
	configurationMaps_.clear();


	if (amptekDataServerGroupThread_->isRunning())
		amptekDataServerGroupThread_->terminate();

	serverGroup_->deleteLater();
	amptekDataServerGroupThread_->deleteLater();

}

void AMDSCentralServerSGM::initializeBufferGroup(quint64 maxCountSize)
{
	//	detectorGroup_ = new AmptekSDD123DetectorGroup(configurationMaps);
	//	connect(detectorGroup_, SIGNAL(dataRequestReady(ClientDataRequest*)), tcpServer_.server(), SLOT(dataRequestReady(ClientDataRequest*)));
}

void AMDSCentralServerSGM::initializeAndStartDataServer()
{
//	serverGroup_ = new AmptekSDD123ServerGroup(detectorGroup_->detectorManagers(), configurationMaps);
	serverGroup_ = new AmptekSDD123ServerGroup(configurationMaps_);

	connectBufferGroupAndDataServer();

	amptekDataServerGroupThread_ = new QThread();
	serverGroup_->moveToThread(amptekDataServerGroupThread_);
	amptekDataServerGroupThread_->start();

	connect(serverGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(serverGroup_, SIGNAL(serverChangedToDwellState(int)), this, SLOT(onServerChangedToDwellState(int)));
}

void AMDSCentralServerSGM::connectBufferGroupAndDataServer()
{
	//	AmptekSDD123Server *oneServer;
	//	for(int x = 0, size = detectorManagers_.count(); x < size; x++){
	//		oneServer = new AmptekSDD123Server(configurationMaps.at(x), QString("%1 Server").arg(detectorManagers_.at(x)->detector()->name()), this);
	//		oneServer->setSpectrumPacketReceiver(detectorManagers_.at(x)->detector());
	//		oneServer->setConfirmationPacketReceiver(detectorManagers_.at(x));
	//		oneServer->setRequestIntervalTimer(masterRequestInterval_);
	//		servers_.append(oneServer);
	//		serversInDwellMode_.append(oneServer);

	//		connect(oneServer, SIGNAL(serverAboutToChangeToConfigurationState()), serverAboutToConfigurationStateMapper_, SLOT(map()));
	//		serverAboutToConfigurationStateMapper_->setMapping(oneServer, x);

	//		connect(oneServer, SIGNAL(serverChangedToConfigurationState()), serverConfigurationStateMapper_, SLOT(map()));
	//		serverConfigurationStateMapper_->setMapping(oneServer, x);

	//		connect(oneServer, SIGNAL(serverChangedToDwellState()), serverDwellStateMapper_, SLOT(map()));
	//		serverDwellStateMapper_->setMapping(oneServer, x);
	//	}

	//	for(int x = 0, size = detectorManagers().count(); x < size; x++)
	//		detectorManagers().at(x)->setRequestEventReceiver((QObject*)(servers().at(x)));

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


//#include "source/DataHolder/AMDSScalarDataHolder.h"
//void AMDSCentralServerSGM::onFiftyMillisecondTimerUpdate(){
//	AMDSLightWeightScalarDataHolder *oneScalerDataHolder = new AMDSLightWeightScalarDataHolder();
//	double valueAsDouble = (double)simpleCounter_++;
//	oneScalerDataHolder->setSingleValue(valueAsDouble);

//	energyBufferGroup_->append(oneScalerDataHolder);
//}

//#include "source/DataHolder/AMDSSpectralDataHolder.h"
//void AMDSCentralServerSGM::onHundredMillisecondTimerUpdate(){
//	AMDSLightWeightSpectralDataHolder *oneSpectralDataHolder = new AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::Signed64, amptek1BufferGroup_->bufferGroupInfo().size(0));

//	AMDSFlatArray oneSpectralFlatArray(AMDSDataTypeDefinitions::Signed64, amptek1BufferGroup_->bufferGroupInfo().size(0));
//	spectralCounter_++;
//	qint64 oneValue;
//	for(int x = 0; x < oneSpectralFlatArray.constVectorQint64().size(); x++){
//		oneValue = (spectralCounter_+x)*2;
//		oneSpectralFlatArray.vectorQint64()[x] = oneValue;
//	}
//	oneSpectralDataHolder->setData(&oneSpectralFlatArray);

//	amptek1BufferGroup_->append(oneSpectralDataHolder);
//}

//void AMDSCentralServerSGM::initializeBufferGroup(quint64 maxCountSize)
//{
//	QList<AMDSAxisInfo> mcpBufferGroupAxes;
//	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
//	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
//	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", AMDSBufferGroupInfo::NoFlatten, mcpBufferGroupAxes);
//	AMDSBufferGroup *mcpBufferGroup = new AMDSBufferGroup(mcpBufferGroupInfo, maxCountSize);
//	AMDSThreadedBufferGroup *mcpThreadedBufferGroup = new AMDSThreadedBufferGroup(mcpBufferGroup);
//	bufferGroups_.insert(mcpThreadedBufferGroup->bufferGroupInfo().name(), mcpThreadedBufferGroup);

//	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
//	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
//	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", AMDSBufferGroupInfo::NoFlatten, amptek1BufferGroupAxes);
//	amptek1BufferGroup_ = new AMDSBufferGroup(amptek1BufferGroupInfo, maxCountSize);
//	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroup_);
//	bufferGroups_.insert(amptek1ThreadedBufferGroup->bufferGroupInfo().name(), amptek1ThreadedBufferGroup);

//	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV", AMDSBufferGroupInfo::Average);
//	energyBufferGroup_ = new AMDSBufferGroup(energyBufferGroupInfo, maxCountSize);
//	AMDSThreadedBufferGroup *energyThreadedBufferGroup = new AMDSThreadedBufferGroup(energyBufferGroup_);
//	bufferGroups_.insert(energyThreadedBufferGroup->bufferGroupInfo().name(), energyThreadedBufferGroup);

//	connect(mcpBufferGroup, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
//	connect(amptek1BufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
//	connect(energyBufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

//}

//virtual void AMDSCentralServerSGM::initializeAndStartDataServer()
//{
//	fiftyMillisecondTimer_ = new QTimer(this);
//	hundredMillisecondTimer_ = new QTimer(this);
//	connect(fiftyMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onFiftyMillisecondTimerUpdate()));
//	connect(hundredMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondTimerUpdate()));

//	simpleCounter_ = 0;
//	spectralCounter_ = 0;

//	fiftyMillisecondTimer_->start(50);
//	hundredMillisecondTimer_->start(100);
//}
