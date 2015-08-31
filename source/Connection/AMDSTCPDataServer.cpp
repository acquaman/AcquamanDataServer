#include "AMDSTCPDataServer.h"

#include <QDateTime>
#include <QTimer>

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/Connection/AMDSDataStream.h"

#include "source/util/AMDSErrorMonitor.h"

AMDSTCPDataServer::AMDSTCPDataServer(QObject *parent) :
	QObject(parent)
{
	session_ = 0;
	server_ = 0;
	port_ = 0;
	clientDisconnectSignalMapper_ = 0;
	clientRequestSignalMapper_ = 0;

	tenMillisecondsStats_.setName("TenMilliseconds");
	hundredMillisecondsStats_.setName("HundredMilliseconds");
	oneSecondsStats_.setName("OneSecond");
	tenSecondsStats_.setName("TenSeconds");

	tenMillisecondStatsTimer_ = new QTimer(this);
	hundredMillisecondStatsTimer_ = new QTimer(this);
	oneSecondStatsTimer_ = new QTimer(this);
	tenSecondStatsTimer_ = new QTimer(this);

	connect(tenMillisecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onTenMillisecondStatsTimerTimeout()));
	connect(hundredMillisecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondStatsTimerTimeout()));
	connect(oneSecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onOneSecondStatsTimerTimeout()));
	connect(tenSecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onTenSecondStatsTimerTimeout()));

	tenMillisecondStatsTimer_->start(10);
	hundredMillisecondStatsTimer_->start(100);
	oneSecondStatsTimer_->start(1000);
	tenSecondStatsTimer_->start(10000);
}

AMDSTCPDataServer::~AMDSTCPDataServer()
{
	stop();
}

void AMDSTCPDataServer::displayClients()
{
	if(clientSockets_.isEmpty())
		AMDSErrorMon::information(this, 0, "No clients connected");
	else
	{
		QStringList keys =  clientSockets_.keys();
		for (int iClientKey = 0; iClientKey < keys.count(); ++iClientKey)
		{
			QTcpSocket* currentSocket = clientSockets_.value(keys.at(iClientKey));
			if(currentSocket)
				AMDSErrorMon::alert(this, 0, QString("%1:%2").arg(currentSocket->peerAddress().toString()).arg(currentSocket->peerPort()));
		}
	}

}

void AMDSTCPDataServer::displayDetails()
{
	if(!server_->isListening())
		AMDSErrorMon::alert(this, 0, QString("Server is failing to listen: %1").arg(server_->errorString()));
	else
	{
		AMDSErrorMon::alert(this, 0, QString("Server is listening on %1 (%2:%3)").arg(interfaceName_).arg(server_->serverAddress().toString()).arg(server_->serverPort()));
	}
}

void AMDSTCPDataServer::start(const QString &interfaceName, quint16 port)
{
	interfaceName_ = interfaceName;
	port_ = port;

	clientDisconnectSignalMapper_ = new QSignalMapper(this);
	clientRequestSignalMapper_ = new QSignalMapper(this);
	QNetworkConfigurationManager manager;
	if(manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
	{
		QSettings settings;
		settings.beginGroup("Session");
		const QString id = settings.value("DefaultSessionConfig").toString();
		settings.endGroup();


		QNetworkConfiguration config = manager.configurationFromIdentifier(id);
		if((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
		{
			config = manager.defaultConfiguration();
		}

		session_ = new QNetworkSession(config, this);
		connect(session_, SIGNAL(opened()), this, SLOT(sessionOpened()));
		session_->open();
	}
	else
	{
		sessionOpened();
	}
	connect(clientDisconnectSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientDisconnect(QString)));
	connect(clientRequestSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientSentRequest(QString)));
}

void AMDSTCPDataServer::stop()
{
	QStringList clientKeys = clientSockets_.keys();

	for(int iClientKey = 0; iClientKey < clientKeys.count(); ++iClientKey)
	{
		QString currentClientKey = clientKeys.at(iClientKey);
		QTcpSocket* clientSocket = clientSockets_.value(currentClientKey);
		if(clientSocket)
		{
			clientSocket->disconnect();
			clientSockets_.remove(currentClientKey);
			clientSocket->deleteLater();
		}
	}

	server_->close();
	if(session_)
		session_->close();

	AMDSErrorMon::information(this, 0, "AMDS TCP Data Server stopped");
}

void AMDSTCPDataServer::onClientRequestProcessed(AMDSClientRequest *processedRequest)
{
	QTcpSocket* requestingSocket = clientSockets_.value(processedRequest->socketKey(), 0);
	if(requestingSocket != 0)
	{
		QByteArray block;
		AMDSDataStream output(&block, QIODevice::WriteOnly);
		output.setVersion(QDataStream::Qt_4_0);
		output << (quint32)0;

		output.encodeClientRequestType(*processedRequest);
		output.write(*processedRequest);

		output.device()->seek(0);
		output << (quint32)(block.size() - sizeof(quint32));
		requestingSocket->write(block);

		qint64 outboundBytes = block.size();
		tenMillisecondsStats_.setOutboundBytes(tenMillisecondsStats_.outboundBytes()+outboundBytes);
		hundredMillisecondsStats_.setOutboundBytes(hundredMillisecondsStats_.outboundBytes()+outboundBytes);
		oneSecondsStats_.setOutboundBytes(oneSecondsStats_.outboundBytes()+outboundBytes);
		tenSecondsStats_.setOutboundBytes(tenSecondsStats_.outboundBytes()+outboundBytes);
	}

	AMDSClientDataRequest *processedClientDataRequest = qobject_cast<AMDSClientDataRequest*>(processedRequest);
	if(processedClientDataRequest) {
		processedClientDataRequest->validateResponse();
	}

	if (!processedClientDataRequest || !processedClientDataRequest->isContinuousMessage()) {
		onClientRequestTaskAccomplished(processedRequest);
	}
}

void AMDSTCPDataServer::disconnectFromHost(QString &socketKey)
{
	QTcpSocket* requestingSocket = clientSockets_.value(socketKey, 0);
	if(requestingSocket)
		requestingSocket->disconnectFromHost();
}

void AMDSTCPDataServer::sessionOpened()
{
	if(session_)
	{
		QNetworkConfiguration config = session_->configuration();
		QString id;
		if(config.type() == QNetworkConfiguration::UserChoice)
			id = session_->sessionProperty("UserChoiceConfiguration").toString();
		else
			id = config.identifier();


		QSettings settings;
		settings.beginGroup("Session");
		settings.setValue("DefaultSessionConfig", id);
		settings.endGroup();
	}

	QHostAddress address;
	QString interfaceWithoutSuffix = interfaceName_;
	int interfaceOffset = 0;

	if(interfaceName_.contains(":"))
	{
		QStringList nameParts = interfaceName_.split(':');
		interfaceWithoutSuffix = nameParts.at(0);
		interfaceOffset = nameParts.at(1).toInt();
		/// Odd as it may seem addresses listed under a range are of the form:
		/// eth0, eth0:0, eth0:1 ... eth0:n
		/// therefore the actual value in the list will be interface offset +1
		interfaceOffset++;
	}

	QList<QNetworkAddressEntry> associatedAddresses = QNetworkInterface::interfaceFromName(interfaceWithoutSuffix).addressEntries();

	QList<QNetworkAddressEntry> associatedIPV4Addresses;
	for(int x = 0, size = associatedAddresses.count(); x < size; x++){
		AMDSErrorMon::debug(this, 0, QString("At %1 %2 %3").arg(x).arg(associatedAddresses.at(x).ip().toString()).arg(associatedAddresses.at(x).ip().toIPv4Address()));
		if(associatedAddresses.at(x).ip().toIPv4Address()) {
			associatedIPV4Addresses.append(associatedAddresses.at(x));
		}
	}

	if(interfaceOffset < associatedAddresses.count())
		address = associatedIPV4Addresses.at(interfaceOffset).ip();

	server_ = new QTcpServer(this);
	connect(server_, SIGNAL(newConnection()), this, SLOT(onNewClientConnected()));
	if(!server_->listen(address, port_))
	{
		emit error(AMDSErrorReport::Serious, AMDS_TCPDATASERVER_FAIL_TO_START_SERVER, QString("Unable to start server: %1").arg(server_->errorString()));
		return;
	}

	AMDSErrorMon::information(this, 0, QString("Listening on %1:%2").arg(server_->serverAddress().toString()).arg(server_->serverPort()));
}


void AMDSTCPDataServer::onNewClientConnected()
{
	QTcpSocket* clientSocket = server_->nextPendingConnection();

	if(clientSocket)
	{
		QString socketDescription = QString("%1:%2").arg(clientSocket->peerAddress().toString()).arg(clientSocket->peerPort());
		AMDSErrorMon::information(this, 0, QString("Connection with %1 established").arg(socketDescription));
		clientSockets_.insert(socketDescription, clientSocket);

		clientDisconnectSignalMapper_->setMapping(clientSocket, socketDescription);
		clientRequestSignalMapper_->setMapping(clientSocket, socketDescription);
		connect(clientSocket, SIGNAL(disconnected()), clientDisconnectSignalMapper_, SLOT(map()));
		connect(clientSocket, SIGNAL(readyRead()), clientRequestSignalMapper_, SLOT(map()));
	}
}

void AMDSTCPDataServer::onClientDisconnect(const QString &clientKey)
{
	QTcpSocket* socketToDisconnect = clientSockets_.value(clientKey);
	if(!socketToDisconnect)
		return;

	clientSockets_.remove(clientKey);
	AMDSErrorMon::information(this, 0, QString("Disconnection from %1").arg(clientKey));

	clientDisconnectSignalMapper_->removeMappings(socketToDisconnect);
	clientRequestSignalMapper_->removeMappings(socketToDisconnect);
	disconnect(socketToDisconnect, SIGNAL(disconnected()), clientDisconnectSignalMapper_, SLOT(map()));
	disconnect(socketToDisconnect, SIGNAL(readyRead()), clientRequestSignalMapper_, SLOT(map()));

	socketToDisconnect->deleteLater();
}

void AMDSTCPDataServer::onClientSentRequest(const QString &clientKey)
{

	QTcpSocket* requestingSocket = clientSockets_.value(clientKey, 0);

	if(requestingSocket == 0)
		return;

	AMDSDataStream incoming(requestingSocket);

	incoming.setVersion(QDataStream::Qt_4_0);

	qint16 blockSize = 0;
	// if clientSocketDataInProgress contains a reference to this client, then we've tried to read
	// the data before, but couldn't because it wasn't fully delivered yet. If so we obtain the block
	// size from the clientSocketDataInProgress hash
	if(clientSocketDataInProgress_.contains(clientKey))
	{
		blockSize = clientSocketDataInProgress_.value(clientKey);
	}
	else
	{
		// if clientSocketDataInProgress does not contain a reference to the client, then this is the first
		// time we've tried to read data for this request. In this case either we have the necessary number
		// of bytes to get an accurate sense of the full block size, or we'll have to wait (in which case we return)
		if(requestingSocket->bytesAvailable() < (int)sizeof(quint16))
			return;
		incoming >> blockSize;
	}


	// At this stage enough bytes have come through in order to determine the size of the full data block, but not necessarily
	// the full block itself. If we don't have all the data we add a reference to the request into clientSocketDataInProgress
	// containing the size of the block, keyed on the clientKey (ip:port) and wait until more comes through. Otherwise the full
	// block is available, and we remove any potential entries in the key and progress to processing the request.
	if(requestingSocket->bytesAvailable() < blockSize)
	{
		clientSocketDataInProgress_.insert(clientKey, blockSize);
		return;
	}
	clientSocketDataInProgress_.remove(clientKey);

	qint64 inboundBytes = requestingSocket->bytesAvailable();
	tenMillisecondsStats_.setInboundBytes(tenMillisecondsStats_.inboundBytes()+inboundBytes);
	hundredMillisecondsStats_.setInboundBytes(hundredMillisecondsStats_.inboundBytes()+inboundBytes);
	oneSecondsStats_.setInboundBytes(oneSecondsStats_.inboundBytes()+inboundBytes);
	tenSecondsStats_.setInboundBytes(tenSecondsStats_.inboundBytes()+inboundBytes);

	AMDSClientRequest *clientRequest = incoming.decodeAndInstantiateClientRequestType();
	incoming.read(*clientRequest);

	clientRequest->setSocketKey(clientKey);

	if(clientRequest->isStatisticsMessage()){
		onClientStaticsRequestReceived(clientRequest);
	} else if (clientRequest->isContinuousMessage()) {
		onClientContinuousRequestReceived(clientRequest);
	} else {
		emit clientRequestRead(clientRequest);
	}
}

void AMDSTCPDataServer::onClientStaticsRequestReceived(AMDSClientRequest *clientRequest)
{
	AMDSClientStatisticsRequest *clientStatisticsRequest = qobject_cast<AMDSClientStatisticsRequest*>(clientRequest);
	if(clientStatisticsRequest){
		clientStatisticsRequest->appendPacketStats(tenMillisecondsStats_);
		clientStatisticsRequest->appendPacketStats(hundredMillisecondsStats_);
		clientStatisticsRequest->appendPacketStats(oneSecondsStats_);
		clientStatisticsRequest->appendPacketStats(tenSecondsStats_);

		onClientRequestProcessed(clientRequest);
	}
}

void AMDSTCPDataServer::onClientContinuousRequestReceived(AMDSClientRequest *clientRequest)
{
	AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
	if(clientContinuousDataRequest){
		if (clientContinuousDataRequest->isHandShakingMessage()) {
			QString socketKey = clientContinuousDataRequest->handShakeSocketKey();
			AMDSClientContinuousDataRequest *activeContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(activeContinuousDataRequestList_.value(socketKey));
			if (activeContinuousDataRequest) {
				AMDSErrorMon::information(this, 0, QString("Hand shaking with message (%1)").arg(socketKey));
				activeContinuousDataRequest->handShaking(clientContinuousDataRequest);
			} else {
				AMDSErrorMon::alert(this, 0, QString("Didn't find hand shaking message (%1)").arg(socketKey));
			}

			onClientRequestTaskAccomplished(clientContinuousDataRequest);
		} else {
			activeContinuousDataRequestList_.insert(clientContinuousDataRequest->socketKey(), clientContinuousDataRequest);
			connect(clientContinuousDataRequest, SIGNAL(sendNewContinuousDataRequest(AMDSClientRequest*)), this, SIGNAL(clientRequestRead(AMDSClientRequest*)));
			connect(clientContinuousDataRequest, SIGNAL(clientRequestTaskAccomplished(AMDSClientRequest*)), this, SLOT(onClientRequestTaskAccomplished(AMDSClientRequest*)));

			emit clientRequestRead(clientRequest);
		}
	}
}

void AMDSTCPDataServer::onClientRequestTaskAccomplished(AMDSClientRequest *clientRequest)
{
	if (clientRequest) {
		if (clientRequest->isContinuousMessage() && activeContinuousDataRequestList_.contains(clientRequest->socketKey())) {
			disconnect(clientRequest, SIGNAL(sendNewContinuousDataRequest(AMDSClientRequest*)), this, SIGNAL(clientRequestRead(AMDSClientRequest*)));
			disconnect(clientRequest, SIGNAL(clientRequestTaskAccomplished(AMDSClientRequest*)), this, SLOT(onClientRequestTaskAccomplished(AMDSClientRequest*)));

			activeContinuousDataRequestList_.remove(clientRequest->socketKey());
		}

		QString socketKey = clientRequest->socketKey();
		disconnectFromHost(socketKey);
		clientRequest->deleteLater();
	}
}

void AMDSTCPDataServer::onTenMillisecondStatsTimerTimeout(){
	if(tenMillisecondsStats_.inboundBytes() > tenMillisecondsStats_.maxInboundBytes())
		tenMillisecondsStats_.setMaxInboundBytes(tenMillisecondsStats_.inboundBytes());
	if(tenMillisecondsStats_.outboundBytes() > tenMillisecondsStats_.maxOutboundBytes())
		tenMillisecondsStats_.setMaxOutboundBytes(tenMillisecondsStats_.outboundBytes());
	if( (tenMillisecondsStats_.inboundBytes() + tenMillisecondsStats_.outboundBytes()) > tenMillisecondsStats_.maxTotalBytes())
		tenMillisecondsStats_.setMaxTotalBytes(tenMillisecondsStats_.inboundBytes() + tenMillisecondsStats_.outboundBytes());

	tenMillisecondsStats_.setInboundBytes(0);
	tenMillisecondsStats_.setOutboundBytes(0);
}

void AMDSTCPDataServer::onHundredMillisecondStatsTimerTimeout(){
	if(hundredMillisecondsStats_.inboundBytes() > hundredMillisecondsStats_.maxInboundBytes())
		hundredMillisecondsStats_.setMaxInboundBytes(hundredMillisecondsStats_.inboundBytes());
	if(hundredMillisecondsStats_.outboundBytes() > hundredMillisecondsStats_.maxOutboundBytes())
		hundredMillisecondsStats_.setMaxOutboundBytes(hundredMillisecondsStats_.outboundBytes());
	if( (hundredMillisecondsStats_.inboundBytes() + hundredMillisecondsStats_.outboundBytes()) > hundredMillisecondsStats_.maxTotalBytes())
		hundredMillisecondsStats_.setMaxTotalBytes(hundredMillisecondsStats_.inboundBytes() + hundredMillisecondsStats_.outboundBytes());

	hundredMillisecondsStats_.setInboundBytes(0);
	hundredMillisecondsStats_.setOutboundBytes(0);
}

void AMDSTCPDataServer::onOneSecondStatsTimerTimeout(){
	if(oneSecondsStats_.inboundBytes() > oneSecondsStats_.maxInboundBytes())
		oneSecondsStats_.setMaxInboundBytes(oneSecondsStats_.inboundBytes());
	if(oneSecondsStats_.outboundBytes() > oneSecondsStats_.maxOutboundBytes())
		oneSecondsStats_.setMaxOutboundBytes(oneSecondsStats_.outboundBytes());
	if( (oneSecondsStats_.inboundBytes() + oneSecondsStats_.outboundBytes()) > oneSecondsStats_.maxTotalBytes())
		oneSecondsStats_.setMaxTotalBytes(oneSecondsStats_.inboundBytes() + oneSecondsStats_.outboundBytes());

	oneSecondsStats_.setInboundBytes(0);
	oneSecondsStats_.setOutboundBytes(0);
}

void AMDSTCPDataServer::onTenSecondStatsTimerTimeout(){
	if(tenSecondsStats_.inboundBytes() > tenSecondsStats_.maxInboundBytes())
		tenSecondsStats_.setMaxInboundBytes(tenSecondsStats_.inboundBytes());
	if(tenSecondsStats_.outboundBytes() > tenSecondsStats_.maxOutboundBytes())
		tenSecondsStats_.setMaxOutboundBytes(tenSecondsStats_.outboundBytes());
	if( (tenSecondsStats_.inboundBytes() + tenSecondsStats_.outboundBytes()) > tenSecondsStats_.maxTotalBytes())
		tenSecondsStats_.setMaxTotalBytes(tenSecondsStats_.inboundBytes() + tenSecondsStats_.outboundBytes());

	tenSecondsStats_.setInboundBytes(0);
	tenSecondsStats_.setOutboundBytes(0);
}
