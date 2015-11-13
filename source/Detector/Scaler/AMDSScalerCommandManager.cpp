#include "AMDSScalerCommandManager.h"

/// ===================== implementation of the static instance and methods ==================
AMDSScalerCommandManager* AMDSScalerCommandManager::instance_ = 0;

AMDSScalerCommandManager* AMDSScalerCommandManager::scalerCommandManager()
{
	if (!instance_) {
		instance_ = new AMDSScalerCommandManager();
	}

	return instance_;
}

void AMDSScalerCommandManager::releaseScalerCommands()
{
	if (instance_) {
		instance_->deleteLater();
		instance_ = 0;
	}
}

AMDSScalerCommandManager::~AMDSScalerCommandManager()
{

}

AMDSScalerCommandManager::AMDSScalerCommandManager()
{
	initiateAMDSCommands();
}

void AMDSScalerCommandManager::initiateAMDSCommands()
{
	//Acknowledge types
	commands_.append( AMDSCommand(AcknowledgeOk,                            "ff00", "OK"));

	//Request types
//	QStringList tempResponses = QStringList() << "Request Status Packet Response" << "NormalErrors";
	commands_.append( AMDSCommand(RequestStartDwell,  "0200", "Start Dwell",  7, 3, QStringList() << "0200"));
	commands_.append( AMDSCommand(RequestEnableChannel,  "0201", "Enable Scaler Channel",  7, 3, QStringList() << "0201"));
	commands_.append( AMDSCommand(RequestDisableChannel, "0202", "Disable Scaler Channel", 7, 3, QStringList() << "0202"));

	for (int i = 0; i < commands_.count(); i++) {
		commandHash_[commands_.at(i).commandId()] = commands_.at(i);
		commandHexMapping_.set(commands_.at(i).command(), commands_.at(i));
	}
}
