#ifndef AMPTEKSDD123DETECTORGROUPSGM_H
#define AMPTEKSDD123DETECTORGROUPSGM_H

#include <QObject>
#include <QMap>
#include <QThread>

class AmptekSDD123ConfigurationMap;
class AmptekSDD123DetectorManager;
class AMDSDataHolder;

class AmptekSDD123DetectorGroupSGM : public QObject
{
	Q_OBJECT
public:
	AmptekSDD123DetectorGroupSGM(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);
	~AmptekSDD123DetectorGroupSGM();

	/// returns the list of AmptekDetectorManager
	QList<AmptekSDD123DetectorManager*> detectorManagers();

protected:
	/// the mapping of detector names with detector managers
	QMap<QString, AmptekSDD123DetectorManager*> detectorManagers_;

	/// the mapping of detector names with detector manager threads
	QMap<QString, QThread*> detectorManagerThreads_;
};

#endif // AMPTEKSDD123DETECTORGROUPSGM_H
