#ifndef AMPTEKSDD123THREADEDHISTOGRAMGROUP_H
#define AMPTEKSDD123THREADEDHISTOGRAMGROUP_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include "AmptekSDD123Histogram.h"
//class AmptekSDD123Histogram;
class AMDSDataHolder;
class AmptekStatusData;
class AMDSBufferGroup;
class AMDSBufferGroupInfo;

class AmptekSDD123ThreadedHistogramGroup : public QObject
{
	Q_OBJECT
public:
	explicit AmptekSDD123ThreadedHistogramGroup(AMDSBufferGroupInfo bufferGroupInfo, int maxCountSize, QObject *parent = 0);
//	void append(AmptekSDD123Histogram *histogram);
	void append(AMDSDataHolder *histogram);


	void clear();

	int count() const;
signals:
	void dataRequestReady(AMDSClientDataRequest*);
	void requestData(AMDSClientDataRequest*);

public slots:
protected slots:
protected:
private slots:
private:
//	AmptekSDD123BufferedHistogramGroup allData_;
	AMDSBufferGroup *allData_;

	mutable QReadWriteLock lock_;
	QThread thread_;
};

#endif // AMPTEKSDD123THREADEDHISTOGRAMGROUP_H
