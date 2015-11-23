#ifndef AMDSPVCONTROLLERSERVER_H
#define AMDSPVCONTROLLERSERVER_H

#include "Detector/AMDSDetectorServer.h"

class AMDSPVControllerServer : public AMDSDetectorServer
{
    Q_OBJECT
public:
	explicit AMDSPVControllerServer(const QString &pvName, QObject *parent = 0);
	~AMDSPVControllerServer();

signals:
	/// signal to request enable pv control for "pvName"
	void enablePVController(const QString &pvName);
	/// signal to request disable pv control for "pvName"
	void disablePVController(const QString &pvName);

public slots:
	/// the function to perform the configuration request
	virtual void onConfigurationRequestReceived(AMDSClientRequest *configurationRequest);

protected:
	/// helper function to make the configuration to the PV Controllers
	void performConfiguration(int commandId, const QString &value);
};

#endif // AMDSPVCONTROLLERSERVER_H
