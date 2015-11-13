#ifndef SCALERCOMMANDMANAGERSGM_H
#define SCALERCOMMANDMANAGERSGM_H

#include "DataElement/AMDSCommandManager.h"

class ScalerCommandManagerSGM : public AMDSCommandManager
{
public:
	enum ScalerCommandDefSGM {
		// request type
		RequestEnableChannel = CustomizedCommand + 1,
		RequestDisableChannel
	};

	/// the static function to get the object
	static ScalerCommandManagerSGM* scalerCommandManager();
	/// the static function to release the object
	static void releaseScalerCommands();

	~ScalerCommandManagerSGM();

protected:
	/// constructor of AMDSCommand class, which is protected since AMDSCommand is used as singleton
    ScalerCommandManagerSGM();

	/// implementation of the function to initialize the amptek commands and the hex mappling
	virtual void initiateAMDSCommands();

protected:
	/// static Instance variable
	static ScalerCommandManagerSGM* instance_;
};

#endif // SCALERCOMMANDMANAGERSGM_H
