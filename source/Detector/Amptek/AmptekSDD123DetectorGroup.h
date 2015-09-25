#ifndef AMPTEKSDD123DETECTORGROUP_H
#define AMPTEKSDD123DETECTORGROUP_H

#include <QObject>
//#include "AmptekSDD123Histogram.h"

class ClientDataRequest;
class AmptekSDD123ConfigurationMap;
class AmptekSDD123DetectorManager;

class AmptekSDD123DetectorGroup : public QObject
{
Q_OBJECT
public:
	AmptekSDD123DetectorGroup(QList<AmptekSDD123ConfigurationMap*> configurationMaps, QObject *parent = 0);

	QList<AmptekSDD123DetectorManager*> detectorManagers();

signals:
	void dataRequestReady(ClientDataRequest*);

//public slots:
//	void requestData(ClientDataRequest *data);

protected:
	QList<AmptekSDD123DetectorManager*> detectorManagers_;
};

#endif // AMPTEKSDD123DETECTORGROUP_H
