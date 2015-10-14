#ifndef AMPTEKSDD123PACKET_H
#define AMPTEKSDD123PACKET_H

#include <QObject>
#include <QDateTime>

#include "Detector/Amptek/SGM/AmptekCommandManagerSGM.h"

class AmptekSDD123Packet : public QObject
{
	Q_OBJECT

	enum amptekSDD123PacketError{
		noError = 0,
		invalidHeader = 1,
		invalidChecksum = 2,
		otherError = 3
	};

public:
	AmptekSDD123Packet(int packetID, const QString &commandString, const QString &dataString = "", bool isForwarded = false, QObject *parent = 0);
	AmptekSDD123Packet(int packetID, const QByteArray &datagram, bool isForwarded = false, QObject *parent = 0);
	AmptekSDD123Packet(const AmptekSDD123Packet &original);
	AmptekSDD123Packet(int packetID, const AmptekSDD123Packet &original);

	QString commandString() const;
	QString commandText() const;
	QString lengthString() const;
	QString dataString() const;
	QString fullString() const;
	int timeout() const;
	int maxRetries() const;
	int currentRetries() const;
	QStringList possibleResponses() const;

	int dataLength() const;

	QByteArray datagram() const;

	bool isValid() const;
	bool isForwarded() const;
	int packetID() const;
	AmptekSDD123Packet::amptekSDD123PacketError lastError() const;

	AmptekSDD123Packet& operator=(const AmptekSDD123Packet &other);

	const QList<int>& relatedSyncRequestIDs() const;

public slots:
	void setPacketStrings(const QString &commandString, const QString &dataString = "");

	void incrementRetries(int newPacketID);
	void appendRelatedSyncRequestID(int syncRequestID);
	void removeRelatedSynchRequestID(int syncRequestID);

protected:
	void calculatePacket();
	QString calculateLengthString(const QString &dataString);
	void appendChecksum(const QByteArray &data, QByteArray &datagram);

	bool validatePacket(const QByteArray &datagram);

protected:
	QString commandString_;
	QString dataString_;
	bool isValid_;
	bool isForwarded_;
	int packetID_;
	int retries_;
	AmptekSDD123Packet::amptekSDD123PacketError lastError_;
	QList<int> relatedSyncRequestIDs_;

	QByteArray datagram_;
};


class AmptekSDD123PacketMonitor : public QObject{
Q_OBJECT

public:
	AmptekSDD123PacketMonitor(QObject *parent = 0);

	int totalOccurances() const;
	double totalRate() const;
	int occurancesSince(QDateTime timeOfInterest) const;
	double rateSince(QDateTime timeOfInterest) const;

	void appendPacket(QDateTime timeOfIncident, AmptekSDD123Packet packet);

protected:
	QList< QPair<QDateTime, AmptekSDD123Packet> > monitorList;
};

#endif // AMPTEKSDD123PACKET_H
