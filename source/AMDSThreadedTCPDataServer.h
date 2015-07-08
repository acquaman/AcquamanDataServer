#ifndef AMDSTHREADEDTCPDATASERVER_H
#define AMDSTHREADEDTCPDATASERVER_H

#include <QObject>
#include <QThread>

#include "source/AMDSTCPDataServer.h"

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
	/// Returns a reference to the server
	AMDSTCPDataServer* server();

signals:
	/// Starts the server listening on the given interfaceName and port
	void startServer(const QString& interfaceName, quint16 port);

	void requestBufferGroupInfo();

protected slots:
	/// Slot which handles the thread having successfully started
	void onThreadStarted();

private:
	AMDSTCPDataServer *server_;
	QThread *thread_;
};

#endif // AMDSTHREADEDTCPDATASERVER_H
