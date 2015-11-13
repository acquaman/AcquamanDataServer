#include "AMDSAppController.h"

#include "util/AMErrorMonitor.h"

AMDSAppController *AMDSAppController::appController_ = 0;

AMDSAppController *AMDSAppController::appController()
{
	if (!appController_)
		AMErrorMon::error(0, 0, "The app controller is NOT instanced!");

	return appController_;
}

void AMDSAppController::releaseAppController()
{
	if (appController_) {
		appController_->deleteLater();
		appController_ = 0;
	}
}

AMDSAppController::AMDSAppController(AMDSAppController::AMDSAppType appType, QObject *parent)
	:QObject(parent)
{
	appType_ = appType;
}