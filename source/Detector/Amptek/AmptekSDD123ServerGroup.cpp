#include "AmptekSDD123ServerGroup.h"

#include <QSignalMapper>

#include "AmptekSDD123Application.h"
#include "AmptekSDD123Server.h"

AmptekSDD123ServerGroup::AmptekSDD123ServerGroup(QList<AmptekSDD123ConfigurationMap *> configurationMaps, QObject *parent) :
	QObject(parent)
{
	requestSpectraTimer_ = 0;
	masterRequestInterval_ = 50;
//	serverState_ = AmptekSDD123ServerGroup::DwellState;

	serverAboutToConfigurationStateMapper_ = new QSignalMapper();
	serverConfigurationStateMapper_ = new QSignalMapper();
	serverDwellStateMapper_ = new QSignalMapper();

	AmptekSDD123Server *oneServer;
	for(int x = 0, size = configurationMaps.count(); x < size; x++){
		oneServer = new AmptekSDD123Server(configurationMaps.at(x), QString("%1 Server").arg(configurationMaps.at(x)->detectorName()), this);
		oneServer->setRequestIntervalTimer(masterRequestInterval_);
		servers_.append(oneServer);
		serversInDwellMode_.append(oneServer);

		serverAboutToConfigurationStateMapper_->setMapping(oneServer, x);
		serverConfigurationStateMapper_->setMapping(oneServer, x);
		serverDwellStateMapper_->setMapping(oneServer, x);

		connect(oneServer, SIGNAL(serverAboutToChangeToConfigurationState()), serverAboutToConfigurationStateMapper_, SLOT(map()));
		connect(oneServer, SIGNAL(serverChangedToConfigurationState()), serverConfigurationStateMapper_, SLOT(map()));
		connect(oneServer, SIGNAL(serverChangedToDwellState()), serverDwellStateMapper_, SLOT(map()));
	}

	connect(serverAboutToConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerAboutToChangeToConfigurationState(int)));
	connect(serverConfigurationStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToConfigurationState(int)));
	connect(serverDwellStateMapper_, SIGNAL(mapped(int)), this, SLOT(onServerChangedToDwellState(int)));

	QTimer::singleShot(1000, this, SLOT(initiateAllRequestSpectrum()));
}

AmptekSDD123ServerGroup::~AmptekSDD123ServerGroup()
{
	if (requestSpectraTimer_) {
		if (requestSpectraTimer_->isActive())
			requestSpectraTimer_->stop();

		disconnect(requestSpectraTimer_, SIGNAL(timeout()), this, SLOT(requestAllSpectra()));
		requestSpectraTimer_->deleteLater();
		requestSpectraTimer_ = 0;
	}

	foreach (AmptekSDD123Server *amptekServer, servers_) {
		disconnect(amptekServer, SIGNAL(serverAboutToChangeToConfigurationState()), serverAboutToConfigurationStateMapper_, SLOT(map()));
		disconnect(amptekServer, SIGNAL(serverChangedToConfigurationState()), serverConfigurationStateMapper_, SLOT(map()));
		disconnect(amptekServer, SIGNAL(serverChangedToDwellState()), serverDwellStateMapper_, SLOT(map()));

		amptekServer->deleteLater();
		servers_.removeOne(amptekServer);
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

QList<AmptekSDD123Server*> AmptekSDD123ServerGroup::servers(){
	return servers_;
}

//void AmptekSDD123ServerGroup::setServerState(int index, AmptekSDD123ServerGroup::ServerState serverState){
//	if(serverState == AmptekSDD123ServerGroup::ConfigurationState){
//		if(serversInDwellMode_.contains(servers_.at(index)))
//			serversInDwellMode_.removeAll(servers_.at(index));

//		QTimer::singleShot(masterRequestInterval_+5, servers_.at(index), SLOT(disableMCAMCS()));
//	}
//	else if(serverState == AmptekSDD123ServerGroup::DwellState)
//		servers_.at(index)->requestDataPacket(AmptekCommandManagerSGM::RequestEnableMCAMCS);
//}

void AmptekSDD123ServerGroup::setConfigurationValueFrom(const QString &configurationValueCommand, int index){
	servers_.at(index)->requestDataPacket(AmptekCommandManagerSGM::RequestTextConfiguration, configurationValueCommand.toAscii().toHex());
}

void AmptekSDD123ServerGroup::initiateAllRequestSpectrum(){
	if (requestSpectraTimer_) {
		if (requestSpectraTimer_->isActive())
			requestSpectraTimer_->stop();

		disconnect(requestSpectraTimer_, SIGNAL(timeout()), this, SLOT(requestAllSpectra()));
		requestSpectraTimer_->deleteLater();
		requestSpectraTimer_ = 0;
	}

	requestSpectraTimer_ = new QTimer(this);
	connect(requestSpectraTimer_, SIGNAL(timeout()), this, SLOT(requestAllSpectra()));

	requestSpectraTimer_->setInterval(masterRequestInterval_);
	requestSpectraTimer_->start();
}

void AmptekSDD123ServerGroup::requestAllSpectra(){
	for(int x = 0, size = serversInDwellMode_.count(); x < size; x++)
		serversInDwellMode_.at(x)->requestDataPacket(AmptekCommandManagerSGM::RequestAndClearSpecturmPlusStatus);
}

void AmptekSDD123ServerGroup::onServerAboutToChangeToConfigurationState(int index){
	if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
		qDebug() << "Heard server at " << index << " is about to change to configuration mode";
	if(serversInDwellMode_.contains(servers_.at(index)))
		serversInDwellMode_.removeAll(servers_.at(index));
}

void AmptekSDD123ServerGroup::onServerChangedToConfigurationState(int index){
	if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
		qDebug() << "Heard server at " << index << " changed to configuration state";
	emit serverChangedToConfigurationState(index);
}

void AmptekSDD123ServerGroup::onServerChangedToDwellState(int index){
	if(AmptekSDD123Application::amptekApp()->debuggingEnabled())
		qDebug() << "Heard server at " << index << " changed to dwell state";
	if(!serversInDwellMode_.contains(servers_.at(index)))
		serversInDwellMode_.append(servers_.at(index));
	emit serverChangedToDwellState(index);
}
