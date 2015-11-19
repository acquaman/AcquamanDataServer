#ifndef AMDSSCALERCOMMANDMANAGER_H
#define AMDSSCALERCOMMANDMANAGER_H

#include "DataElement/AMDSCommandManager.h"

class AMDSScalerCommandManager : public AMDSCommandManager
{
public:
	enum ScalerSConfigurationCommands {
		// request type
		RequestEnableChannel = CustomizedScalerCommand + 1,
		RequestDisableChannel
	};

	/// the static function to get the object
	static AMDSScalerCommandManager* scalerCommandManager();
	/// the static function to release the object
	static void releaseScalerCommands();

	~AMDSScalerCommandManager();

protected:
	/// constructor of AMDSCommand class, which is protected since AMDSCommand is used as singleton
	AMDSScalerCommandManager();

	/// implementation of the function to initialize the amptek commands and the hex mappling
	virtual void initiateAMDSCommands();

protected:
	/// static Instance variable
	static AMDSScalerCommandManager* instance_;
};

#endif // AMDSSCALERCOMMANDMANAGER_H
