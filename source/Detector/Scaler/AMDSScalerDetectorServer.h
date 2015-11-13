#ifndef AMDSSCALERSERVER_H
#define AMDSSCALERSERVER_H

#include "Detector/AMDSDetectorServer.h"

class AMDSScalerDetectorServer : public AMDSDetectorServer
{
    Q_OBJECT
public:
	explicit AMDSScalerDetectorServer(const QString &scalerName, QObject *parent = 0);
	~AMDSScalerDetectorServer();

signals:
	/// signal to request enable channel for "channelId"
	void enableScalerChannel(int channelId);
	/// signal to request disable channel for "channelId"
	void disableScalerChannel(int channelId);

public slots:
	/// the function to perform the configuration request
	virtual void onConfigurationRequestReceived(AMDSClientRequest *configurationRequest);

protected:
	/// helper function to make the configuration to the scaler detector
	void performConfiguration(int commandId, QVariant value);
};

#endif // AMDSSCALERSERVER_H
