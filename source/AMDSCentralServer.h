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
	AMDSCentralServer(QString hwType, QObject *parent = 0);

signals:
	void clientRequestProcessed(AMDSClientRequest *processedRequest);

protected slots:
	void onDataServerClientRequestReady(AMDSClientRequest *clientRequest);

	void onFiftyMillisecondTimerUpdate();
	void onHundredMillisecondTimerUpdate();

protected:
	AMDSThreadedTcpDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;

	AMDSBufferGroup *energyBufferGroup_;
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;

	AMDSBufferGroup *amptek1BufferGroup_;
	quint64 spectralCounter_;
	QTimer *hundredMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_H
