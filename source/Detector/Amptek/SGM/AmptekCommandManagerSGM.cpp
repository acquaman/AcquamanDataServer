#include "AmptekCommandManagerSGM.h"

/// ===================== implementation of the static instance and methods ==================
AmptekCommandManagerSGM* AmptekCommandManagerSGM::instance_ = 0;
AmptekCommandManagerSGM* AmptekCommandManagerSGM::sddCommands(){
	if(instance_ == 0)
		instance_ = new AmptekCommandManagerSGM();

	return instance_;
}

void AmptekCommandManagerSGM::releaseSDDCommands(){
	if(instance_) {
		delete instance_;
		instance_ = 0;
	}
}

/// ===================== implementation of the public methods ==================
AmptekCommandManagerSGM::~AmptekCommandManagerSGM()
{
}

//QString AmptekCommandManagerSGM::requestStatusPacketHex() const{
//	return amptekCommandFromCommandId(ReqeustStatusPacket).hex();
//}

//QString AmptekCommandManagerSGM::requestSpectrumHex() const{
//	return amptekCommandFromCommandId(RequestSpecturmPlusStatus).hex();
//}

//QString AmptekCommandManagerSGM::requestSpectrumAndClearHex() const{
//	return amptekCommandFromCommandId(RequestAndClearSpecturmPlusStatus).hex();
//}

//QString AmptekCommandManagerSGM::clearSpectrumHex() const{
//	return amptekCommandFromCommandId(RequestClearSpecturm).hex();
//}

//QString AmptekCommandManagerSGM::enableMCAMCSHex() const{
//	return amptekCommandFromCommandId(RequestEnableMCAMCS).hex();
//}

//QString AmptekCommandManagerSGM::disableMCAMCSHex() const{
//	return amptekCommandFromCommandId(RequestDisableMCAMCS).hex();
//}

//QString AmptekCommandManagerSGM::textConfigurationReadbackHex() const{
//	return amptekCommandFromCommandId(RequestTextConfigurationReadback).hex();
//}

//QString AmptekCommandManagerSGM::textConfigurationHex() const{
//	return amptekCommandFromCommandId(RequestTextConfiguration).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckOKPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKOKPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckOKwithSharingPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKOKWithSharingPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckSyncErrorPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKSyncErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckPIDErrorPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKPIDErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckLENErrorPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKLENErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckChecksumErrorPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKChecksumErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckBadParameterPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestACKBadParameterPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestEchoPacketHex() const{
//	return amptekCommandFromCommandId(RequestCommTestEchoPacket).hex();
//}

QString AmptekCommandManagerSGM::textFromHex(const QString &hexString) const{
	AmptekCommand other(-1, hexString, "", -1);
	return commandHexMapping_.valueR(other);
}

//bool AmptekCommandManagerSGM::isAcknowledgePacket(const QString &hexString) const{
//	if(hexString.at(0) == 'f' && hexString.at(1) == 'f')
//		return true;
//	return false;
//}

//bool AmptekCommandManagerSGM::isAcknowledgeErrorPacket(const QString &hexString) const{
//	if(isAcknowledgePacket(hexString) && hexString != "ff00" && hexString != "ff01")
//		return true;
//	return false;
//}

AmptekCommandManagerSGM::AmptekCommandManagerSGM()
{
	initiateAmptekCommands();
}

void AmptekCommandManagerSGM::initiateAmptekCommands()
{
	//Acknowledge types
	amptekCommands_.append( AmptekCommand(AcknowledgeOk,                            "ff00", "OK"));
	amptekCommands_.append( AmptekCommand(AcknowledgeOKWithInterfaceSharingRequest, "ff0c", "OK,with Interface Sharing Request"));
	amptekCommands_.append( AmptekCommand(AcknowledgeSyncError,                     "ff01", "Sync Error"));
	amptekCommands_.append( AmptekCommand(AcknowledgePIDError,                      "ff02", "PID Error"));
	amptekCommands_.append( AmptekCommand(AcknowledgeLENError,                      "ff03", "LEN Error"));
	amptekCommands_.append( AmptekCommand(AcknowledgeChecksumError,                 "ff04", "Checksum Error"));
	amptekCommands_.append( AmptekCommand(AcknowledgeBadParameter,                  "ff05", "Bad Parameter"));
	amptekCommands_.append( AmptekCommand(AcknowledgeBadHexRecrod,                  "ff06", "Bad Hex Record"));
	amptekCommands_.append( AmptekCommand(AcknowledgeUnrecognizedCommand,           "ff07", "Unrecognized Command"));
	amptekCommands_.append( AmptekCommand(AcknowledgeFPGAError,                     "ff08", "FPGA Error"));
	amptekCommands_.append( AmptekCommand(AcknowledgeCP2201NotFound,                "ff09", "CP2201 Not Found"));
	amptekCommands_.append( AmptekCommand(AcknowledgeScopeDataNotAvailable,         "ff0a", "Scope Data Not Available"));
	amptekCommands_.append( AmptekCommand(AcknowledgePC5NotPresent,                 "ff0b", "PC5 Not Present"));
	amptekCommands_.append( AmptekCommand(AcknowledgeI2CError,                      "ff0e", "I2C Error"));

	//Response types
	amptekCommands_.append( AmptekCommand(ResponseRequestStatusPacket,           "8001", "Request Status Packet Response"));
	amptekCommands_.append( AmptekCommand(Response256ChannelSpectrumPlusStatus,  "8102", "256-channel spectrum plus Status"));
	amptekCommands_.append( AmptekCommand(Response512ChannelSpectrumPlusStatus,  "8104", "512-channel spectrum plus Status"));
	amptekCommands_.append( AmptekCommand(Response1024ChannelSpectrumPlusStatus, "8105", "1024-channel spectrum plus Status"));
	amptekCommands_.append( AmptekCommand(ResponseConfiguration,                 "8207", "Configuration Readback"));
	amptekCommands_.append( AmptekCommand(ResponseCommTestEchoPacket,            "8f7f", "Comm test - Echo packet Response"));

	//Request types
	QStringList tempResponses;
	tempResponses.clear();
	tempResponses << "Request Status Packet Response" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(ReqeustStatusPacket, "0101", "Request Status Packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestClearSpecturm, "f001", "Clear Spectrum", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	amptekCommands_.append( AmptekCommand(RequestSpecturmPlusStatus, "0203", "Request Spectrum plus Status", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	amptekCommands_.append( AmptekCommand(RequestAndClearSpecturmPlusStatus, "0204", "Request and clear Spectrum plus Status", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestEnableMCAMCS, "f002", "Enable MCA/MCS", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestDisableMCAMCS, "f003", "Disable MCA/MCS", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Configuration Readback" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestTextConfigurationReadback, "2003", "Text Configuration Readback", 100, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestTextConfiguration, "2002", "Text Configuration", 25, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKOKPacket, "f100", "Comm test - Request ACK OK packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "OK,with Interface Sharing Request" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKOKWithSharingPacket, "f10c", "Comm test - Request ACK OK with Sharing packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Sync Error" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKSyncErrorPacket, "f101", "Comm test - Request ACK Sync Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "PID Error" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKPIDErrorPacket, "f102", "Comm test - Request ACK PID Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "LEN Error" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKLENErrorPacket, "f103", "Comm test - Request ACK LEN Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Checksum Error" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKChecksumErrorPacket, "f104", "Comm test - Request ACK Checksum Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Bad Parameter" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestACKBadParameterPacket, "f105", "Comm test - Request ACK Bad Parameter packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses.clear();
	tempResponses << "Comm test - Echo packet Response" << "NormalErrors";
	amptekCommands_.append( AmptekCommand(RequestCommTestEchoPacket, "f17f", "Comm test - Echo packet", 7, 1, internalCreateResponsesFromText(tempResponses)));


	for (int i = 0; i < amptekCommands_.count(); i++) {
		amptekCommandHash_[amptekCommands_.at(i).commandId()] = amptekCommands_.at(i);
		commandHexMapping_.set(amptekCommands_.at(i).command(), amptekCommands_.at(i));
	}
}

QStringList AmptekCommandManagerSGM::internalCreateResponsesFromText(QStringList texts){
	QStringList retVal;
	for(int x = 0; x < texts.count(); x++){
		if(texts.at(x) == "NormalErrors")
			retVal << amptekCommand(AcknowledgeChecksumError).hex() << amptekCommand(AcknowledgeLENError).hex() << amptekCommand("PID Error").hex();
		else
			retVal << amptekCommand(texts.at(x)).hex();
	}
	return retVal;
}
