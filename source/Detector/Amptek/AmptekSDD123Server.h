#ifndef AMPTEKSDD123SERVER_H
#define AMPTEKSDD123SERVER_H

#include <QObject>
#include <QTime>
#include <QStringList>
#include <QQueue>

#include <QSharedMemory>
#include <QTimer>

#include "AmptekSDD123Packet.h"
#include "AmptekSDD123ConfigurationMap.h"

class QUdpSocket;

#define AMPTEK_SERVER_ALERT_FAILED_TO_BOUND_UDP 30200
#define AMPTEK_SERVER_ALERT_PACKET_QUEUE_BUSY 30201
#define AMPTEK_SERVER_ALERT_SPECTRUM_EVENT_RECEIVER_UNDEFINED 30202
#define AMPTEK_SERVER_INFO_PACKET_QUEUE_BUSY 30210
#define AMPTEK_SERVER_ERR_INVALID_PACKET_DATA 30220

class AmptekSDD123Server : public QObject
{
Q_OBJECT

public:
	AmptekSDD123Server(AmptekSDD123ConfigurationMap *configurationMap, const QString &serverName, QObject *parent = 0);
	~AmptekSDD123Server();

	/// returns the statistics of dropped packets
	QList<double> droppedPacketStatistics() const;
	/// returns the statistics of unrequested packets
	QList<double> unrequestedPacketStatistics() const;

	/// function to hadle the system events
	bool event(QEvent *e);

	/// returns whether the detector is unavailable
	inline bool detectorUnavailable() const { return detectorUnavailable_; }

public slots:
	/// Returns id of newly created packet or -1 in case of failure
	int forwardDatagram(const QByteArray &datagram);

	/// request data packat
	void requestDataPacket(AmptekCommandManagerSGM::AmptekCommandDefSGM command, const QString &dataStringHex="", bool fakeSendRequest=false, bool sendSyncRequest=false, int overrideTimeout=-1);
	/// request to disable MCA/MCS
	void disableMCAMCS();
	/// request MCS channels count
	void mcaChannelsCount();
	/// request preset time
	void presetTime();
	/// request all text configuration parameters
	void requestAllTextConfigurationParameters();
	/// request text configuration readback with single/multi command request(s)
	void textConfigurationReadback(const QString &singleCommandRequest);
	void textConfigurationReadback(const QStringList &multiCommandRequest);
	/// Sets a text configuration value. Must be the format of CMD=VALUE;
	void textConfigurationSet(const QString &singleCommandRequest);

	/// read the reply datagram
	bool readReplyDatagram(int &id, QByteArray &datagram);

	/// set the request interval timer
	void setRequestIntervalTimer(int mSec);
	/// set the spectrum packet receiver
	void setSpectrumPacketReceiver(QObject *spectrumPacketReceiver);
	/// set the confirmation packet receiver
	void setConfirmationPacketReceiver(QObject *confirmationPacketReceiver);

signals:
	/// signal to acknowledge OK
	void acknowledgeOK();
	/// signal to indicate the spectrum data is ready
	void spectrumDataReady(const QByteArray &spectrumData, int numChannels);
	/// signal to indicate the status data is ready
	void statusDataReady(const QByteArray &statusData);
	/// signal to indicate all parameters text configuration is ready
	void allParametersTextConfiguration(const QString &textConfiguration);
	/// signal to indicate the response packet is ready
	void responsePacketReady();

	/// signal to indicate system detected drop packet
	void dropPacketDetected();
	/// signal to indicate system detected unrequested packet
	void unrequestedPacketDetected();

	/// signal to indicate a reply datagram is ready
	void replyDatagram(int id, const QByteArray &datagram);

	/// signal to indicate detector availability changed
	void detectorUnavailableChanged(bool detectorUnavailable);
	/// signal to indicate fatal timeout
	void fatalTimeout();

	/// signal to indicate that the server is going to change to configuration state
	void serverAboutToChangeToConfigurationState();
	/// signal to indicate that the server changed to configuration state
	void serverChangedToConfigurationState();
	/// signal to indicate that the server changed to dwell state
	void serverChangedToDwellState();

protected slots:
	/// helper function to send request datagram
	void sendRequestDatagram(const AmptekSDD123Packet &packet, int overrideTimeout = -1);
	/// helper function to send request datagram synchronizedly
	void sendSyncDatagram(const AmptekSDD123Packet &packet, int overrideTimeout = -1);
	/// helper function to send fake request datagram
	void fakeSendDatagram(const AmptekSDD123Packet &packet, int overrideTimeout = -1);

	/// helper function to percess pending diagrams
	void processPendingDatagrams();
	/// helper function to handle reponse packet ready signal
	void onResponsePacketReady();

	/// helper function to synchonize request
	void syncRequest(int timeout = -1);
	/// helper function to fake synchonize request
	void fakeSyncRequest(int timeout = -1);
	/// helper function to handle request datagram timeout
	void requestDatagramTimeout();
	/// helper function to handle request sync datagram timeout
	void syncDatagramTimeout();

protected:
	/// helper function to handle the local timedout message queue for the responsePacket
	void processLocalStoredPacket(const AmptekSDD123Packet &responsePacket);

	/// helper function to send the response packet
	void processResponseFeedback(const AmptekSDD123Packet &responsePacket);
	/// helper function to post the spectrumPlusStatusReady response
	void postSpectrumPlusStatusReadyResponse(const QByteArray &spectrumByteArray, const QByteArray &statusByteArray, int channelCount);
	/// helper function to post the configuration readback response
	void postConfigurationReadbackResponse(const QString &ASCIICommands);

	/// helper funtion to check the last request packet
	void processLastRequestPacket(const AmptekSDD123Packet &lastRequestPacket);
	/// helper function to send the first packet in queue
	void sendPacketInQueue();

protected:
	/// the udp detector socket
	QUdpSocket *udpDetectorSocket_;

	/// the name of the server
	QString serverName_;
	/// the configruation of th server
	AmptekSDD123ConfigurationMap *configurationMap_;

	/// the total response datagram
	QByteArray totalResponseDatagram_;
	/// the size of the total response datagram
	int totalResponseDatagramSize_;
	/// the number of bytes we've read so far for this response
	int currentlyReadSize_;
	/// the byte pattern that signifies the amptek packet header
	QByteArray amptekHeaderBytes_;
	/// flag to indicate whether the total response datagram is still on the way (more packets are expecting)
	bool stillReceivingResponseDatagram_;
//	/// flag to indicate that the response packet is ready
//	bool responsePacketReady_;

	/// flag to indicate the socket is busy
	bool socketLocallyBusy_;
	/// flag to indicate the detector is unavailable
	bool detectorUnavailable_;

	/// the current request packet
	AmptekSDD123Packet currentRequestPacket_;
	/// the current synch packet
	AmptekSDD123Packet currentSyncPacket_;
	/// the queue of packets
	QList<AmptekSDD123Packet> packetQueue_;

	/// the current time of data request
	QTime currentRequestTime_;
	/// the request packet timer
	QTimer *requestPacketTimer_;
	/// the synch packet timer
	QTimer *syncPacketTimer_;
	/// the list of timedout packet
	QQueue<AmptekSDD123Packet> timedOutPackets_;

	/// the dropped packets
	AmptekSDD123PacketMonitor droppedPackets_;
	/// the unrequested packets
	AmptekSDD123PacketMonitor unrequestedPackets_;

	/// flag to indicate that reqplay is ready
	bool hasReplyReady_;
	/// the counter of packetID
	int packetIDCounter_;

	/// the timedout packet
	AmptekSDD123Packet timedOutPacket_;
	/// the coutner of timedout packet
	int timeOutIDCounter_;

	/// the interval to request packet
	int requestIntervalTimer_;
	/// the receiver of spectrum packet
	QObject *spectrumPacketReceiver_;
	/// the receiver of the confirmation packet
	QObject *confirmationPacketReceiver_;

	/// the latest time of the request spectrum
	QTime *requestSpectrumTime_;
	/// the latest time of the last request spectrum
	QTime *lastRequestSpectrumTime_;
	/// the latest time of the reply request spectrum
	QTime *replySpectrumTime_;
};

#endif // AMPTEKSDD123SERVER_H
