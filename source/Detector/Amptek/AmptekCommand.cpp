#include "AmptekCommand.h"

/// ===================== implementation of the static instance and methods ==================
AmptekCommand* AmptekCommand::instance_ = 0;
AmptekCommand* AmptekCommand::sddCommands(){
	if(instance_ == 0)
		instance_ = new AmptekCommand();

	return instance_;
}

void AmptekCommand::releaseSDDCommands(){
	if(instance_) {
		delete instance_;
		instance_ = 0;
	}
}

/// ===================== implementation of the public methods ==================
AmptekCommand::~AmptekCommand()
{
}

QString AmptekCommand::requestStatusPacketHex() const{
	return hexFromText("Request Status Packet");
}

QString AmptekCommand::requestSpectrumHex() const{
	return hexFromText("Request Spectrum plus Status");
}

QString AmptekCommand::requestSpectrumAndClearHex() const{
	return hexFromText("Request and clear Spectrum plus Status");
}

QString AmptekCommand::clearSpectrumHex() const{
	return hexFromText("Clear Spectrum");
}

QString AmptekCommand::enableMCAMCSHex() const{
	return hexFromText("Enable MCA/MCS");
}

QString AmptekCommand::disableMCAMCSHex() const{
	return hexFromText("Disable MCA/MCS");
}

QString AmptekCommand::textConfigurationReadbackHex() const{
	return hexFromText("Text Configuration Readback");
}

QString AmptekCommand::textConfigurationHex() const{
	return hexFromText("Text Configuration");
}

QString AmptekCommand::commTestRequestAckOKPacketHex() const{
	return hexFromText("Comm test - Request ACK OK packet");
}

QString AmptekCommand::commTestRequestAckOKwithSharingPacketHex() const{
	return hexFromText("Comm test - Request ACK OK with Sharing packet");
}

QString AmptekCommand::commTestRequestAckSyncErrorPacketHex() const{
	return hexFromText("Comm test - Request ACK Sync Error packet");
}

QString AmptekCommand::commTestRequestAckPIDErrorPacketHex() const{
	return hexFromText("Comm test - Request ACK PID Error packet");
}

QString AmptekCommand::commTestRequestAckLENErrorPacketHex() const{
	return hexFromText("Comm test - Request ACK LEN Error packet");
}

QString AmptekCommand::commTestRequestAckChecksumErrorPacketHex() const{
	return hexFromText("Comm test - Request ACK Checksum Error packet");
}

QString AmptekCommand::commTestRequestAckBadParameterPacketHex() const{
	return hexFromText("Comm test - Request ACK Bad Parameter packet");
}

QString AmptekCommand::commTestRequestEchoPacketHex() const{
	return hexFromText("Comm test - Echo packet");
}

QString AmptekCommand::textFromHex(const QString &hexString) const{
	//return text2hex_.valueR(hexString);
	AmptekCommandParameter other(hexString, -1);
	return text2hex_.valueR(other);
}

QString AmptekCommand::hexFromText(const QString &textString) const{
	return text2hex_.valueF(textString).hex();
}

int AmptekCommand::timeoutFromText(const QString &textString) const{
	return text2hex_.valueF(textString).timeout();
}

int AmptekCommand::retriesFromText(const QString &textString) const{
	return text2hex_.valueF(textString).retries();
}

QStringList AmptekCommand::responsesFromText(const QString &textString) const{
	return text2hex_.valueF(textString).responseHexes();
}

bool AmptekCommand::isAcknowledgePacket(const QString &hexString) const{
	if(hexString.at(0) == 'f' && hexString.at(1) == 'f')
		return true;
	return false;
}

bool AmptekCommand::isAcknowledgeErrorPacket(const QString &hexString) const{
	if(isAcknowledgePacket(hexString) && hexString != "ff00" && hexString != "ff01")
		return true;
	return false;
}

AmptekCommand::AmptekCommand()
{
	//Acknowledge types
	text2hex_.set("OK", AmptekCommandParameter("ff00"));
	text2hex_.set("OK,with Interface Sharing Request", AmptekCommandParameter("ff0c"));
	text2hex_.set("Sync Error", AmptekCommandParameter("ff01"));
	text2hex_.set("PID Error", AmptekCommandParameter("ff02"));
	text2hex_.set("LEN Error", AmptekCommandParameter("ff03"));
	text2hex_.set("Checksum Error", AmptekCommandParameter("ff04"));
	text2hex_.set("Bad Parameter", AmptekCommandParameter("ff05"));
	text2hex_.set("Unrecognized Command", AmptekCommandParameter("ff07"));
	text2hex_.set("PC5 Not Present", AmptekCommandParameter("ff0b"));
	text2hex_.set("Bad Hex Record", AmptekCommandParameter("ff06"));
	text2hex_.set("FPGA Error", AmptekCommandParameter("ff08"));
	text2hex_.set("CP2201 Not Found", AmptekCommandParameter("ff09"));
	text2hex_.set("Scope Data Not Available", AmptekCommandParameter("ff0a"));
	text2hex_.set("I2C Error", AmptekCommandParameter("ff0e"));

	//Response types
	text2hex_.set("Request Status Packet Response", AmptekCommandParameter("8001"));
	text2hex_.set("256-channel spectrum plus Status", AmptekCommandParameter("8102"));
	text2hex_.set("512-channel spectrum plus Status", AmptekCommandParameter("8104"));
	text2hex_.set("1024-channel spectrum plus Status", AmptekCommandParameter("8106"));
	text2hex_.set("Configuration Readback", AmptekCommandParameter("8207"));
	text2hex_.set("Comm test - Echo packet Response", AmptekCommandParameter("8f7f"));

	QStringList tempResponses;
	//Request types
	tempResponses.clear();
	tempResponses << "Request Status Packet Response" << "NormalErrors";
	text2hex_.set("Request Status Packet", AmptekCommandParameter("0101", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	text2hex_.set("Clear Spectrum", AmptekCommandParameter("f001", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	text2hex_.set("Request Spectrum plus Status", AmptekCommandParameter("0203", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	text2hex_.set("Request and clear Spectrum plus Status", AmptekCommandParameter("0204", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	text2hex_.set("Enable MCA/MCS", AmptekCommandParameter("f002", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	text2hex_.set("Disable MCA/MCS", AmptekCommandParameter("f003", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Configuration Readback" << "NormalErrors";
	text2hex_.set("Text Configuration Readback", AmptekCommandParameter("2003", 100, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	text2hex_.set("Text Configuration", AmptekCommandParameter("2002", 25, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK OK packet", AmptekCommandParameter("f100", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK,with Interface Sharing Request" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK OK with Sharing packet", AmptekCommandParameter("f10c", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Sync Error" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK Sync Error packet", AmptekCommandParameter("f101", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "PID Error" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK PID Error packet", AmptekCommandParameter("f102", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "LEN Error" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK LEN Error packet", AmptekCommandParameter("f103", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Checksum Error" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK Checksum Error packet", AmptekCommandParameter("f104", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Bad Parameter" << "NormalErrors";
	text2hex_.set("Comm test - Request ACK Bad Parameter packet", AmptekCommandParameter("f105", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Comm test - Echo packet Response" << "NormalErrors";
	text2hex_.set("Comm test - Echo packet", AmptekCommandParameter("f17f", 7, 1, internalCreateResponsesFromText(tempResponses)));
}

QStringList AmptekCommand::internalCreateResponsesFromText(QStringList texts){
	QStringList retVal;
	for(int x = 0; x < texts.count(); x++){
		if(texts.at(x) == "NormalErrors")
			retVal << hexFromText("Checksum Error") << hexFromText("LEN Error") << hexFromText("PID Error");
		else
			retVal << hexFromText(texts.at(x));
	}
	return retVal;
}

AmptekCommandParameter::AmptekCommandParameter(const QString &hex, int timeout, int retries, QStringList responseHexes)
{
	hex_ = hex;
	timeout_ = timeout;
	retries_ = retries;
	responseHexes_ = responseHexes;
}

QString AmptekCommandParameter::hex() const{
	return hex_;
}

int AmptekCommandParameter::timeout() const{
	return timeout_;
}

int AmptekCommandParameter::retries() const{
	return retries_;
}

QStringList AmptekCommandParameter::responseHexes() const{
	return responseHexes_;
}
