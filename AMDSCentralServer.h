#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class QTimer;

class AMDSThreadedBufferGroup;
class AMDSThreadedTcpDataServer;
class AMDSClientDataRequestV1;
class AMDSBufferGroup;

class AMDSCentralServer : public QObject
{
Q_OBJECT
public:
	AMDSCentralServer(QObject *parent = 0);

signals:
	void dataRequestReady(AMDSClientDataRequestV1 *dataRequest);

protected slots:
	void onDataServerDataRequested(AMDSClientDataRequestV1 *dataRequest);

	void onFiftyMillisecondTimerUpdate();

protected:
	AMDSThreadedTcpDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;

	AMDSBufferGroup *energyBufferGroup_;
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_H
