#ifndef ALLCOMMANDS_H
#define ALLCOMMANDS_H

#include <QObject>
#include <QStringList>

//#include "../acquaman/source/util/AMBiHash.h"
#include "util/AMBiHash.h"

class AmptekCommandParameter;

class AmptekCommand : public QObject
{
Q_OBJECT
public:
	/// the static function to get the object
	static AmptekCommand* sddCommands();
	/// the static function to release the object
	static void releaseSDDCommands();

	virtual ~AmptekCommand();

	/// returns the Hex code of "Request Status Packet" command
	QString requestStatusPacketHex() const;
	/// returns the Hex code of "Request Spectrum plus Status" command
	QString requestSpectrumHex() const;
	/// returns the Hex code of "Request and clear Spectrum plus Status" command
	QString requestSpectrumAndClearHex() const;
	/// returns the Hex code of "Clear Spectrum" command
	QString clearSpectrumHex() const;
	/// returns the Hex code of "Enable MCA/MCS" command
	QString enableMCAMCSHex() const;
	/// returns the Hex code of "Disable MCA/MCS" command
	QString disableMCAMCSHex() const;
	QString textConfigurationReadbackHex() const;
	QString textConfigurationHex() const;
	QString commTestRequestAckOKPacketHex() const;
	QString commTestRequestAckOKwithSharingPacketHex() const;
	QString commTestRequestAckSyncErrorPacketHex() const;
	QString commTestRequestAckPIDErrorPacketHex() const;
	QString commTestRequestAckLENErrorPacketHex() const;
	QString commTestRequestAckChecksumErrorPacketHex() const;
	QString commTestRequestAckBadParameterPacketHex() const;
	QString commTestRequestEchoPacketHex() const;

	QString textFromHex(const QString &hexString) const;
	QString hexFromText(const QString &textString) const;
	int timeoutFromText(const QString &textString) const;
	int retriesFromText(const QString &textString) const;
	QStringList responsesFromText(const QString &textString) const;

	bool isAcknowledgePacket(const QString &hexString) const;
	bool isAcknowledgeErrorPacket(const QString &hexString) const;

protected:
	/// constructor of AmptekCommand class, which is protected since AmptekCommand is used as singleton
	AmptekCommand();

	QStringList internalCreateResponsesFromText(QStringList texts);

protected:
	/// static Instance variable
	static AmptekCommand* instance_;

	AMBiHash<QString, AmptekCommandParameter> text2hex_;
};

class AmptekCommandParameter
{
public:
	AmptekCommandParameter(const QString &hex = "", int timeout = -1, int retries = 0, QStringList responseHexes = QStringList());

	QString hex() const;
	int timeout() const;
	int retries() const;
	QStringList responseHexes() const;

protected:
	QString hex_;
	int timeout_;
	int retries_;
	QStringList responseHexes_;
};

inline bool operator==(const AmptekCommandParameter &cp1, const AmptekCommandParameter &cp2){
	if(cp1.hex() == cp2.hex())
		return true;
	return false;
}

inline uint qHash(const AmptekCommandParameter &sddCommandParameters){
	return qHash(sddCommandParameters.hex());
}

#endif // ALLCOMMANDS_H
