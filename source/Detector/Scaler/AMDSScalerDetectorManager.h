#ifndef AMDSSCALERDETECTORMANAGER_H
#define AMDSSCALERDETECTORMANAGER_H

#include <QObject>
#include <QThread>

#include "DataHolder/AMDSDataHolder.h"

class AMDSScalerConfigurationMap;
class AMDSScalerDetector;
class AMDSDwellScalerDetector;
class AMDSScalerDetectorManager;

class AMDSThreadedScalerDetectorManager : public QObject
{
	Q_OBJECT
public:
	explicit AMDSThreadedScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	~AMDSThreadedScalerDetectorManager();

	/// returns the instance of scaler detector manager
	inline AMDSScalerDetectorManager *scalerDetectorManager() const { return scalerDetectorManager_; }

protected:
	/// the instance of detector manager thread
	QThread *detectorManagerThread_;
	/// the instance of scaler detector
	AMDSScalerDetectorManager *scalerDetectorManager_;
};

class AMDSScalerDetectorManager : public QObject
{
    Q_OBJECT

public:
	explicit AMDSScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	~AMDSScalerDetectorManager();

signals:
	/// signal to indicate that the physical scaler detector scan mode changed
	void detectorScanModeChanged(int);
	/// signal to indicate that the physical scaler received new counts data
	void newScalerScanDataReceived(const AMDSDataHolderList &scanCountsDataHolder);

	/// Signal to request flattened data from the central server
	void requestFlattenedData(double seconds);

public slots:
	/// slot to start dwelling
	void onStartScalerDwelling();
	/// slot to stop dwelling
	void onScalerStopDwelling();
	/// slot to enable a given channel
	void onEnableScalerChannel(int channelId);
	/// slot to disable a given channel
	void onDisableScalerChannel(int channelId);

	/// Called to set the flattened data once the request has been processed
	void setFlattenedData(const QVector<double> &data);

protected:
	/// the scaler detector of physical Scaler
	AMDSScalerDetector *scalerDetector_;
	/// the scaler detector of AMDS dwell Scaler
	AMDSDwellScalerDetector *dwellScalerDetector_;
};

#endif // AMDSSCALERDETECTORMANAGER_H
