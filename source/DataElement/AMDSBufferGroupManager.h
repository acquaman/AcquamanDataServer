#ifndef AMDSBUFFERGROUPMANAGER_H
#define AMDSBUFFERGROUPMANAGER_H

#include <QObject>
#include <QThread>

#include "source/DataElement/AMDSBufferGroup.h"

class AMDSBufferGroupManager : public QObject
{
Q_OBJECT
public:
	AMDSBufferGroupManager(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxCountSize, QObject *parent = 0);

	AMDSBufferGroupInfo bufferGroupInfo() const;
	AMDSBufferGroup * bufferGroup() const;

signals:
	void bufferGroupReady();

protected slots:
	void onBufferGroupThreadStarted();

protected:
	AMDSBufferGroup *bufferGroup_;

	QThread *bufferGroupThread_;
	mutable QReadWriteLock lock_;
};

#endif // AMDSBUFFERGROUPMANAGER_H
