#ifndef AMPTEKSDD123THREADEDHISTOGRAMGROUP_H
#define AMPTEKSDD123THREADEDHISTOGRAMGROUP_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include "AmptekSDD123Histogram.h"
class AmptekSDD123Histogram;
class AmptekStatusData;

class AmptekSDD123ThreadedHistogramGroup : public QObject
{
	Q_OBJECT
public:
	explicit AmptekSDD123ThreadedHistogramGroup(int maxBufferSize, QObject *parent = 0);
	void append(AmptekSDD123Histogram *histogram);


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
	AmptekSDD123BufferedHistogramGroup allData_;
	mutable QReadWriteLock lock_;
	QThread thread_;
};

#endif // AMPTEKSDD123THREADEDHISTOGRAMGROUP_H
