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

	/// the = operation
	AmptekSDD123Packet& operator=(const AmptekSDD123Packet &other);

	/// returns the command text
	QString commandText() const;
	/// returns the length of the datagram
	QString lengthString() const;
	/// returns the data string of the datagram in hex
	QString dataString() const;
	/// returns the datagram in hex
	QString fullString() const;
	/// returns the value of timeout
	int timeout() const;
	/// returns the value of maxRetries
	int maxRetries() const;
	/// returns the value of currentRetries
	int currentRetries() const;
	/// returns the possible responses
	QStringList possibleResponses() const;

	/// returns the legth of the data
	int dataLength() const;

	/// returns the command string
	inline QString commandString() const { return commandString_; }
	/// returns the datagram in byte
	inline QByteArray datagram() const { return datagram_; }
	/// returns the value of isValid flag
	inline bool isValid() const { return isValid_; }
	/// returns the value of isForarded flag
	inline bool isForwarded() const { return isForwarded_; }
	/// returns the value of packetID
	inline int packetID() const { return packetID_; }
	/// returns the value of lastError
	inline AmptekSDD123Packet::amptekSDD123PacketError lastError() const { return lastError_; }
	/// returns the related sync request IDs
	inline const QList<int>& relatedSyncRequestIDs() const { return relatedSyncRequestIDs_; }

public slots:
	/// slot to set the packet strings
	void setPacketStrings(const QString &commandString, const QString &dataString = "");
	/// slot to increment the retries
	void incrementRetries(int newPacketID);
	/// slot to append a related sync request ID
	void appendRelatedSyncRequestID(int syncRequestID);
	/// slot to remove a related sync request ID
	void removeRelatedSynchRequestID(int syncRequestID);

protected:
	/// helper function to calculate packet
	void calculatePacket();
	/// helper function to calculate the lenght of datastring
	QString calculateLengthString(const QString &dataString);

	/// helper function to append checksum for the given data
	void appendChecksum(const QByteArray &data, QByteArray &datagram);
	/// helper function to validate a packet
	bool validatePacket(const QByteArray &datagram);

protected:
	/// the command string of the data packet
	QString commandString_;
	/// the data string of the data packet
	QString dataString_;
	/// the isValid flag of the data packet
	bool isValid_;
	/// the isForwarded flag of the data packet
	bool isForwarded_;
	/// the packetID of the data packet
	int packetID_;
	/// the number of retries of the data packet
	int retries_;
	/// the last error of the data packet
	AmptekSDD123Packet::amptekSDD123PacketError lastError_;
	/// the related sync request ID of the data packet
	QList<int> relatedSyncRequestIDs_;

	/// the datagram of the data packet
	QByteArray datagram_;
};


class AmptekSDD123PacketMonitor : public QObject{
Q_OBJECT

public:
	AmptekSDD123PacketMonitor(QObject *parent = 0);

	/// returns the value total occurances of the packets
	int totalOccurances() const;
	/// returns the value total rate of the packets
	double totalRate() const;
	/// return the occurances since a given time
	int occurancesSince(QDateTime timeOfInterest) const;
	/// return the rate since a given time
	double rateSince(QDateTime timeOfInterest) const;

	/// append a packet
	void appendPacket(QDateTime timeOfIncident, AmptekSDD123Packet packet);

protected:
	/// the list of packets based on datetime
	QList< QPair<QDateTime, AmptekSDD123Packet> > monitorList;
};

#endif // AMPTEKSDD123PACKET_H
