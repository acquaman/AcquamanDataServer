#include "AMDSAppController.h"

#include "util/AMDSRunTimeSupport.h"

AMDSAppController *AMDSAppController::appController_ = 0;

AMDSAppController *AMDSAppController::appController()
{
	if (!appController_ )
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, 0, 0, "The app controller is NOT instanced!");

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
	AMDSRunTimeSupport::setDebugLevel(AMDSRunTimeSupport::Alert);

	appType_ = appType;
}
