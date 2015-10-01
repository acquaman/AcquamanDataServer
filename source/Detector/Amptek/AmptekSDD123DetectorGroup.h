#ifndef AMPTEKSDD123DETECTORGROUP_H
#define AMPTEKSDD123DETECTORGROUP_H

#include <QObject>
#include <QMap>

class AMDSClientDataRequest;
class AmptekSDD123ConfigurationMap;
class AmptekSDD123DetectorManager;

class AmptekSDD123DetectorGroup : public QObject
{
	Q_OBJECT
public:
	AmptekSDD123DetectorGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, quint64 maxCountSize, QObject *parent = 0);

	/// returns the list of AmptekDetectorManager
	QList<AmptekSDD123DetectorManager*> detectorManagers();

signals:
	/// signal to indicate that dataRequest is Ready
	void clientRequestProcessed(AMDSClientDataRequest*);

public slots:
	/// slot to handle the request data event
	void requestData(AMDSClientDataRequest *data);

protected:
	/// the mapping of detector names with detector managers
	QMap<QString, AmptekSDD123DetectorManager*> detectorManagers_;
};

#endif // AMPTEKSDD123DETECTORGROUP_H
