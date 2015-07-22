#ifndef AMDSCENTRALSERVER_H
#define AMDSCENTRALSERVER_H

#include <QObject>
#include <QMap>

class QTimer;

class AMDSThreadedBufferGroup;
class AMDSThreadedTCPDataServer;
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
	/// Handle the errors information
	void onDataServerErrorHandler(quint8 errorLevel, quint16 errorCode, QString errorMessage);
	void onDataServerClientRequestReady(AMDSClientRequest *clientRequest);

	void onFiftyMillisecondTimerUpdate();
	void onHundredMillisecondTimerUpdate();

protected:
	AMDSThreadedTCPDataServer *dataServer_;
	QMap<QString, AMDSThreadedBufferGroup*> bufferGroups_;

	AMDSBufferGroup *energyBufferGroup_;
	quint64 simpleCounter_;
	QTimer *fiftyMillisecondTimer_;

	AMDSBufferGroup *amptek1BufferGroup_;
	quint64 spectralCounter_;
	QTimer *hundredMillisecondTimer_;
};

#endif // AMDSCENTRALSERVER_H
