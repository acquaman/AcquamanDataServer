#include "AmptekSDD123Packet.h"

#include <QStringBuilder>

AmptekSDD123Packet::AmptekSDD123Packet(int packetID, const QString &commandString, const QString &dataString, bool isForwarded, QObject *parent) :
		QObject(parent)
{
	retries_ = 0;
	packetID_ = packetID;
	lastError_ = AmptekSDD123Packet::noError;
	isValid_ = false;
	isForwarded_ = isForwarded;
	setPacketStrings(commandString, dataString);
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
		datagram_ = datagram;
		commandString_ = datagram_.mid(2,2).toHex();
		isValid_ = true;
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

QString AmptekSDD123Packet::commandString() const{
	return commandString_;
}

QString AmptekSDD123Packet::commandText() const{
	return AmptekCommandManagerSGM::sddCommands()->textFromHex(commandString());
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

int AmptekSDD123Packet::timeout() const{
	return AmptekCommandManagerSGM::sddCommands()->amptekCommand(commandText()).timeout();
}

int AmptekSDD123Packet::maxRetries() const{
	return AmptekCommandManagerSGM::sddCommands()->amptekCommand(commandText()).retries();
}

int AmptekSDD123Packet::currentRetries() const{
	return retries_;
}

QStringList AmptekSDD123Packet::possibleResponses() const{
	return AmptekCommandManagerSGM::sddCommands()->amptekCommand(commandText()).responseHexes();
}

int AmptekSDD123Packet::dataLength() const{
	if(isValid_){
		bool ok;
		return lengthString().toInt(&ok, 16);
	}
	return -1;
}

QByteArray AmptekSDD123Packet::datagram() const{
	return datagram_;
}

bool AmptekSDD123Packet::isValid() const{
	return isValid_;
}

bool AmptekSDD123Packet::isForwarded() const{
	return isForwarded_;
}

int AmptekSDD123Packet::packetID() const{
	return packetID_;
}

AmptekSDD123Packet::amptekSDD123PacketError AmptekSDD123Packet::lastError() const{
	return lastError_;
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
			datagram_ = other.datagram();
			commandString_ = datagram_.mid(2,2).toHex();
			isValid_ = true;
		}
	}
	return *this;
}

const QList<int>& AmptekSDD123Packet::relatedSyncRequestIDs() const{
	return relatedSyncRequestIDs_;
}

void AmptekSDD123Packet::setPacketStrings(const QString &commandString, const QString &dataString){
	commandString_ = commandString;
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

void AmptekSDD123Packet::calculatePacket(){
	QString headerString = "f5fa";
	QString lengthString = calculateLengthString(dataString_);

	QString hexString;
	if(dataLength() == 0)
		hexString = headerString%commandString_%lengthString;
	else
		hexString = headerString%commandString_%lengthString%dataString_;
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
