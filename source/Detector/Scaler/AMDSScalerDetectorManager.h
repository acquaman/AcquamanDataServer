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

	inline AMDSScalerDetector *scalerDetector() { return scalerDetector_; }

signals:

public slots:

protected:
	QThread *detectorManagerThread_;
	AMDSScalerDetector *scalerDetector_;
};

#endif // AMDSSCALERDETECTORMANAGER_H
