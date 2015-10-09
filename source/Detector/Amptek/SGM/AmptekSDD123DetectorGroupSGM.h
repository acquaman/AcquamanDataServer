#ifndef AMPTEKSDD123DETECTORGROUPSGM_H
#define AMPTEKSDD123DETECTORGROUPSGM_H

#include <QObject>
#include <QMap>

class AmptekSDD123ConfigurationMap;
class AmptekSDD123DetectorManager;
class AMDSDataHolder;

class AmptekSDD123DetectorGroupSGM : public QObject
{
	Q_OBJECT
public:
	AmptekSDD123DetectorGroupSGM(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);

	/// returns the list of AmptekDetectorManager
	QList<AmptekSDD123DetectorManager*> detectorManagers();

signals:
	/// the signal to request clear the histrogramData
	void clearHistrogramData(QString detectorName);
	/// the signal to request clear the dwell histrogramData
	void clearDwellHistrogramData(QString detectorName);
	/// the signal to announce the new histrogramData
	void newHistrogramReceived(QString detectorName, AMDSDataHolder *);
	/// the signal to announce the new dwell histrogramData
	void newDwellHistrogramReceived(QString detectorName, AMDSDataHolder * dataHolder, double elapsedDwellTime);
	void dwellFinishedDataUpdate(QString detectorName);

protected:
	/// the mapping of detector names with detector managers
	QMap<QString, AmptekSDD123DetectorManager*> detectorManagers_;
};

#endif // AMPTEKSDD123DETECTORGROUPSGM_H
