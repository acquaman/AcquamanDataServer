#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class QTimer;

class AMDSThreadedBufferGroup;
class AMDSThreadedTcpDataServer;
class AMDSBufferGroup;
class AMDSClientRequest;

class AMDSCentralServer : public QObject
{
Q_OBJECT
public:
	AMDSCentralServer(QObject *parent = 0);

signals:
	void clientRequestProcessed(AMDSClientRequest *processedRequest);

protected slots:
	void onDataServerClientRequestReady(AMDSClientRequest *clientRequest);

	void onFiftyMillisecondTimerUpdate();

protected:
	AMDSThreadedTcpDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;

	AMDSBufferGroup *energyBufferGroup_;
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_H
