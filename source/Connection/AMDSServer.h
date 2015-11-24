#ifndef AMDSSERVER_H
#define AMDSSERVER_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QTcpSocket>

class AMDSClientTCPSocket;
class AMDSClientRequest;

class AMDSServer : public QObject
{
    Q_OBJECT
public:
	/// helper function to generate a host identifier by given hostName and portNumber
	inline static QString generateServerIdentifier(QString hostName, quint16 portNumber) { return QString("%1:%2").arg(hostName).arg(portNumber); }

	explicit AMDSServer(QString hostName, quint16 portNumber, QObject *parent = 0);
	~AMDSServer();

	/// helper function to return the server identifier of the current server
	inline QString serverIdentifier() const { return generateServerIdentifier(hostName_, portNumber_); }
	/// helper function to return the hostName of the current server
	inline QString hostName() const { return hostName_; }
	/// helper function to return the portNumber of the current server
	inline quint16 portNumber() { return portNumber_; }

	/// helper function to return the list of buffers of the current server
	inline QStringList bufferNames() const { return bufferNames_; }
	/// helper function to return the socket keys list of active TCP socket connections (continuous connections)
	inline QStringList activeTCPSocketKeys() const { return QStringList(activeTCPSockets_.keys()); }

	/// helper function to set the list of buffers of the current server
	void setBufferNames(QStringList activeBufferNames);

signals:
	/// signal to indicate that something wrong with the connection happened
	void AMDSServerError(const QString &serverIdentifier, int errorCode, const QString &socketKey, const QString &errorMessage);
	/// signal to indicate that the data is sent from server for read
	void requestDataReady(AMDSClientRequest*);

public slots:
	/// slot to establish a new TCP socket connection
	AMDSClientTCPSocket * establishSocketConnection();

private slots:
	/// slot to handle Socket data ready signal
	void onSocketDataReady(AMDSClientTCPSocket* socket, AMDSClientRequest *clientRequest);
	/// slot to handle Socket error signal
	void onSocketError(AMDSClientTCPSocket *clientTCPSocket, QAbstractSocket::SocketError serverError);
	/// slot to handle the request to remove a given signal
	void removeTCPSocket(AMDSClientTCPSocket *clientTCPSocket);

private:
	/// the host name of the server
	QString hostName_;
	/// the portNumber of the server
	quint16 portNumber_;

	/// the buffers of the server
	QStringList bufferNames_;

	/// the hash table of active server TCP socket connections (for continuous messages)
	QHash<QString, AMDSClientTCPSocket *> activeTCPSockets_;
};

#endif // AMDSSERVER_H
