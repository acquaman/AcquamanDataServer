#ifndef AMDSSCALERDETECTORMANAGER_H
#define AMDSSCALERDETECTORMANAGER_H

#include <QObject>
#include <QThread>

class AMDSScalerConfigurationMap;
class AMDSScalerDetector;

class AMDSScalerDetectorManager : public QObject
{
    Q_OBJECT
public:
	explicit AMDSScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent = 0);
	~AMDSScalerDetectorManager();

	/// returns the instance of scaler detector
	inline AMDSScalerDetector *scalerDetector() const { return scalerDetector_; }

protected:
	/// the instance of detector manager thread
	QThread *detectorManagerThread_;
	/// the instance of scaler detector
	AMDSScalerDetector *scalerDetector_;
};

#endif // AMDSSCALERDETECTORMANAGER_H
