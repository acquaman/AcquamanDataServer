#include "AMDSPVCommandManager.h"

/// ===================== implementation of the static instance and methods ==================
AMDSPVCommandManager* AMDSPVCommandManager::instance_ = 0;

AMDSPVCommandManager* AMDSPVCommandManager::pvCommandManager()
{
	if (!instance_) {
		instance_ = new AMDSPVCommandManager();
	}

	return instance_;
}

void AMDSPVCommandManager::releasePVCommandManager()
{
	if (instance_) {
		instance_->deleteLater();
		instance_ = 0;
	}
}

AMDSPVCommandManager::~AMDSPVCommandManager()
{

}

AMDSPVCommandManager::AMDSPVCommandManager()
{
	initiateAMDSCommands();
}

void AMDSPVCommandManager::initiateAMDSCommands()
{
	//Request types
	commands_.append( AMDSCommand(RequestStartDwell,  "0200", "Start Dwell",  7, 3, QStringList() << "0200"));
	commands_.append( AMDSCommand(RequestStopDwell,   "0201", "Stop Dwell",  7, 3, QStringList() << "0201"));
	commands_.append( AMDSCommand(RequestEnablePV,    "0301", "Enable PV Buffer",  7, 3, QStringList() << "0301"));
	commands_.append( AMDSCommand(RequestDisablePV,   "0301", "Disable PV Buffer", 7, 3, QStringList() << "0301"));

	for (int i = 0; i < commands_.count(); i++) {
		commandHash_[commands_.at(i).commandId()] = commands_.at(i);
		commandHexMapping_.set(commands_.at(i).command(), commands_.at(i));
	}
}
