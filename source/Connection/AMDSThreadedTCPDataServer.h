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

	/// returns the TCP server
	inline AMDSTCPDataServer *server() const { return server_; }

signals:
	/// Starts the server listening on the given interfaceName and port
	void startServer(const QString& interfaceName, quint16 port);

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
