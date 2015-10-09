#ifndef AMDSTHREADEDTCPDATASERVER_H
#define AMDSTHREADEDTCPDATASERVER_H

#include <QObject>
#include <QThread>

#include "Connection/AMDSTCPDataServer.h"

class AMDSClientRequest;

/**
  * Class which maintains the TCPServer in a separate thread, and passes signal messages to it.
  */
class AMDSThreadedTCPDataServer : public QObject
{
	Q_OBJECT
public:
	/// Constructs a TCP server object, and passes it to a new thread
	explicit AMDSThreadedTCPDataServer(QObject *parent = 0);
	/// Destructor
	~AMDSThreadedTCPDataServer();

signals:
	/// Starts the server listening on the given interfaceName and port
	void startServer(const QString& interfaceName, quint16 port);

	/// (forwarded from AMDSTCPDataServer) error signal
	void error(quint8 errorLevel, quint16 errorCode, const QString &errorString);
	///  (forwarded from AMDSTCPDataServer) the signal of new client request read from socket
	void clientRequestRead(AMDSClientRequest*);

public slots:
	/// slot to handle the clientRquestProcessed signal, which will ask the AMDSTCPDataServer instance to handle it
	void onClientRequestProcessed(AMDSClientRequest*);

protected slots:
	/// Slot which handles the thread having successfully started
	void onThreadStarted();

private:
	/// the instance of TCPDataServer
	AMDSTCPDataServer *server_;
	/// the instance of QThread
	QThread *thread_;
};

#endif // AMDSTHREADEDTCPDATASERVER_H
