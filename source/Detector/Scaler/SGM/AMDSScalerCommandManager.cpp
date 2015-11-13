#include "ScalerCommandManagerSGM.h"

/// ===================== implementation of the static instance and methods ==================
ScalerCommandManagerSGM* ScalerCommandManagerSGM::instance_ = 0;

ScalerCommandManagerSGM* ScalerCommandManagerSGM::scalerCommandManager()
{
	if (!instance_) {
		instance_ = new ScalerCommandManagerSGM();
	}

	return instance_;
}

void ScalerCommandManagerSGM::releaseScalerCommands()
{
	if (instance_) {
		instance_->deleteLater();
		instance_ = 0;
	}
}

ScalerCommandManagerSGM::~ScalerCommandManagerSGM()
{

}

ScalerCommandManagerSGM::ScalerCommandManagerSGM()
{
	initiateAMDSCommands();
}

void ScalerCommandManagerSGM::initiateAMDSCommands()
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
