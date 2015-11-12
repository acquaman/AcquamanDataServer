#include "AmptekSDD123Server.h"

#include <QtNetwork>
#include <QCoreApplication>
#include <QDebug>

#include "AmptekEventDefinitions.h"
#include "util/AMErrorMonitor.h"

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
	spectrumPacketReceiver_ = 0;
	confirmationPacketReceiver_ = 0;

	configurationMap_ = configurationMap;
	serverName_ = serverName;
	requestIntervalTimer_ = -1;

	if(configurationMap_){
		stillReceivingResponseDatagram_ = false;
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
		bool boundUDP = udpDetectorSocket_->bind(configurationMap_->localAddress(), 10001, QUdpSocket::ShareAddress);
		if (!boundUDP)
			AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_FAILED_TO_BOUND_UDP, QString("Amptek Server failed to bound to the detector via %1:%2").arg(configurationMap_->localAddress().toString()).arg(10001));

		connect(udpDetectorSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
		connect(this, SIGNAL(responsePacketReady()), this, SLOT(onResponsePacketReady()));
	}
}

AmptekSDD123Server::~AmptekSDD123Server()
{
	spectrumPacketReceiver_ = 0;
	confirmationPacketReceiver_ = 0;

	udpDetectorSocket_->deleteLater();

	requestPacketTimer_->deleteLater();
	syncPacketTimer_->deleteLater();

	delete requestSpectrumTime_;
	delete lastRequestSpectrumTime_;
	delete replySpectrumTime_;
}

QList<double> AmptekSDD123Server::droppedPacketStatistics() const
{
	QDateTime last30Seconds = QDateTime::currentDateTimeUtc().addSecs(-30);
	QList<double> retVal;
	retVal << droppedPackets_.totalOccurances() << droppedPackets_.totalRate() << droppedPackets_.occurancesSince(last30Seconds) << droppedPackets_.rateSince(last30Seconds);
	return retVal;
}

QList<double> AmptekSDD123Server::unrequestedPacketStatistics() const{
	QDateTime last30Seconds = QDateTime::currentDateTimeUtc().addSecs(-30);
	QList<double> retVal;
	retVal << unrequestedPackets_.totalOccurances() << unrequestedPackets_.totalRate()<< unrequestedPackets_.occurancesSince(last30Seconds) << unrequestedPackets_.rateSince(last30Seconds);
	return retVal;
}

bool AmptekSDD123Server::event(QEvent *e)
{
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

int AmptekSDD123Server::forwardDatagram(const QByteArray &datagram)
{
	AmptekSDD123Packet forwardingPacket(packetIDCounter_, datagram, true);
	packetIDCounter_++;

	if(forwardingPacket.isValid()){
		sendRequestDatagram(forwardingPacket);
		return forwardingPacket.packetID();
	}
	return -1;
}

void AmptekSDD123Server::requestDataPacket(AmptekCommandManagerSGM::AmptekCommandDefSGM command, const QString &dataStringHex, bool sendFakeRequest, bool sendSyncRequest, int overrideTimeout)
{
	// Need to restart the requestSpectrumtime form RequestAndClearSpecturmPlusStatus (ID 23)
	if(command == AmptekCommandManagerSGM::RequestAndClearSpecturmPlusStatus) {
		lastRequestSpectrumTime_->setHMS(requestSpectrumTime_->hour(), requestSpectrumTime_->minute(), requestSpectrumTime_->second(), requestSpectrumTime_->msec());
		requestSpectrumTime_->restart();
	}

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

void AmptekSDD123Server::disableMCAMCS()
{
	requestDataPacket(AmptekCommandManagerSGM::RequestDisableMCAMCS);
}


void AmptekSDD123Server::mcaChannelsCount()
{
	textConfigurationReadback("MCAC");
}

void AmptekSDD123Server::presetTime()
{
	requestDataPacket(AmptekCommandManagerSGM::ReqeustStatusPacket);
}

void AmptekSDD123Server::requestAllTextConfigurationParameters()
{
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
			<< "TECS" << "THFA" << "THSL" << "TPEA" << "TPFA"
			<< "GPIN" << "GPMC" << "GPME" << "GPED";
	textConfigurationReadback(manyCommands);
}

void AmptekSDD123Server::textConfigurationReadback(const QString &singleCommandRequest)
{
	QString dataString = singleCommandRequest+"=?;";
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfigurationReadback, dataString.toAscii().toHex());
}

void AmptekSDD123Server::textConfigurationReadback(const QStringList &multiCommandRequest)
{
	QString singleCommand;
	QString fullCommandString;
	foreach(singleCommand, multiCommandRequest)
		fullCommandString.append(singleCommand+";");
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfigurationReadback, fullCommandString.toAscii().toHex());
}

void AmptekSDD123Server::textConfigurationSet(const QString &singleCommandRequest)
{
	requestDataPacket(AmptekCommandManagerSGM::RequestTextConfiguration, singleCommandRequest.toAscii().toHex());
}

bool AmptekSDD123Server::readReplyDatagram(int &id, QByteArray &datagram)
{
	if(hasReplyReady_){
		id = currentRequestPacket_.packetID();
		datagram = currentRequestPacket_.datagram();
		hasReplyReady_ = false;
		return true;
	}
	return false;
}

void AmptekSDD123Server::setRequestIntervalTimer(int mSec)
{
	requestIntervalTimer_ = mSec;
}

void AmptekSDD123Server::setSpectrumPacketReceiver(QObject *spectrumPacketReceiver)
{
	spectrumPacketReceiver_ = spectrumPacketReceiver;
}

void AmptekSDD123Server::setConfirmationPacketReceiver(QObject *confirmationPacketReceiver)
{
	confirmationPacketReceiver_ = confirmationPacketReceiver;
}

void AmptekSDD123Server::sendRequestDatagram(const AmptekSDD123Packet &packet, int overrideTimeout)
{
	if(socketLocallyBusy_){
		AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_PACKET_QUEUE_BUSY, "\nPACKET QUEUE IS BUSY (send request datagram)");
		packetQueue_.append(packet);
	} else {
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

void AmptekSDD123Server::sendSyncDatagram(const AmptekSDD123Packet &packet, int overrideTimeout)
{
	if(socketLocallyBusy_){
		AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_PACKET_QUEUE_BUSY, "PACKET QUEUE IS BUSY (send sync datagram)");
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

void AmptekSDD123Server::fakeSendDatagram(const AmptekSDD123Packet &packet, int overrideTimeout)
{
	if(socketLocallyBusy_){
		AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_PACKET_QUEUE_BUSY, "PACKET QUEUE IS BUSY (fake send datagram)");
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

void AmptekSDD123Server::processPendingDatagrams()
{
	bool emitResponsePacketReady = false;

	while(udpDetectorSocket_->hasPendingDatagrams()){
		QByteArray datagram;
		bool ok;
		datagram.resize(udpDetectorSocket_->pendingDatagramSize());
		QHostAddress *sourceAddress = new QHostAddress();
		udpDetectorSocket_->readDatagram(datagram.data(), datagram.size(), sourceAddress);

		if(datagram.size() < 520 && datagram.mid(0,2) == QByteArray::fromHex("f5fa")){
			emitResponsePacketReady = true;

			totalResponseDatagramSize_ = datagram.size();
			totalResponseDatagram_.clear();
			totalResponseDatagram_.append(datagram);
			stillReceivingResponseDatagram_ = false;
		}
		else if(stillReceivingResponseDatagram_){
			totalResponseDatagram_.append(datagram);
			if(totalResponseDatagram_.size() == totalResponseDatagramSize_){
				emitResponsePacketReady = true;
				stillReceivingResponseDatagram_ = false;
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

	if(emitResponsePacketReady)
		emit responsePacketReady();
}

void AmptekSDD123Server::onResponsePacketReady()
{
	requestPacketTimer_->stop();
	syncPacketTimer_->stop();

	AmptekSDD123Packet lastRequestPacket = currentRequestPacket_;
	AmptekSDD123Packet responsePacket(currentRequestPacket_.packetID(), totalResponseDatagram_);

	if (!responsePacket.isValid()) {
		AMErrorMon::error(this, AMPTEK_SERVER_ERR_INVALID_PACKET_DATA, QString("The datagram of reponse packet is invalid (error %1)").arg(responsePacket.lastError()));
	}

	processLocalStoredPacket(responsePacket);
	processResponseFeedback(responsePacket);
	processLastRequestPacket(lastRequestPacket);
	sendPacketInQueue();
}

void AmptekSDD123Server::syncRequest(int timeout)
{
	timeOutIDCounter_--;

	QString hashTag = QString("%1").arg(timeOutIDCounter_);
	requestDataPacket(AmptekCommandManagerSGM::RequestCommTestEchoPacket, hashTag.toAscii().toHex(), false, true, timeout);
}

void AmptekSDD123Server::fakeSyncRequest(int timeout)
{
	QString hashTag = QString("%1").arg(timeOutIDCounter_);
	requestDataPacket(AmptekCommandManagerSGM::RequestCommTestEchoPacket, hashTag.toAscii().toHex(), true, false, timeout);

	timeOutIDCounter_--;
}

void AmptekSDD123Server::requestDatagramTimeout()
{
	socketLocallyBusy_ = false;
	disconnect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
	requestPacketTimer_->stop();
	if(stillReceivingResponseDatagram_){
		connect(requestPacketTimer_, SIGNAL(timeout()), this, SLOT(requestDatagramTimeout()));
		requestPacketTimer_->start(currentRequestPacket_.timeout());
		return;
	}

	if(currentRequestPacket_.commandId() == AmptekCommandManagerSGM::RequestCommTestEchoPacket && QString(QByteArray::fromHex(currentRequestPacket_.dataString().toAscii())) == "a7xi272727;idhai"){
		detectorUnavailable_ = true;
		emit detectorUnavailableChanged(detectorUnavailable_);
		return;
	}
	timedOutPackets_.append(currentRequestPacket_);
	syncRequest();
}

void AmptekSDD123Server::syncDatagramTimeout()
{
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

void AmptekSDD123Server::processLocalStoredPacket(const AmptekSDD123Packet &responsePacket)
{
	bool foundTimedOutPacket = false;
	bool rerequestRequired = false;

	if(timedOutPackets_.count() > 0){
		for(int x = 0; x < timedOutPackets_.count(); x++){
			AMErrorMon::information(this, AMPTEK_SERVER_INFO_PACKET_QUEUE_BUSY, QString("Timed Out Packet: %1 %2").arg(timedOutPackets_.at(x).command()).arg(QString(QByteArray::fromHex(timedOutPackets_.at(x).dataString().toAscii()))));
		}

		AmptekSDD123Packet headPacket(-1, QString("NONE"));
		while(!foundTimedOutPacket && timedOutPackets_.count() > 0){
			headPacket = timedOutPackets_.dequeue();

			if(responsePacket.commandId() == AmptekCommandManagerSGM::ResponseCommTestEchoPacket && headPacket.commandId() == AmptekCommandManagerSGM::RequestCommTestEchoPacket && responsePacket.dataString() == headPacket.dataString()) {
				foundTimedOutPacket = true;

			} else if(responsePacket.commandId() != AmptekCommandManagerSGM::ResponseCommTestEchoPacket && headPacket.possibleResponses().contains(responsePacket.commandHex())){
				foundTimedOutPacket = true;
				AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
				currentRequestPacket_ = emptyPacket;

			} else {
				if(headPacket.command() == currentRequestPacket_.command() && responsePacket.dataString() == currentSyncPacket_.dataString())
					rerequestRequired = true;

				droppedPackets_.appendPacket(QDateTime::currentDateTimeUtc(), headPacket);
				emit dropPacketDetected();
			}
		}
	}

	AmptekSDD123Packet localRequestPacket(currentRequestPacket_.packetID(), currentRequestPacket_);

	if (!foundTimedOutPacket) {
		if (currentRequestPacket_.possibleResponses().contains(responsePacket.commandHex())) {
			AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
			currentRequestPacket_ = emptyPacket;
		} else if (currentSyncPacket_.dataString() == responsePacket.dataString()) {
			AmptekSDD123Packet emptyPacket(-1, QString("NONE"));
			currentSyncPacket_ = emptyPacket;
		} else {
			unrequestedPackets_.appendPacket(QDateTime::currentDateTimeUtc(), responsePacket);
			emit unrequestedPacketDetected();
		}
	}

	socketLocallyBusy_ = false;

	if(rerequestRequired){
		if(localRequestPacket.currentRetries() < localRequestPacket.maxRetries()){
			localRequestPacket.incrementRetries(packetIDCounter_);
			sendRequestDatagram(localRequestPacket);

			packetIDCounter_++;
		} else {
			emit fatalTimeout();
		}
	}

	if(localRequestPacket.isForwarded()){
		hasReplyReady_ = true;
		emit replyDatagram(responsePacket.packetID(), responsePacket.datagram());
	}
}

void AmptekSDD123Server::processResponseFeedback(const AmptekSDD123Packet &responsePacket)
{
	QByteArray spectrumByteArray = responsePacket.spectrumByteArray();
	QByteArray statusByteArray = responsePacket.statusByteArray();

	if(responsePacket.commandId() == AmptekCommandManagerSGM::ResponseRequestStatusPacket){
		emit statusDataReady(statusByteArray);
	} else if(responsePacket.commandId() == AmptekCommandManagerSGM::AcknowledgeOk){
		emit acknowledgeOK();
	} else if(responsePacket.commandId() == AmptekCommandManagerSGM::Response256ChannelSpectrumPlusStatus){
		postSpectrumPlusStatusReadyResponse(spectrumByteArray, statusByteArray, 256);
	} else if(responsePacket.commandId() == AmptekCommandManagerSGM::Response512ChannelSpectrumPlusStatus){
		postSpectrumPlusStatusReadyResponse(spectrumByteArray, statusByteArray, 512);
	} else if(responsePacket.commandId() == AmptekCommandManagerSGM::Response1024ChannelSpectrumPlusStatus){
		postSpectrumPlusStatusReadyResponse(spectrumByteArray, statusByteArray, 1024);
	} else if(responsePacket.commandId() == AmptekCommandManagerSGM::ResponseConfiguration){
		postConfigurationReadbackResponse(QString(QByteArray::fromHex(responsePacket.dataString().toAscii())));
	}
}

void AmptekSDD123Server::postSpectrumPlusStatusReadyResponse(const QByteArray &spectrumByteArray, const QByteArray &statusByteArray, int channelCount)
{
	replySpectrumTime_->restart();

	emit spectrumDataReady(spectrumByteArray, channelCount);
	emit statusDataReady(statusByteArray);

	if(spectrumPacketReceiver_ != 0) {
		QTime dwellStartTime(lastRequestSpectrumTime_->hour(), lastRequestSpectrumTime_->minute(), lastRequestSpectrumTime_->second(), lastRequestSpectrumTime_->msec());
		QTime dwellEndTime(requestSpectrumTime_->hour(), requestSpectrumTime_->minute(), requestSpectrumTime_->second(), requestSpectrumTime_->msec());
		QTime dwellReplyTime(replySpectrumTime_->hour(), replySpectrumTime_->minute(), replySpectrumTime_->second(), replySpectrumTime_->msec());

		QEvent *responseEvent = new AmptekSpectrumPacketEvent(spectrumByteArray, statusByteArray, channelCount, dwellStartTime, dwellEndTime, dwellReplyTime);

		QCoreApplication::postEvent(spectrumPacketReceiver_, responseEvent);
	} else {
		AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_SPECTRUM_EVENT_RECEIVER_UNDEFINED, QString("No spectrum packet receiver"));
	}
}

void AmptekSDD123Server::postConfigurationReadbackResponse(const QString &ASCIICommands)
{
	qDebug() << "About to check on the ConfigurationReadbackResponse";
	if(ASCIICommands.contains("ALLP")){
		qDebug() << "Did so, it's: " << ASCIICommands;
		emit allParametersTextConfiguration(ASCIICommands);

		if(spectrumPacketReceiver_ != 0){
			AmptekConfigurationReadbackEvent *configurationReadbackEvent = new AmptekConfigurationReadbackEvent();
			configurationReadbackEvent->configurationReadback_ = ASCIICommands;

			QCoreApplication::postEvent(spectrumPacketReceiver_, configurationReadbackEvent);
		} else {
			AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_SPECTRUM_EVENT_RECEIVER_UNDEFINED, QString("No spectrum packet receiver"));
		}
	}

}

void AmptekSDD123Server::processLastRequestPacket(const AmptekSDD123Packet &lastRequestPacket)
{
	if(lastRequestPacket.commandId() == AmptekCommandManagerSGM::RequestTextConfiguration){
		requestAllTextConfigurationParameters();
	} else if(lastRequestPacket.commandId() == AmptekCommandManagerSGM::RequestEnableMCAMCS){
		emit serverChangedToDwellState();
	} else if(lastRequestPacket.commandId() == AmptekCommandManagerSGM::RequestDisableMCAMCS){
		emit serverChangedToConfigurationState();

		if(confirmationPacketReceiver_ != 0){
			AmptekConfigurationModeConfirmationEvent *configurationConfirmationEvent = new AmptekConfigurationModeConfirmationEvent();
			configurationConfirmationEvent->confirmConfigurationMode_ = true;
			QCoreApplication::postEvent(confirmationPacketReceiver_, configurationConfirmationEvent);
		} else {
			AMErrorMon::alert(this, AMPTEK_SERVER_ALERT_SPECTRUM_EVENT_RECEIVER_UNDEFINED, QString("No spectrum confirmation packet receiver"));
		}
	}
}

void AmptekSDD123Server::sendPacketInQueue()
{
	if(!packetQueue_.isEmpty()){
		AmptekSDD123Packet resendPacket(packetQueue_.takeFirst());
		if(resendPacket.commandId() == AmptekCommandManagerSGM::RequestCommTestEchoPacket)
			sendSyncDatagram(resendPacket);
		else
			sendRequestDatagram(resendPacket);
	}
}

