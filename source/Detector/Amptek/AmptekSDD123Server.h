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

class AmptekSDD123Server : public QObject
{
Q_OBJECT

public:
	AmptekSDD123Server(AmptekSDD123ConfigurationMap *configurationMap, const QString &serverName, QObject *parent = 0);

	bool detectorUnavailable() const;

	QList<double> droppedPacketStatistics() const;
	QList<double> unrequestedPacketStatistics() const;

	bool event(QEvent *e);

public slots:
	/// Returns id of newly created packet or -1 in case of failure
	int forwardDatagram(const QByteArray &datagram);

	void requestDataPacket(AmptekCommandManagerSGM::AmptekCommandDefSGM command, const QString &dataStringHex="", bool fakeSendRequest=false, bool sendSyncRequest=false, int overrideTimeout=-1);
//	void requestStatus();
//	void fakeRequestStatus();
//	void requestSpectrum();
//	void requestSpectrumAndClear();
//	void clearSpectrum();
//	void enableMCAMCS();
	void disableMCAMCS();
	void mcaChannelsCount();
	void presetTime();
	void requestAllTextConfigurationParameters();
	void textConfigurationReadback(QString singleCommandRequest);
	void textConfigurationReadback(QStringList multiCommandRequest);
	/// Sets a text configuration value. Must be the format of CMD=VALUE;
	void textConfigurationSet(QString singleCommandRequest);

//	void requestCommTestAckPacket(const QString &hexCommand);
//	void requestCommTestEchoPacket(const QString &hashTag = QString());
//	void fakeRequestCommTestEchoPacket(const QString &hashTag = QString());


	bool readReplyDatagram(int &id, QByteArray &datagram);

	void setRequestIntervalTimer(int mSec);
	void setSpectrumPacketReceiver(QObject *spectrumPacketReceiver);
	void setConfirmationPacketReceiver(QObject *confirmationPacketReceiver);

signals:
	void acknowledgeOK();
	void spectrumDataReady(const QByteArray &spectrumData, int numChannels);
	void statusDataReady(const QByteArray &statusData);
	void allParametersTextConfiguration(const QString &textConfiguration);
	void responsePacketReady();

	void dropPacketDetected();
	void unrequestedPacketDetected();

	void replyDatagram(int id, const QByteArray &datagram);

	void detectorUnavailableChanged(bool detectorUnavailable);
	void fatalTimeout();

	void serverAboutToChangeToConfigurationState();
	void serverChangedToConfigurationState();
	void serverChangedToDwellState();

protected slots:
	void sendRequestDatagram(AmptekSDD123Packet packet, int overrideTimeout = -1);
	void sendSyncDatagram(AmptekSDD123Packet packet, int overrideTimeout = -1);
	void fakeSendDatagram(AmptekSDD123Packet packet, int overrideTimeout = -1);

	void processPendingDatagrams();
	void onResponsePacketReady();

	void syncRequest(int timeout = -1);
	void fakeSyncRequest(int timeout = -1);
	void requestDatagramTimeout();
	void syncDatagramTimeout();

protected:
	QUdpSocket *udpDetectorSocket_;
	QString serverName_;
	AmptekSDD123ConfigurationMap *configurationMap_;

	QByteArray totalResponseDatagram_;
	int totalResponseDatagramSize_;
	bool stillReceivingResponseDatagram_;
	bool responsePacketReady_;

	bool socketLocallyBusy_;
	bool detectorUnavailable_;
	AmptekSDD123Packet currentRequestPacket_;
	AmptekSDD123Packet currentSyncPacket_;
	QList<AmptekSDD123Packet> packetQueue_;

	QTime currentRequestTime_;
	QTimer *requestPacketTimer_;
	QTimer *syncPacketTimer_;
	QQueue<AmptekSDD123Packet> timedOutPackets_;

	AmptekSDD123PacketMonitor droppedPackets_;
	AmptekSDD123PacketMonitor unrequestedPackets_;

	bool hasReplyReady_;
	int packetIDCounter_;

	AmptekSDD123Packet timedOutPacket_;
	int timeOutIDCounter_;

	int requestIntervalTimer_;
	QObject *spectrumPacketReceiver_;
	QObject *confirmationPacketReceiver_;

	QTime *requestSpectrumTime_;
	QTime *lastRequestSpectrumTime_;
	QTime *replySpectrumTime_;
};

#endif // AMPTEKSDD123SERVER_H
