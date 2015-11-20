#ifndef AMDSCENTRALSERVERSGMPV_H
#define AMDSCENTRALSERVERSGMPV_H

#include <QObject>

#include "application/AMDSCentralServer.h"
#include "DataHolder/AMDSDataHolder.h"

class AMDSPVConfigurationMap;
class AMDSPVControllerManager;

#define AMDS_SGM_SERVER_ALT_INVALID_BUFFERGROUP_NAME 30101

class AMDSCentralServerSGMPV : public AMDSCentralServer
{
	Q_OBJECT
public:
	/// Constructor: to initialize the TCP Data server thread and the timers for buffer groups
	AMDSCentralServerSGMPV(QObject *parent = 0);
	~AMDSCentralServerSGMPV();

signals:

protected slots:
	/// slot to handle the new data of a given buffer group
	void onNewPVDataReceived(const QString &bufferName, AMDSDataHolder *newData);

protected:
	/// function to initialize the system configurations
	void initializeConfiguration();
	/// function to initialize the buffer groups, with the given buffer size, by default we will host 10 hours of 1kHz signal
	virtual void initializeBufferGroup();
	/// function to initialize the detector manager
	virtual void initializeDetectorManager();
	/// function to initialize the data server to update the buffer groups
	virtual void initializeAndStartDetectorServer();
	/// function to finalize the initialization
	virtual void wrappingUpInitialization();

	/// helper function to fill the configuration commands for the given bufferName
	void fillConfigurationCommandForClientRequest(const QString &bufferName, AMDSClientConfigurationRequest *clientRequest);

protected:
	/// the pv configruation map
	QList<AMDSPVConfigurationMap*> pvConfigurationMaps_;
	/// the pv controller manager
	AMDSPVControllerManager *pvControllerManager_;

//	/// the Scaler detector manager
//	AMDSScalerDetectorManager *scalerDetectorManager_;
//	/// the scaler server manager
//	AMDSDetectorServerManager *scalerDetectorServerManager_;
};

#endif // AMDSCENTRALSERVERSGMPV_H
