#ifndef AMDSTHREADEDBUFFERGROUP_H
#define AMDSTHREADEDBUFFERGROUP_H

#include <QObject>
#include <QThread>

class AMDSBufferGroup;

class AMDSThreadedBufferGroup : public QObject
{
Q_OBJECT
public:
	AMDSThreadedBufferGroup(AMDSBufferGroup *bufferGroup, QObject *parent = 0);

	AMDSBufferGroup* bufferGroup();

signals:
	void bufferGroupReady();

protected slots:
	void onBufferGroupThreadStarted();

protected:
	AMDSBufferGroup *bufferGroup_;
	QThread *bufferGroupThread_;
};

#endif // AMDSTHREADEDBUFFERGROUP_H
