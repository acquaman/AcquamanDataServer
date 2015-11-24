#ifndef AMDSPVCOMMANDMANAGER_H
#define AMDSPVCOMMANDMANAGER_H

#include "DataElement/AMDSCommandManager.h"

class AMDSPVCommandManager : public AMDSCommandManager
{
public:
	enum PVSConfigurationCommands {
		// request type
		RequestEnablePV = CustomizedPVCommand + 1,
		RequestDisablePV
	};

	/// the static function to get the object
	static AMDSPVCommandManager* pvCommandManager();
	/// the static function to release the object
	static void releasePVCommandManager();

	~AMDSPVCommandManager();

protected:
	/// constructor of AMDSCommand class, which is protected since AMDSCommand is used as singleton
	AMDSPVCommandManager();

	/// implementation of the function to initialize the amptek commands and the hex mappling
	virtual void initiateAMDSCommands();

protected:
	/// static Instance variable
	static AMDSPVCommandManager* instance_;
};

#endif // AMDSPVCOMMANDMANAGER_H
