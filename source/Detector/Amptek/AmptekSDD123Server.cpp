#include "AmptekSDD123Server.h"

#include <QtNetwork>
#include <QCoreApplication>

#include "AmptekEventDefinitions.h"

/* Notes on how to convert back and forth
QString dataString = "4d4341433d3f3b";
QString plainString = "MCAC=?;";
qDebug() << "Normal " << dataString << plainString;
qDebug() << "In hex " << dataString << plainString.toAscii().toHex();
qDebug() << "In ASCII " << QString(QByteArray::fromHex(dataString.toAscii())) << plainString;
*/

AmptekSDD123Server::AmptekSDD123Server(AmptekSDD123ConfigurationMap *configurationMap, const QString &serverName, QObject *parent) :
	QObject(parent) , currentRequestPacket_(-1, QString("NONE")), currentSyncPacket_(-1, QString("NONE")), timedOutPacket_(-1, QString("NONE"))
{
	configurationMap_ = configurationMap;
	serverName_ = serverName;
	spectrumPacketReceiver_ = 0;
	requestIntervalTimer_ = -1;

	if(configurationMap_){
		stillReceivingResponseDatagram_ = false;
		responsePacketReady_ = false;
		socketLocallyBusy_ = false;
		detectorUnavailable_ = false;

		currentRequestTime_.start();
		requestPacketTimer_ = new QTimer(this);
		requestPacketTimer_->setSingleShot(true);
		syncPacketTimer_ = new QTimer(this);
		syncPacketTimer_->setSingleShot(true);

		packetIDCounter_ = 27;
		hasReplyReady_ = false;

		timeOutIDCounter_ = -27;

		requestSpectrumTime_ = new QTime();
		requestSpectrumTime_->start();
		lastRequestSpectrumTime_ = new QTime();
		lastRequestSpectrumTime_->start();
		replySpectrumTime_ = new QTime();
		replySpectrumTime_->start();

		udpDetectorSocket_ = new QUdpSocket(this);

		connect(udpDetectorSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
		connect(this, SIGNAL(responsePacketReady()), this, SLOT(onResponsePacketReady()));
	}
}

bool AmptekSDD123Server::detectorUnavailable() const{
	return detectorUnavailable_;
}

QList<double> AmptekSDD123Server::droppedPacketStatistics() const{
	QDateTime last30Seconds = QDateTime::currentDateTime().addSecs(-30);
	QList<double> retVal;
	retVal << droppedPackets_.totalOccurances() << droppedPackets_.totalRate() << droppedPackets_.occurancesSince(last30Seconds) << droppedPackets_.rateSince(last30Seconds);
	return retVal;
}

QList<double> AmptekSDD123Server::unrequestedPacketStatistics() const{
	QDateTime last30Seconds = QDateTime::currentDateTime().addSecs(-30);
	QList<double> retVal;
	retVal << unrequestedPackets_.totalOccurances() << unrequestedPackets_.totalRate()<< unrequestedPackets_.occurancesSince(last30Seconds) << unrequestedPackets_.rateSince(last30Seconds);
	return retVal;
}

bool AmptekSDD123Server::event(QEvent *e){
	if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationInitiateRequestEvent){
		if(((AmptekConfigurationInitiateRequestEvent*)e)->configurationMode_){
			emit serverAboutToChangeToConfigurationState();
			QTimer::singleShot(requestIntervalTimer_+5, this, SLOT(disableMCAMCS()));
		}
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationRequestEvent){
		if(((AmptekConfigurationRequestEvent*)e)->allParametersRequest_)
			requestAllTextConfigurationParameters();
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::DwellRequestEvent){
		if(((AmptekDwellRequestEvent*)e)->dwellMode_)
			requestDataPacket(AmptekCommandManagerSGM::RequestEnableMCAMCS);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationSetEvent){
		QStringList configurationCommands = ((AmptekConfigurationSetEvent*)e)->configurationCommands_;
		textConfigurationSet(configurationCommands.at(0));
		e->accept();
		return true;
	}
	return QObject::event(e);
}

int AmptekSDD123Server::forwardDatagram(const QByteArray &datagram){
	AmptekSDD123Packet forwardingPacket(packetIDCounter_++, datagram, true);
	if(forwardingPacket.isValid()){
		sendRequestDatagram(forwardingPacket);
		return forwardingPacket.packetID();
	}
	return -1;
}

void AmptekSDD123Server::requestDataPacket(AmptekCommandManagerSGM::AmptekCommandDefSGM command, const QString &dataStringHex, bool sendFakeRequest, bool sendSyncRequest, int overrideTimeout)
{
	AmptekSDD123Packet requestStatusPacket(packetIDCounter_, AmptekCommandManagerSGM::sddCommands()->amptekCommand(command).hex(), dataStringHex);
	packetIDCounter_++;

	if (sendFakeRequest) {
		fakeSendDatagram(requestStatusPacket, overrideTimeout);
	} else if (sendSyncRequest) {
		requestStatusPacket.appendRelatedSyncRequestID(timeOutIDCounter_);
		sendSyncDatagram(requestStatusPacket, overrideTimeout);
	} else {
		sendRequestDatagram(requestStatusPacket);
	}
}

void AmptekSDD123Server::disableMCAMCS(){
	requestDataPacket(AmptekCommandManagerSGM::RequestDisableMCAMCS);
}


void AmptekSDD123Server::mcaChannelsCount(){
	textConfigurationReadback("MCAC");
}

void AmptekSDD123Server::presetTime(){
	requestDataPacket(AmptekCommandManagerSGM::ReqeustStatusPacket);
}

void AmptekSDD123Server::requestAllTextConfigurationParameters(){
	QStringList manyCommands;
	manyCommands << "ALLP" << "CLCK" << "GAIA" << "GAIF" << "GAIN" << "HVSE" << "MCAC" << "MCAE" << "MCAS" << "PAPS" << "PDMD" << "PRER" << "PURE"
			<< "SCAI=1" << "SCAH" << "SCAL"
			<< "SCAI=2" << "SCAH" << "SCAL"
			<< "SCAI=3" << "SCAH" << "SCAL"
			<< "SCAI=4" << "SCAH" << "SCAL"
			<< "SCAI=5" << "SCAH" << "SCAL"
			<< "SCAI=6" << "SCAH" << "SCAL"
			<< "SCAI=7" << "SCAH" << "SCAL"
			<< "SCAI=8" << "SCAH" << "SCAL"
			<< "TECS" << "THFA" << "THSL" << "TPEA" << "TPFA";
	textConfigurationReadback(manyCommands);
}

void AmptekSDD123Server::textConfigurationReadback(QString singleCommandRequest){
	QString dataString = singleCommandRequest+"=?;";
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfigurationReadback, dataString.toAscii().toHex());
}

void AmptekSDD123Server::textConfigurationReadback(QStringList multiCommandRequest){
	QString singleCommand;
	QString fullCommandString;
	foreach(singleCommand, multiCommandRequest)
		fullCommandString.append(singleCommand+";");
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfigurationReadback, fullCommandString.toAscii().toHex());
}

void AmptekSDD123Server::textConfigurationSet(QString singleCommandRequest){
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfiguration, singleCommandRequest.toAscii().toHex());
}

bool AmptekSDD123Server::readReplyDatagram(int &id, QByteArray &datagram){
	if(hasReplyReady_){
		id = currentRequestPacket_.packetID();
		datagram = currentRequestPacket_.datagram();
		hasReplyReady_ = false;
		return true;
	}
	return false;
}

void AmptekSDD123Server::setRequestIntervalTimer(int mSec){
	requestIntervalTimer_ = mSec;
}

void AmptekSDD123Server::setSpectrumPacketReceiver(QObject *spectrumPacketReceiver){
	spectrumPacketReceiver_ = spectrumPacketReceiver;
}

void AmptekSDD123Server::setConfirmationPacketReceiver(QObject *confirmationPacketReceiver){
	confirmationPacketReceiver_ = confirmationPacketReceiver;
}

void AmptekSDD123Server::sendRequestDatagram(AmptekSDD123Packet packet, int overrideTimeout){
	if(socketLocallyBusy_){
		qDebug() << "\nPACKET QUEUE IS BUSY\n";
		packetQueue_.append(packet);
	}
	else{
		//qDebug() << "Sending request datagram";
		socketLocallyBusy_ = true;
		currentRequestPacket_ = packet;
		currentRequestTime_.restart();
		udpDetectorSocket_->writeDatagram(packet.datagram(), packet.datagram().size(), configurationMap_->detectorAddress(), 10001);
		connect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
		if(overrideTimeout == -1)
			requestPacketTimer_->start(currentRequestPacket_.timeout());
		else
			requestPacketTimer_->start(overrideTimeout);
	}
}

void AmptekSDD123Server::sendSyncDatagram(AmptekSDD123Packet packet, int overrideTimeout){
	if(socketLocallyBusy_){
		qDebug() << "PACKET QUEUE IS BUSY";
		packetQueue_.append(packet);
	}
	else{
		socketLocallyBusy_ = true;
		currentSyncPacket_ = packet;
		udpDetectorSocket_->writeDatagram(packet.datagram(), packet.datagram().size(), configurationMap_->detectorAddress(), 10001);
		connect(syncPacketTimer_, SIGNAL(timeout()), this, SLOT(syncDatagramTimeout()));
		if(overrideTimeout == -1)
			syncPacketTimer_->start(currentSyncPacket_.timeout());
		else
			syncPacketTimer_->start(overrideTimeout);
	}
}

void AmptekSDD123Server::fakeSendDatagram(AmptekSDD123Packet packet, int overrideTimeout){
	if(socketLocallyBusy_){
		qDebug() << "PACKET QUEUE IS BUSY";
		packetQueue_.append(packet);
	}
	else{
		socketLocallyBusy_ = true;
		currentRequestPacket_ = packet;
		currentRequestTime_.restart();
		//udpDetectorSocket_->writeDatagram(packet.datagram(), packet.datagram().size(), configurationMap_->detectorAddress(), 10001);
		connect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
		if(overrideTimeout == -1)
			requestPacketTimer_->start(currentRequestPacket_.timeout());
		else
			requestPacketTimer_->start(overrideTimeout);
	}
}

void AmptekSDD123Server::processPendingDatagrams(){
	responsePacketReady_ = false;
	while(udpDetectorSocket_->hasPendingDatagrams()){
		QByteArray datagram;
		bool ok;
		datagram.resize(udpDetectorSocket_->pendingDatagramSize());
		QHostAddress *sourceAddress = new QHostAddress();
		udpDetectorSocket_->readDatagram(datagram.data(), datagram.size(), sourceAddress);

		if(datagram.size() < 520 && datagram.mid(0,2) == QByteArray::fromHex("f5fa")){
			totalResponseDatagramSize_ = datagram.size();
			totalResponseDatagram_.clear();
			totalResponseDatagram_.append(datagram);
			responsePacketReady_ = true;
			stillReceivingResponseDatagram_ = false;
		}
		else if(stillReceivingResponseDatagram_){
			totalResponseDatagram_.append(datagram);
			if(totalResponseDatagram_.size() == totalResponseDatagramSize_){
				stillReceivingResponseDatagram_ = false;
				responsePacketReady_ = true;
			}
		}
		else if(datagram.size() == 520){
			if(datagram.mid(0,2) == QByteArray::fromHex("f5fa")){
				stillReceivingResponseDatagram_ = true;
				totalResponseDatagramSize_ = datagram.mid(4,2).toHex().toInt(&ok, 16) + 8;//+8 for header, command, length, and checksum
				totalResponseDatagram_.clear();
				totalResponseDatagram_.append(datagram);
			}
		}
		delete sourceAddress;
	}
	if(responsePacketReady_)
		emit responsePacketReady();
}

void AmptekSDD123Server::onResponsePacketReady(){
	//qDebug() << "In onResonsePacketReady";
	requestPacketTimer_->stop();
	syncPacketTimer_->stop();
	AmptekSDD123Packet lastRequestPacket = currentRequestPacket_;
	AmptekSDD123Packet responsePacket(currentRequestPacket_.packetID(), totalResponseDatagram_);

	bool foundTimedOutPacket = false;
	bool rerequestRequired = false;
	if(timedOutPackets_.count() > 0){
		for(int x = 0; x < timedOutPackets_.count(); x++){
			qDebug() << timedOutPackets_.at(x).commandText() << QString(QByteArray::fromHex(timedOutPackets_.at(x).dataString().toAscii()));
		}

		AmptekSDD123Packet headPacket(-1, QString("NONE"));
		while(!foundTimedOutPacket && timedOutPackets_.count() > 0){
			headPacket = timedOutPackets_.dequeue();
			if(responsePacket.commandText() == "Comm test - Echo packet Response" && headPacket.commandText() == "Comm test - Echo packet" && responsePacket.dataString() == headPacket.dataString())
				foundTimedOutPacket = true;
			else if(responsePacket.commandText() != "Comm test - Echo packet Response" && headPacket.possibleResponses().contains(responsePacket.commandString())){
				foundTimedOutPacket = true;
				AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
				currentRequestPacket_ = emptyPacket;
			}
			else{
				if(headPacket.commandText() == currentRequestPacket_.commandText() && responsePacket.dataString() == currentSyncPacket_.dataString())
					rerequestRequired = true;
				droppedPackets_.appendPacket(QDateTime::currentDateTime(), headPacket);
				emit dropPacketDetected();
			}
		}
	}

	AmptekSDD123Packet localRequestPacket(currentRequestPacket_.packetID(), currentRequestPacket_);

	if(!foundTimedOutPacket && currentRequestPacket_.possibleResponses().contains(responsePacket.commandString())){
		AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
		currentRequestPacket_ = emptyPacket;
	}
	else if(!foundTimedOutPacket && currentSyncPacket_.dataString() == responsePacket.dataString()){
		AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
		currentSyncPacket_ = emptyPacket;
	}
	else if(!foundTimedOutPacket){
		unrequestedPackets_.appendPacket(QDateTime::currentDateTime(), responsePacket);
		emit unrequestedPacketDetected();
	}

	socketLocallyBusy_ = false;

	if(rerequestRequired){
		if(localRequestPacket.currentRetries() < localRequestPacket.maxRetries()){
			localRequestPacket.incrementRetries(packetIDCounter_++);
			sendRequestDatagram(localRequestPacket);
		}
		else
			emit fatalTimeout();
	}


	if(localRequestPacket.isForwarded()){
		hasReplyReady_ = true;
		emit replyDatagram(responsePacket.packetID(), responsePacket.datagram());
	}
	if(responsePacket.commandText() == "Request Status Packet Response"){
		emit statusDataReady(responsePacket.datagram().mid(6, 64));
	}
	if(responsePacket.commandText() == "256-channel spectrum plus Status"){
		replySpectrumTime_->restart();
		//qDebug() << "Heard a 256-channel response";
		emit spectrumDataReady(responsePacket.datagram().mid(6, 768), 256);
		emit statusDataReady(responsePacket.datagram().mid(774, 64));
		if(spectrumPacketReceiver_){
			AmptekSpectrumPacketEvent *spectrumPacketEvent = new AmptekSpectrumPacketEvent();
			spectrumPacketEvent->spectrumByteArray_ = responsePacket.datagram().mid(6, 768);
			spectrumPacketEvent->channelCount_ = 256;
			spectrumPacketEvent->statusDataArray_ = responsePacket.datagram().mid(774, 64);
			spectrumPacketEvent->dwellStartTime_.setHMS(lastRequestSpectrumTime_->hour(), lastRequestSpectrumTime_->minute(), lastRequestSpectrumTime_->second(), lastRequestSpectrumTime_->msec());
			spectrumPacketEvent->dwellEndTime_.setHMS(requestSpectrumTime_->hour(), requestSpectrumTime_->minute(), requestSpectrumTime_->second(), requestSpectrumTime_->msec());
			spectrumPacketEvent->dwellReplyTime_.setHMS(replySpectrumTime_->hour(), replySpectrumTime_->minute(), replySpectrumTime_->second(), replySpectrumTime_->msec());
			QCoreApplication::postEvent(spectrumPacketReceiver_, spectrumPacketEvent);
		}
	}
	if(responsePacket.commandText() == "512-channel spectrum plus Status"){
		replySpectrumTime_->restart();
		//qDebug() << "Heard a 512-channel response";
		emit spectrumDataReady(responsePacket.datagram().mid(6, 1536), 512);
		emit statusDataReady(responsePacket.datagram().mid(1542, 64));
		if(spectrumPacketReceiver_){
			AmptekSpectrumPacketEvent *spectrumPacketEvent = new AmptekSpectrumPacketEvent();
			spectrumPacketEvent->spectrumByteArray_ = responsePacket.datagram().mid(6, 1536);
			spectrumPacketEvent->channelCount_ = 512;
			spectrumPacketEvent->statusDataArray_ = responsePacket.datagram().mid(1542, 64);
			spectrumPacketEvent->dwellStartTime_.setHMS(lastRequestSpectrumTime_->hour(), lastRequestSpectrumTime_->minute(), lastRequestSpectrumTime_->second(), lastRequestSpectrumTime_->msec());
			spectrumPacketEvent->dwellEndTime_.setHMS(requestSpectrumTime_->hour(), requestSpectrumTime_->minute(), requestSpectrumTime_->second(), requestSpectrumTime_->msec());
			spectrumPacketEvent->dwellReplyTime_.setHMS(replySpectrumTime_->hour(), replySpectrumTime_->minute(), replySpectrumTime_->second(), replySpectrumTime_->msec());
			QCoreApplication::postEvent(spectrumPacketReceiver_, spectrumPacketEvent);
		}
	}
	if(responsePacket.commandText() == "1024-channel spectrum plus Status"){
		replySpectrumTime_->restart();
		emit spectrumDataReady(responsePacket.datagram().mid(6, 3072), 1024);
		emit statusDataReady(responsePacket.datagram().mid(3078, 64));
		if(spectrumPacketReceiver_){
			AmptekSpectrumPacketEvent *spectrumPacketEvent = new AmptekSpectrumPacketEvent();
			spectrumPacketEvent->spectrumByteArray_ = responsePacket.datagram().mid(6, 3072);
			spectrumPacketEvent->channelCount_ = 1024;
			spectrumPacketEvent->statusDataArray_ = responsePacket.datagram().mid(3078, 64);
			spectrumPacketEvent->dwellStartTime_.setHMS(lastRequestSpectrumTime_->hour(), lastRequestSpectrumTime_->minute(), lastRequestSpectrumTime_->second(), lastRequestSpectrumTime_->msec());
			spectrumPacketEvent->dwellEndTime_.setHMS(requestSpectrumTime_->hour(), requestSpectrumTime_->minute(), requestSpectrumTime_->second(), requestSpectrumTime_->msec());
			spectrumPacketEvent->dwellReplyTime_.setHMS(replySpectrumTime_->hour(), replySpectrumTime_->minute(), replySpectrumTime_->second(), replySpectrumTime_->msec());
			QCoreApplication::postEvent(spectrumPacketReceiver_, spectrumPacketEvent);
		}
	}
	if(lastRequestPacket.commandText() == "Text Configuration"){
		requestAllTextConfigurationParameters();
	}
	if(responsePacket.commandText() == "Configuration Readback"){
		QString allASCIICommands = QString(QByteArray::fromHex(responsePacket.dataString().toAscii()));
		if(allASCIICommands.contains("ALLP")){
			emit allParametersTextConfiguration(allASCIICommands);

			if(spectrumPacketReceiver_){
				AmptekConfigurationReadbackEvent *configurationReadbackEvent = new AmptekConfigurationReadbackEvent();
				configurationReadbackEvent->configurationReadback_ = allASCIICommands;
				QCoreApplication::postEvent(spectrumPacketReceiver_, configurationReadbackEvent);
			}
		}
	}
	if(lastRequestPacket.commandText() == "Enable MCA/MCS"){
		emit serverChangedToDwellState();
	}
	if(lastRequestPacket.commandText() == "Disable MCA/MCS"){
		emit serverChangedToConfigurationState();

		if(confirmationPacketReceiver_){
			AmptekConfigurationModeConfirmationEvent *configurationConfirmationEvent = new AmptekConfigurationModeConfirmationEvent();
			configurationConfirmationEvent->confirmConfigurationMode_ = true;
			QCoreApplication::postEvent(confirmationPacketReceiver_, configurationConfirmationEvent);
		}
	}
	if(responsePacket.commandText() == "OK"){
		emit acknowledgeOK();
	}

	if(!packetQueue_.isEmpty()){
		AmptekSDD123Packet resendPacket(packetQueue_.takeFirst());
		if(resendPacket.commandText() == "Comm test - Echo packet")
			sendSyncDatagram(resendPacket);
		else
			sendRequestDatagram(resendPacket);
	}
}

void AmptekSDD123Server::syncRequest(int timeout){
	QString hashTag = QString("%1").arg(--timeOutIDCounter_);
	requestDataPacket(AmptekCommandManagerSGM::RequestCommTestEchoPacket, hashTag.toAscii().toHex(), false, true, timeout);
}

void AmptekSDD123Server::fakeSyncRequest(int timeout){
	QString hashTag = QString("%1").arg(timeOutIDCounter_--);
	requestDataPacket(AmptekCommandManagerSGM::RequestCommTestEchoPacket, hashTag.toAscii().toHex(), true, timeout);
}

void AmptekSDD123Server::requestDatagramTimeout(){
	socketLocallyBusy_ = false;
	disconnect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
	requestPacketTimer_->stop();
	if(stillReceivingResponseDatagram_){
		connect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
		requestPacketTimer_->start(currentRequestPacket_.timeout());
		return;
	}
	if(currentRequestPacket_.commandText() == "Comm test - Echo packet" && QString(QByteArray::fromHex(currentRequestPacket_.dataString().toAscii())) == "a7xi272727;idhai"){
		detectorUnavailable_ = true;
		emit detectorUnavailableChanged(detectorUnavailable_);
		return;
	}
	timedOutPackets_.append(currentRequestPacket_);
	syncRequest();
}

void AmptekSDD123Server::syncDatagramTimeout(){
	socketLocallyBusy_ = false;
	disconnect(syncPacketTimer_, SIGNAL(timeout()), this, SLOT(syncDatagramTimeout()));
	syncPacketTimer_->stop();
	if(stillReceivingResponseDatagram_){
		connect(syncPacketTimer_, SIGNAL(timeout()), this, SLOT(syncDatagramTimeout()));
		syncPacketTimer_->start(currentSyncPacket_.timeout());
		return;
	}
	if(currentRequestPacket_.relatedSyncRequestIDs().count() < 4){
		timedOutPackets_.append(currentSyncPacket_);
		syncRequest();
	}
	else{
		qDebug() << "Could not complete sync request" << " Been " << currentRequestTime_.elapsed() << " since original request";
	}
}
