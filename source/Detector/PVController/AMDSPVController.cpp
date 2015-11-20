#include "AMDSPVController.h"

#include <QThread>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"

#include "Detector/PVController/AMDSPVConfigurationMap.h"
#include "util/AMDSRunTimeSupport.h"

/// ==================== implementation of AMDSThreadedPVControllerManager ============================
AMDSPVControllerManager::AMDSPVControllerManager(QList<AMDSPVConfigurationMap *>pvConfigurations, QObject *parent)
	: QObject(parent)
{
	pvControllerThread_ = new QThread();

	pvController_ = new AMDSPVController(pvConfigurations);
	pvController_->moveToThread(pvControllerThread_);

	connect(pvControllerThread_, SIGNAL(finished()), pvController_, SLOT(deleteLater()));

	pvControllerThread_->start();
}

AMDSPVControllerManager::~AMDSPVControllerManager()
{
	if (pvControllerThread_->isRunning())
		pvControllerThread_->quit();

	pvControllerThread_->deleteLater();
	pvControllerThread_ = 0;
}

/// ==================== implementation of AMDSPVController ============================
AMDSPVController::AMDSPVController(QList<AMDSPVConfigurationMap *>pvConfigurations, QObject *parent)
	: AMDSDwellDetector(parent)
{
	pvSignalMapper_ = new QSignalMapper(this);

	foreach(AMDSPVConfigurationMap *pvConfiguration, pvConfigurations) {
		pvConfigurations_.insert(pvConfiguration->pvName(), pvConfiguration);
	}

	initializePVControls();
	connect(pvSignalMapper_, SIGNAL(mapped(const QString &)), this, SLOT(onPVValueChanged(const QString &)));
}

AMDSPVController::~AMDSPVController()
{
	foreach(AMControl *pvControl, pvControlSetByName_.values()) {
		pvSignalMapper_->removeMappings(pvControl);
		pvControl->deleteLater();;
	}
	pvControlSetByName_.clear();
	pvSignalMapper_->deleteLater();

	pvControlSet_->clear();
	pvControlSet_->deleteLater();

	pvConfigurations_.clear();
}

void AMDSPVController::onEnablePVController(const QString &pvName)
{
	onConfigurePVControl(true, pvName);
}

void AMDSPVController::onDisablePVController(const QString &pvName)
{
	onConfigurePVControl(false, pvName);
}

void AMDSPVController::onConfigurePVControl(bool enablePv, const QString &pvName)
{
	AMDSPVConfigurationMap *pvConfiguration = pvConfigurations_.value(pvName);
	if (pvConfiguration) {
		bool wasEnabled = pvConfiguration->enabled();
		if (wasEnabled != enablePv) {
			pvConfiguration->setPVConfigureState(enablePv);

			AMSinglePVControl *pvControl = pvControlSetByName_.value(pvName);
			if (pvConfiguration->enabled()) {
				pvSignalMapper_->setMapping(pvControl, pvName);
				connect(pvControl, SIGNAL(valueChanged(double)), pvSignalMapper_, SLOT(map()));
			} else {
				pvSignalMapper_->removeMappings(pvControl);
				disconnect(pvControl, SIGNAL(valueChanged(double)), pvSignalMapper_, SLOT(map()));
			}
		}
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_PV_CONTROL_ERR_CONFIGURE_UNKNOWN_PV, QString("AMDS PV: try to configure one unknown PV control (%1).").arg(pvName));
	}
}

void AMDSPVController::onControlConnectedChanged(bool connected, AMControl* pvControl)
{
	AMDSPVConfigurationMap *pvConfiguration = pvConfigurations_.value(pvControl->name());
	if (pvConfiguration->enabled() && connected == false) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::ErrorMsg, this, AMDS_PV_CONTROL_ERR_ENABLED_PV_DISCONNECTED, QString("AMDS PV: the PV control (%1) is disconnected.").arg(pvControl->name()));
	}
}

void AMDSPVController::onPVValueChanged(const QString &pvName)
{
	AMSinglePVControl *pvControl = pvControlSetByName_.value(pvName);
	if (pvControl) {
		AMDSPVConfigurationMap *pvConfiguration = pvConfigurations_.value(pvName);

		AMDSLightWeightScalarDataHolder *pvDataHolder = new AMDSLightWeightScalarDataHolder(pvConfiguration->dataType());
		pvDataHolder->setSingleValue(pvControl->value());

		emit newPVDataReceived(pvName, pvDataHolder);
	}
}

void AMDSPVController::initializePVControls()
{
	pvControlSet_ = new AMControlSet(this);

	AMSinglePVControl *pvControl;
	foreach (AMDSPVConfigurationMap *pvConfiguration, pvConfigurations_.values()) {
		pvControl = new AMSinglePVControl(pvConfiguration->pvName(), pvConfiguration->epicsPV(), this);

		pvControlSet_->addControl(pvControl);
		pvControlSetByName_.insert(pvConfiguration->pvName(), pvControl);

		// we only care about the enabled PV controls
		if (pvConfiguration->enabled()) {
			pvSignalMapper_->setMapping(pvControl, pvConfiguration->pvName());
			connect(pvControl, SIGNAL(valueChanged(double)), pvSignalMapper_, SLOT(map()));
		}
	}

	connect(pvControlSet_, SIGNAL(controlConnectedChanged(bool,AMControl*)), this, SLOT(onControlConnectedChanged(bool,AMControl*)));
}
