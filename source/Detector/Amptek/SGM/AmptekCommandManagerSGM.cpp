#include "AmptekCommandManagerSGM.h"

/// ===================== implementation of the static instance and methods ==================
AmptekCommandManagerSGM* AmptekCommandManagerSGM::instance_ = 0;
AmptekCommandManagerSGM* AmptekCommandManagerSGM::amptekCommandManager(){
	if(!instance_)
		instance_ = new AmptekCommandManagerSGM();

	return instance_;
}

void AmptekCommandManagerSGM::releaseAmptekCommands(){
	if(instance_) {
		delete instance_;
		instance_ = 0;
	}
}

/// ===================== implementation of the public methods ==================
AmptekCommandManagerSGM::~AmptekCommandManagerSGM()
{
}

QString AmptekCommandManagerSGM::requestStatusPacketHex() const{
	return amdsCommand(ReqeustStatusPacket).hex();
}

//QString AmptekCommandManagerSGM::requestSpectrumHex() const{
//	return amptekCommand(RequestSpecturmPlusStatus).hex();
//}

//QString AmptekCommandManagerSGM::requestSpectrumAndClearHex() const{
//	return amptekCommand(RequestAndClearSpecturmPlusStatus).hex();
//}

//QString AmptekCommandManagerSGM::clearSpectrumHex() const{
//	return amptekCommand(RequestClearSpecturm).hex();
//}

//QString AmptekCommandManagerSGM::enableMCAMCSHex() const{
//	return amptekCommand(RequestEnableMCAMCS).hex();
//}

//QString AmptekCommandManagerSGM::disableMCAMCSHex() const{
//	return amptekCommand(RequestDisableMCAMCS).hex();
//}

//QString AmptekCommandManagerSGM::textConfigurationReadbackHex() const{
//	return amptekCommand(RequestTextConfigurationReadback).hex();
//}

//QString AmptekCommandManagerSGM::textConfigurationHex() const{
//	return amptekCommand(RequestTextConfiguration).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckOKPacketHex() const{
//	return amptekCommand(RequestCommTestACKOKPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckOKwithSharingPacketHex() const{
//	return amptekCommand(RequestCommTestACKOKWithSharingPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckSyncErrorPacketHex() const{
//	return amptekCommand(RequestCommTestACKSyncErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckPIDErrorPacketHex() const{
//	return amptekCommand(RequestCommTestACKPIDErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckLENErrorPacketHex() const{
//	return amptekCommand(RequestCommTestACKLENErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckChecksumErrorPacketHex() const{
//	return amptekCommand(RequestCommTestACKChecksumErrorPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestAckBadParameterPacketHex() const{
//	return amptekCommand(RequestCommTestACKBadParameterPacket).hex();
//}

//QString AmptekCommandManagerSGM::commTestRequestEchoPacketHex() const{
//	return amptekCommand(RequestCommTestEchoPacket).hex();
//}

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
	initiateAMDSCommands();
}

void AmptekCommandManagerSGM::initiateAMDSCommands()
{
	//Acknowledge types
	commands_.append( AMDSCommand(AcknowledgeOk,                            "ff00", "OK"));
	commands_.append( AMDSCommand(AcknowledgeOKWithInterfaceSharingRequest, "ff0c", "OK,with Interface Sharing Request"));
	commands_.append( AMDSCommand(AcknowledgeSyncError,                     "ff01", "Sync Error"));
	commands_.append( AMDSCommand(AcknowledgePIDError,                      "ff02", "PID Error"));
	commands_.append( AMDSCommand(AcknowledgeLENError,                      "ff03", "LEN Error"));
	commands_.append( AMDSCommand(AcknowledgeChecksumError,                 "ff04", "Checksum Error"));
	commands_.append( AMDSCommand(AcknowledgeBadParameter,                  "ff05", "Bad Parameter"));
	commands_.append( AMDSCommand(AcknowledgeBadHexRecrod,                  "ff06", "Bad Hex Record"));
	commands_.append( AMDSCommand(AcknowledgeUnrecognizedCommand,           "ff07", "Unrecognized Command"));
	commands_.append( AMDSCommand(AcknowledgeFPGAError,                     "ff08", "FPGA Error"));
	commands_.append( AMDSCommand(AcknowledgeCP2201NotFound,                "ff09", "CP2201 Not Found"));
	commands_.append( AMDSCommand(AcknowledgeScopeDataNotAvailable,         "ff0a", "Scope Data Not Available"));
	commands_.append( AMDSCommand(AcknowledgePC5NotPresent,                 "ff0b", "PC5 Not Present"));
	commands_.append( AMDSCommand(AcknowledgeI2CError,                      "ff0e", "I2C Error"));

	//Response types
	commands_.append( AMDSCommand(ResponseRequestStatusPacket,           "8001", "Request Status Packet Response"));
	commands_.append( AMDSCommand(Response256ChannelSpectrumPlusStatus,  "8102", "256-channel spectrum plus Status"));
	commands_.append( AMDSCommand(Response512ChannelSpectrumPlusStatus,  "8104", "512-channel spectrum plus Status"));
	commands_.append( AMDSCommand(Response1024ChannelSpectrumPlusStatus, "8106", "1024-channel spectrum plus Status"));
	commands_.append( AMDSCommand(ResponseConfiguration,                 "8207", "Configuration Readback"));
	commands_.append( AMDSCommand(ResponseCommTestEchoPacket,            "8f7f", "Comm test - Echo packet Response"));

	//Request types
	QStringList tempResponses = QStringList() << "Request Status Packet Response" << "NormalErrors";
	commands_.append( AMDSCommand(ReqeustStatusPacket, "0101", "Request Status Packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK" << "NormalErrors";
	commands_.append( AMDSCommand(RequestClearSpecturm, "f001", "Clear Spectrum", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	commands_.append( AMDSCommand(RequestSpecturmPlusStatus, "0203", "Request Spectrum plus Status", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "1024-channel spectrum plus Status" << "NormalErrors" << "FPGA Error";
	commands_.append( AMDSCommand(RequestAndClearSpecturmPlusStatus, "0204", "Request and clear Spectrum plus Status", 50, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK" << "NormalErrors";
	commands_.append( AMDSCommand(RequestEnableMCAMCS, "f002", "Enable MCA/MCS", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK" << "NormalErrors";
	commands_.append( AMDSCommand(RequestDisableMCAMCS, "f003", "Disable MCA/MCS", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "Configuration Readback" << "NormalErrors";
	commands_.append( AMDSCommand(RequestTextConfigurationReadback, "2003", "Text Configuration Readback", 100, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK" << "NormalErrors";
	commands_.append( AMDSCommand(RequestTextConfiguration, "2002", "Text Configuration", 25, 1, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKOKPacket, "f100", "Comm test - Request ACK OK packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "OK,with Interface Sharing Request" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKOKWithSharingPacket, "f10c", "Comm test - Request ACK OK with Sharing packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "Sync Error" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKSyncErrorPacket, "f101", "Comm test - Request ACK Sync Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "PID Error" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKPIDErrorPacket, "f102", "Comm test - Request ACK PID Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "LEN Error" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKLENErrorPacket, "f103", "Comm test - Request ACK LEN Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "Checksum Error" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKChecksumErrorPacket, "f104", "Comm test - Request ACK Checksum Error packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "Bad Parameter" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestACKBadParameterPacket, "f105", "Comm test - Request ACK Bad Parameter packet", 7, 3, internalCreateResponsesFromText(tempResponses)));

	tempResponses = QStringList() << "Comm test - Echo packet Response" << "NormalErrors";
	commands_.append( AMDSCommand(RequestCommTestEchoPacket, "f17f", "Comm test - Echo packet", 7, 1, internalCreateResponsesFromText(tempResponses)));


	for (int i = 0; i < commands_.count(); i++) {
		commandHash_[commands_.at(i).commandId()] = commands_.at(i);
		commandHexMapping_.set(commands_.at(i).command(), commands_.at(i));
	}
}

QStringList AmptekCommandManagerSGM::internalCreateResponsesFromText(const QStringList &texts){
	QStringList retVal;
	for(int x = 0; x < texts.count(); x++){
		if(texts.at(x) == "NormalErrors")
			retVal << amdsCommand(AcknowledgeChecksumError).hex() << amdsCommand(AcknowledgeLENError).hex() << amdsCommand("PID Error").hex();
		else
			retVal << amdsCommand(texts.at(x)).hex();
	}
	return retVal;
}
