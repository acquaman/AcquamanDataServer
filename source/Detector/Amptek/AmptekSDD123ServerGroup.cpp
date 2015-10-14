#include "AmptekSDD123ServerGroup.h"

#include <QSignalMapper>

#include "AmptekSDD123Server.h"

AmptekSDD123ThreadedDataServerGroup::AmptekSDD123ThreadedDataServerGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent)
	:QObject(parent)
{
	amptekServerGroup_ = new AmptekSDD123ServerGroup(configurationMaps);

	amptekServerGroupThread_ = new QThread();
	amptekServerGroup_->moveToThread(amptekServerGroupThread_);

	connect(amptekServerGroup_, SIGNAL(serverChangedToConfigurationState(int)), this, SIGNAL(serverChangedToConfigurationState(int)));
	connect(amptekServerGroup_, SIGNAL(serverChangedToDwellState(int)), this, SIGNAL(serverChangedToDwellState(int)));

	amptekServerGroupThread_->start();
}

AmptekSDD123ThreadedDataServerGroup::~AmptekSDD123ThreadedDataServerGroup()
{
	if (amptekServerGroupThread_->isRunning()) {
		amptekServerGroupThread_->quit();
	}

	amptekServerGroup_->deleteLater();
	amptekServerGroupThread_->deleteLater();
}

AmptekSDD123Server*  AmptekSDD123ThreadedDataServerGroup::serverAt(int index)
{
	return amptekServerGroup_->serverAt(index);
}


/// =================================== implementation of AmptekSDD123ServerGroup =======================================
AmptekSDD123ServerGroup::AmptekSDD123ServerGroup(QList<AmptekSDD123ConfigurationMap *> configurationMaps, QObject *parent) :
	QObject(parent)
{
	requestSpectraTimer_ = 0;
	masterRequestInterval_ = 50;
//	serverState_ = AmptekSDD123ServerGroup::DwellState;

	serverAboutToConfigurationStateMapper_ = new QSignalMapper();
	serverConfigurationStateMapper_ = new QSignalMapper();
	serverDwellStateMapper_ = new QSignalMapper();

	AmptekSDD123Server *amptekServer;
	for(int index = 0, size = configurationMaps.count(); index < size; index++){
		AmptekSDD123ConfigurationMap *amptekConfiguration = configurationMaps.at(index);

		amptekServer = new AmptekSDD123Server(amptekConfiguration, QString("%1 Server").arg(amptekConfiguration->detectorName()), this);
		amptekServer->setRequestIntervalTimer(masterRequestInterval_);

		servers_.append(amptekServer);
		serversInDwellMode_.append(amptekServer);

		serverAboutToConfigurationStateMapper_->setMapping(amptekServer, index);
		serverConfigurationStateMapper_->setMapping(amptekServer, index);
		serverDwellStateMapper_->setMapping(amptekServer, index);

		connect(amptekServer, SIGNAL(serverAboutToChangeToConfigurationState()), serverAboutToConfigurationStateMapper_, SLOT(map()));
		connect(amptekServer, SIGNAL(serverChangedToConfigurationState()), serverConfigurationStateMapper_, SLOT(map()));
		connect(amptekServer, SIGNAL(serverChangedToDwellState()), serverDwellStateMapper_, SLOT(map()));
	}

	connect(serverAboutToConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerAboutToChangeToConfigurationState(int)));
	connect(serverConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(serverDwellStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToDwellState(int)));

	QTimer::singleShot(1000, this, SLOT(initiateAllRequestSpectrum()));
}

AmptekSDD123ServerGroup::~AmptekSDD123ServerGroup()
{
	releaseRequestSpectrualTimer();

	foreach (AmptekSDD123Server *amptekServer, servers_) {
		disconnect(amptekServer, SIGNAL(serverAboutToChangeToConfigurationState()), serverAboutToConfigurationStateMapper_, SLOT(map()));
		disconnect(amptekServer, SIGNAL(serverChangedToConfigurationState()), serverConfigurationStateMapper_, SLOT(map()));
		disconnect(amptekServer, SIGNAL(serverChangedToDwellState()), serverDwellStateMapper_, SLOT(map()));

		amptekServer->deleteLater();
	}
	serversInDwellMode_.clear();
	servers_.clear();


	disconnect(serverAboutToConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerAboutToChangeToConfigurationState(int)));
	disconnect(serverConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	disconnect(serverDwellStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToDwellState(int)));

	serverAboutToConfigurationStateMapper_->deleteLater();
	serverConfigurationStateMapper_->deleteLater();
	serverDwellStateMapper_->deleteLater();
}

AmptekSDD123Server* AmptekSDD123ServerGroup::serverAt(int index){
	return servers_.at(index);
}

void AmptekSDD123ServerGroup::setServerState(int index, AmptekSDD123ServerGroup::ServerState serverState){
	AmptekSDD123Server *amptekServer = serverAt(index);

	if(serverState == AmptekSDD123ServerGroup::ConfigurationState){
		if(serversInDwellMode_.contains(amptekServer))
			serversInDwellMode_.removeAll(amptekServer);

		QTimer::singleShot(masterRequestInterval_+5, amptekServer, SLOT(disableMCAMCS()));
	}
	else if(serverState == AmptekSDD123ServerGroup::DwellState)
		amptekServer->requestDataPacket(AmptekCommandManagerSGM::RequestEnableMCAMCS);
}

void AmptekSDD123ServerGroup::setConfigurationValueFrom(const QString &configurationValueCommand, int index){
	serverAt(index)->requestDataPacket(AmptekCommandManagerSGM::RequestTextConfiguration, configurationValueCommand.toAscii().toHex());
}

void AmptekSDD123ServerGroup::onRequestAllSpectra(){
	foreach(AmptekSDD123Server *dwellModeServer, serversInDwellMode_)
		dwellModeServer->requestDataPacket(AmptekCommandManagerSGM::RequestAndClearSpecturmPlusStatus);
}

void AmptekSDD123ServerGroup::onServerAboutToChangeToConfigurationState(int index){
	AmptekSDD123Server *amptekServer = serverAt(index);
	if(serversInDwellMode_.contains(amptekServer))
		serversInDwellMode_.removeAll(amptekServer);
}

void AmptekSDD123ServerGroup::onServerChangedToConfigurationState(int index){
	emit serverChangedToConfigurationState(index);
}

void AmptekSDD123ServerGroup::onServerChangedToDwellState(int index){

	AmptekSDD123Server *amptekServer = serverAt(index);
	if(!serversInDwellMode_.contains(amptekServer))
		serversInDwellMode_.append(amptekServer);

	emit serverChangedToDwellState(index);
}

void AmptekSDD123ServerGroup::initiateAllRequestSpectrum(){
	releaseRequestSpectrualTimer();

	requestSpectraTimer_ = new QTimer(this);
	connect(requestSpectraTimer_, SIGNAL(timeout()), this, SLOT(onRequestAllSpectra()));

	requestSpectraTimer_->setInterval(masterRequestInterval_);
	requestSpectraTimer_->start();
}

void AmptekSDD123ServerGroup::releaseRequestSpectrualTimer()
{
	if (requestSpectraTimer_) {
		if (requestSpectraTimer_->isActive())
			requestSpectraTimer_->stop();

		disconnect(requestSpectraTimer_, SIGNAL(timeout()), this, SLOT(onRequestAllSpectra()));
		requestSpectraTimer_->deleteLater();
		requestSpectraTimer_ = 0;
	}
}

