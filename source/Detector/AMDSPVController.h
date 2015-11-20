#ifndef AMDSPVCONTROLLER_H
#define AMDSPVCONTROLLER_H

#include <QObject>
#include <QSignalMapper>
#include <QMap>

#include "Detector/AMDSDwellDetector.h"

class AMControlSet;
class AMControl;
class AMSinglePVControl;

class AMDSDataHolder;
class AMDSPVConfigurationMap;
class AMDSPVController;

#define AMDS_PV_CONTROL_ERR_CONFIGURE_UNKNOWN_PV 50201
#define AMDS_PV_CONTROL_ERR_ENABLED_PV_DISCONNECTED 50202

class AMDSPVControllerManager : public QObject
{
	Q_OBJECT
public:
	explicit AMDSPVControllerManager(QList<AMDSPVConfigurationMap *>pvConfigurations, QObject *parent = 0);
	~AMDSPVControllerManager();

	/// returns the instance of pv controller
	inline AMDSPVController *pvController() const { return pvController_; }

protected:
	/// the instance of detector manager thread
	QThread *pvControllerThread_;
	/// the instance of the PV controller
	AMDSPVController * pvController_;
};

class AMDSPVController : public AMDSDwellDetector
{
    Q_OBJECT
public:
	explicit AMDSPVController(QList<AMDSPVConfigurationMap *>pvConfigurations, QObject *parent = 0);
	~AMDSPVController();

signals:
	/// signal to indicate there is new value for a pv control (buffergroup)
	void newPVDataReceived(const QString &bufferName, AMDSDataHolder *newData);

public slots:
	/// slot to handle the signal to enable/disable tracking a PV
	void onConfigurePVControl(bool enablePv, const QString &pvName);

protected slots:
	/// slot to handle the all PV connected signal
	void onControlConnectedChanged(bool connected, AMControl* pvControl);

	/// slot to handle the value changed signal of a PV controller
	void onPVValueChanged(const QString &pvName);

protected:
	/// helper function to initialize the PV controls of the scaler
	void initializePVControls();

protected:
	/// the configurations of the pv controller
	QMap<QString, AMDSPVConfigurationMap *> pvConfigurations_;

	/// the signal mapper for the valueChanged of the pvControllers
	QSignalMapper *pvSignalMapper_;

	/// the control set to manager all the controls
	AMControlSet *pvControlSet_;
	/// the set of PV controls mapped by name
	QMap<QString, AMSinglePVControl*> pvControlSetByName_;

};

#endif // AMDSPVCONTROLLER_H
