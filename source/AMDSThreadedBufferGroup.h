#ifndef AMDSTHREADEDBUFFERGROUP_H
#define AMDSTHREADEDBUFFERGROUP_H

#include <QObject>
#include <QThread>

#include "source/AMDSBufferGroup.h"

class AMDSThreadedBufferGroup : public QObject
{
Q_OBJECT
public:
	AMDSThreadedBufferGroup(AMDSBufferGroup *bufferGroup, QObject *parent = 0);

	AMDSBufferGroupInfo bufferGroupInfo() const;

signals:
	void bufferGroupReady();

protected slots:
	void onBufferGroupThreadStarted();

protected:
	AMDSBufferGroup *bufferGroup_;

	QThread *bufferGroupThread_;
	mutable QReadWriteLock lock_;
};

#endif // AMDSTHREADEDBUFFERGROUP_H
