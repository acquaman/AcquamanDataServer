#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class AMDSThreadedBufferGroup;
class AMDSThreadedTcpDataServer;
class AMDSClientDataRequest;

class AMDSCentralServer : public QObject
{
Q_OBJECT
public:
	AMDSCentralServer(QObject *parent = 0);

signals:
	void dataRequestReady(AMDSClientDataRequest *dataRequest);

protected slots:
//	void onDataServerBufferGroupInfoRequested();
	void onDataServerDataRequested(AMDSClientDataRequest *dataRequest);

protected:
	AMDSThreadedTcpDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;
};

#endif // AMDSCENTRALSERVER_H
