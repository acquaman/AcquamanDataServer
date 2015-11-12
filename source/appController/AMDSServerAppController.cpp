#include "AMDSServerAppController.h"

AMDSServerAppController *AMDSServerAppController::serverAppController()
{
	if (!appController_) {
		appController_ = new AMDSServerAppController();
	}

	return qobject_cast<AMDSServerAppController *>(appController_);
}

AMDSServerAppController::AMDSServerAppController(QObject *parent)
	:AMDSAppController(AMDSAppController::Server, parent)
{
}

AMDSServerAppController::~AMDSServerAppController()
{

}

