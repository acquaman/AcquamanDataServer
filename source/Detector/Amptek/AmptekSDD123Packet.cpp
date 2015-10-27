#include "AmptekSDD123Packet.h"

#include <QStringBuilder>

AmptekSDD123Packet::AmptekSDD123Packet(int packetID, const QString &commandHex, const QString &dataString, bool isForwarded, QObject *parent) :
		QObject(parent)
{
	retries_ = 0;
	packetID_ = packetID;
	lastError_ = AmptekSDD123Packet::noError;
	isValid_ = false;
	isForwarded_ = isForwarded;

	setAmptekCommand(commandHex);
	setPacketStrings(dataString);
}

AmptekSDD123Packet::AmptekSDD123Packet(int packetID, const QByteArray &datagram, bool isForwarded, QObject *parent) :
		QObject(parent)
{
	retries_ = 0;
	packetID_ = packetID;
	lastError_ = AmptekSDD123Packet::noError;
	isValid_ = false;
	isForwarded_ = isForwarded;
	if(!validatePacket(datagram))
		datagram_ = QByteArray::fromHex(QString("0000").toAscii());
	else{
		isValid_ = true;

		datagram_ = datagram;
		setAmptekCommand(datagram_.mid(2,2).toHex());
	}
}

AmptekSDD123Packet::AmptekSDD123Packet(const AmptekSDD123Packet &original) :
	QObject(0)
{
	*this = original;
}

AmptekSDD123Packet::AmptekSDD123Packet(int packetID, const AmptekSDD123Packet &original)
{
	*this = original;
	packetID_ = packetID;
}


AmptekSDD123Packet& AmptekSDD123Packet::operator =(const AmptekSDD123Packet &other){
	if(this != &other){
		lastError_ = other.lastError();
		isForwarded_ = other.isForwarded();
		isValid_ = false;
		retries_ = other.currentRetries();
		relatedSyncRequestIDs_ = other.relatedSyncRequestIDs();
		packetID_ = other.packetID();
		if(!other.isValid())
			datagram_ = QByteArray::fromHex(QString("0000").toAscii());
		else{
			isValid_ = true;

			datagram_ = other.datagram();
			setAmptekCommand(datagram_.mid(2,2).toHex());
		}
	}
	return *this;
}

QString AmptekSDD123Packet::lengthString() const{
	if(isValid_)
		return fullString().mid(8, 4);
	return "";
}

QString AmptekSDD123Packet::dataString() const{
	if(isValid_){
		int length = dataLength();
		if(length > 0)
			return fullString().mid(12, length*2);
	}
	return "";

}

QString AmptekSDD123Packet::fullString() const{
	if(isValid_)
		return datagram_.toHex();
	return "";
}

int AmptekSDD123Packet::dataLength() const{
	if(isValid_){
		bool ok;
		return lengthString().toInt(&ok, 16);
	}
	return -1;
}

QByteArray AmptekSDD123Packet::spectrumByteArray()
{
	QByteArray spectrum;
	switch (commandId()) {
	case AmptekCommandManagerSGM::Response256ChannelSpectrumPlusStatus:
		spectrum = datagram_.mid(6, 768);
		break;
	case AmptekCommandManagerSGM::Response512ChannelSpectrumPlusStatus:
		spectrum = datagram_.mid(6, 1536);
		break;
	case AmptekCommandManagerSGM::Response1024ChannelSpectrumPlusStatus:
		spectrum = datagram_.mid(6, 3072);
		break;
	}

	return spectrum;
}

QByteArray AmptekSDD123Packet::statusByteArray()
{
	QByteArray status;
	switch (commandId()) {
	case AmptekCommandManagerSGM::ResponseRequestStatusPacket:
		status = datagram_.mid(6, 64);
		break;
	case AmptekCommandManagerSGM::Response256ChannelSpectrumPlusStatus:
		status = datagram_.mid(774, 64);
		break;
	case AmptekCommandManagerSGM::Response512ChannelSpectrumPlusStatus:
		status = datagram_.mid(1542, 64);
		break;
	case AmptekCommandManagerSGM::Response1024ChannelSpectrumPlusStatus:
		status = datagram_.mid(3078, 64);
		break;
	}

	return status;
}

void AmptekSDD123Packet::setPacketStrings(const QString &dataString)
{
	dataString_ = dataString;

	calculatePacket();
}

void AmptekSDD123Packet::incrementRetries(int newPacketID){
	packetID_ = newPacketID;
	retries_++;
}

void AmptekSDD123Packet::appendRelatedSyncRequestID(int syncRequestID){
	relatedSyncRequestIDs_.append(syncRequestID);
}

void AmptekSDD123Packet::removeRelatedSynchRequestID(int syncRequestID){
	relatedSyncRequestIDs_.removeOne(syncRequestID);
}

/// set the AmptekCommand
void AmptekSDD123Packet::setAmptekCommand(const QString &commandHex)
{
	amptekCommand_ = AmptekCommandManagerSGM::sddCommands()->amptekCommandFromHex(commandHex);
}

void AmptekSDD123Packet::calculatePacket(){
	QString headerString = "f5fa";
	QString lengthString = calculateLengthString(dataString_);

	QString hexString;
	if(dataLength() == 0)
		hexString = headerString%amptekCommand_.hex()%lengthString;
	else
		hexString = headerString%amptekCommand_.hex()%lengthString%dataString_;
	appendChecksum(QByteArray::fromHex(hexString.toAscii()), datagram_);
	isValid_ = true;
}

QString AmptekSDD123Packet::calculateLengthString(const QString &dataString){
	if(dataString.size()%2 != 0){
		return "";
	}
	QString retVal = QString("%1").arg(dataString.size()/2, 4, 16, QChar('0'));
	return retVal;
}

void AmptekSDD123Packet::appendChecksum(const QByteArray &data, QByteArray &datagram){
	quint16 sumBits = 0;
	bool ok;
	for(int x = 0; x < data.size(); x++)
		sumBits += (quint16)(data.mid(x, 1).toHex().toUShort(&ok, 16));

	quint16 twoBits = ~sumBits + 1;
	datagram.clear();
	datagram.append(data);
	datagram.append(QByteArray::fromHex(QByteArray::number(twoBits, 16)));
}

bool AmptekSDD123Packet::validatePacket(const QByteArray &datagram){
	QByteArray validHeader = QByteArray::fromHex(QString("f5fa").toAscii());
	QByteArray validSum = QByteArray::fromHex(QString("0000").toAscii());
	if(validHeader != datagram.mid(0,2)){
		lastError_ = AmptekSDD123Packet::invalidHeader;
		return false;
	}
	quint16 sumBits = 0;
	bool ok;
	for(int x = 0; x < datagram.size()-2; x++)
		sumBits += (quint16)(datagram.mid(x, 1).toHex().toUShort(&ok, 16));

	quint16 datagramChecksum = (quint16)(datagram.mid(datagram.size()-2, 2).toHex().toUShort(&ok, 16));

	if(validSum.toHex() != QByteArray::number(sumBits+datagramChecksum, 16).right(4)){
		lastError_ = AmptekSDD123Packet::invalidChecksum;
		return false;
	}
	return true;
}


AmptekSDD123PacketMonitor::AmptekSDD123PacketMonitor(QObject *parent) :
	QObject(parent)
{

}

int AmptekSDD123PacketMonitor::totalOccurances() const{
	return monitorList.count();
}

double AmptekSDD123PacketMonitor::totalRate() const{
	if(monitorList.count() == 0)
		return 0;
	double count = monitorList.count();
	double time = monitorList.first().first.secsTo(QDateTime::currentDateTime());
	if(time == 0)
		time = 1;
	return count/time;
}

int AmptekSDD123PacketMonitor::occurancesSince(QDateTime timeOfInterest) const{
	if(timeOfInterest.secsTo(QDateTime::currentDateTime()) <= 0)
		return 0;
	int secsTo = 0;
	for(int x = monitorList.count(); x > 0; x--){
		secsTo = monitorList.at(x-1).first.secsTo(timeOfInterest);
		if(secsTo > 0)
			return monitorList.count()-x;
	}
	return totalOccurances();
}

double AmptekSDD123PacketMonitor::rateSince(QDateTime timeOfInterest) const{
	if(timeOfInterest.secsTo(QDateTime::currentDateTime()) <= 0)
		return 0;
	int secsTo = 0;
	for(int x = monitorList.count(); x > 0; x--){
		secsTo = monitorList.at(x-1).first.secsTo(timeOfInterest);
		if(secsTo > 0){
			double count = monitorList.count()-x;
			double time = monitorList.at(x).first.secsTo(QDateTime::currentDateTime());
			if(time == 0)
				time = 1;
			return count/time;
		}
	}
	return totalRate();
}

void AmptekSDD123PacketMonitor::appendPacket(QDateTime timeOfIncident, AmptekSDD123Packet packet){
	monitorList.append(qMakePair(timeOfIncident, packet));
}
