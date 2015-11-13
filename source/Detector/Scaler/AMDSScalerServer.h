#ifndef AMDSSCALERSERVER_H
#define AMDSSCALERSERVER_H

#include "Detector/AMDSDetectorServer.h"

class AMDSScalerServer : public AMDSDetectorServer
{
    Q_OBJECT
public:
    explicit AMDSScalerServer(QObject *parent = 0);
	~AMDSScalerServer();

signals:
	/// signal to request enable channel for "channelId"
	void enableChannel(int channelId);
	/// signal to request disable channel for "channelId"
	void disableChannel(int channelId);

public slots:
	/// the function to perform the configuration request
	virtual void onConfigurationRequestReceived(AMDSClientRequest *configurationRequest);

protected:
	/// helper function to make the configuration to the scaler detector
	void performConfiguration(int commandId, QVariant value);
};

#endif // AMDSSCALERSERVER_H
