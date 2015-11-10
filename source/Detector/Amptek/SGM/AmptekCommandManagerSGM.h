#ifndef AMPTEKCOMMANDMANAGERSGM_H
#define AMPTEKCOMMANDMANAGERSGM_H

#include <QObject>
#include <QStringList>

#include "Detector/Amptek/AmptekCommandManager.h"

class AmptekCommandManagerSGM : public AmptekCommandManager
{
	Q_OBJECT

public:
	enum AmptekCommandDefSGM {
		// acknowledge type
		AcknowledgeOk = 0,
		AcknowledgeOKWithInterfaceSharingRequest,
		AcknowledgeSyncError,
		AcknowledgePIDError,
		AcknowledgeLENError,
		AcknowledgeChecksumError,
		AcknowledgeBadParameter,
		AcknowledgeBadHexRecrod,
		AcknowledgeUnrecognizedCommand,
		AcknowledgeFPGAError,
		AcknowledgeCP2201NotFound,
		AcknowledgeScopeDataNotAvailable,
		AcknowledgePC5NotPresent,
		AcknowledgeI2CError,

		// response type
		ResponseRequestStatusPacket,
		Response256ChannelSpectrumPlusStatus,
		Response512ChannelSpectrumPlusStatus,
		Response1024ChannelSpectrumPlusStatus,
		ResponseConfiguration,
		ResponseCommTestEchoPacket,

		// request type
		ReqeustStatusPacket,
		RequestClearSpecturm,
		RequestSpecturmPlusStatus,
		RequestAndClearSpecturmPlusStatus,
		RequestEnableMCAMCS,
		RequestDisableMCAMCS,
		RequestTextConfigurationReadback,
		RequestTextConfiguration,
		RequestCommTestACKOKPacket,
		RequestCommTestACKOKWithSharingPacket,
		RequestCommTestACKSyncErrorPacket,
		RequestCommTestACKPIDErrorPacket,
		RequestCommTestACKLENErrorPacket,
		RequestCommTestACKChecksumErrorPacket,
		RequestCommTestACKBadParameterPacket,
		RequestCommTestEchoPacket
	};

	/// the static function to get the object
	static AmptekCommandManagerSGM* sddCommands();
	/// the static function to release the object
	static void releaseSDDCommands();

	virtual ~AmptekCommandManagerSGM();

	QString requestStatusPacketHex() const;
//	QString requestSpectrumHex() const;
//	QString requestSpectrumAndClearHex() const;
//	QString clearSpectrumHex() const;
//	QString enableMCAMCSHex() const;
//	QString disableMCAMCSHex() const;
//	QString textConfigurationReadbackHex() const;
//	QString textConfigurationHex() const;
//	QString commTestRequestAckOKPacketHex() const;
//	QString commTestRequestAckOKwithSharingPacketHex() const;
//	QString commTestRequestAckSyncErrorPacketHex() const;
//	QString commTestRequestAckPIDErrorPacketHex() const;
//	QString commTestRequestAckLENErrorPacketHex() const;
//	QString commTestRequestAckChecksumErrorPacketHex() const;
//	QString commTestRequestAckBadParameterPacketHex() const;
//	QString commTestRequestEchoPacketHex() const;

/// TODO: legacy codes, there is no usage for this yet...
//	bool isAcknowledgePacket(const QString &hexString) const;
//	bool isAcknowledgeErrorPacket(const QString &hexString) const;

protected:
	/// constructor of AmptekCommand class, which is protected since AmptekCommand is used as singleton
	AmptekCommandManagerSGM();

	/// implementation of the function to initialize the amptek commands and the hex mappling
	virtual void initiateAmptekCommands();
	/// helper function to create response from given texts
	QStringList internalCreateResponsesFromText(const QStringList &texts);

protected:
	/// static Instance variable
	static AmptekCommandManagerSGM* instance_;
};

#endif // AMPTEKCOMMANDMANAGERSGM_H
