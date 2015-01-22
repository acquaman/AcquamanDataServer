#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class AMDSThreadedBufferGroup;
class AMDSThreadedTcpDataServer;

class AMDSCentralServer : public QObject
{
Q_OBJECT
public:
	AMDSCentralServer(QObject *parent = 0);

protected slots:
	void onDataServerBufferGroupInfoRequested();

protected:
	AMDSThreadedTcpDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;
};

#endif // AMDSCENTRALSERVER_H
