#include "source/AMDSTCPDataServer.h"

#include <QDateTime>
#include <QTimer>

#include "source/AMDSDataStream.h"
#include "source/AMDSEventDataSupport.h"
#include "source/AMDSEventData.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"

#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "source/DataHolder/AMDSDataHolder.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"

#include "source/util/AMDSErrorMonitor.h"

AMDSTCPDataServer::AMDSTCPDataServer(QObject *parent) :
	QObject(parent)
{
	if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Introspection))
		AMDSClientRequestSupport::registerClass<AMDSClientIntrospectionRequest>(AMDSClientRequestDefinitions::Introspection);
	if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Statistics))
		AMDSClientRequestSupport::registerClass<AMDSClientStatisticsRequest>(AMDSClientRequestDefinitions::Statistics);
	if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::StartTimePlusCount))
		AMDSClientRequestSupport::registerClass<AMDSClientStartTimePlusCountDataRequest>(AMDSClientRequestDefinitions::StartTimePlusCount);
	if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::RelativeCountPlusCount))
		AMDSClientRequestSupport::registerClass<AMDSClientRelativeCountPlusCountDataRequest>(AMDSClientRequestDefinitions::RelativeCountPlusCount);
	if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Continuous))
		AMDSClientRequestSupport::registerClass<AMDSClientContinuousDataRequest>(AMDSClientRequestDefinitions::Continuous);

	if(!AMDSDataHolderSupport::registeredClasses()->contains(AMDSLightWeightScalarDataHolder::staticMetaObject.className()))
		AMDSDataHolderSupport::registerClass<AMDSLightWeightScalarDataHolder>();
	if(!AMDSDataHolderSupport::registeredClasses()->contains(AMDSLightWeightSpectralDataHolder::staticMetaObject.className()))
		AMDSDataHolderSupport::registerClass<AMDSLightWeightSpectralDataHolder>();

	if(!AMDSEventDataSupport::registeredClasses()->contains(AMDSLightWeightEventData::staticMetaObject.className()))
		AMDSEventDataSupport::registerClass<AMDSLightWeightEventData>();

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
		qDebug() << "No clients connected";
	else
	{
		QStringList keys =  clientSockets_.keys();
		for (int iClientKey = 0; iClientKey < keys.count(); ++iClientKey)
		{
			QTcpSocket* currentSocket = clientSockets_.value(keys.at(iClientKey));
			if(currentSocket)
				qDebug() << currentSocket->peerAddress() << ":" << currentSocket->peerPort();
		}
	}

}

void AMDSTCPDataServer::displayDetails()
{
	if(!server_->isListening())
		qDebug() << "Server is failing to listen: " << server_->errorString();
	else
	{
		qDebug() << "Server is listening on " << interfaceName_ << " (" << server_->serverAddress().toString() << server_->serverPort() << ")";
	}

}

void AMDSTCPDataServer::start(const QString &interfaceName, quint16 port)
{
	interfaceName_ = interfaceName;
	port_ = port;

//	qDebug() << "InterfaceName and port " << interfaceName_ << port_;

	clientDisconnectSignalMapper_ = new QSignalMapper(this);
	clientRequestSignalMapper_ = new QSignalMapper(this);
	continuousDataRequestSignalMapper_ = new QSignalMapper(this);
	QNetworkConfigurationManager manager;
	if(manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired)
	{
		QSettings settings;
		settings.beginGroup("Session");
		const QString id = settings.value("DefaultSessionConfig").toString();
		settings.endGroup();

//		qDebug() << "What's the id right now" << id;

		QNetworkConfiguration config = manager.configurationFromIdentifier(id);
		if((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
		{
			config = manager.defaultConfiguration();
		}

		session_ = new QNetworkSession(config, this);
		connect(session_, SIGNAL(opened()), this, SLOT(sessionOpened()));
//		qDebug() << "Trying to open session";
		session_->open();
	}
	else
	{
//		qDebug() << "Just call session opened";
		sessionOpened();
	}
	connect(clientDisconnectSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientDisconnect(QString)));
	connect(clientRequestSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientSentRequest(QString)));
	connect(continuousDataRequestSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onContinuousDataRequestTimer(QString)));
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

//		qDebug() << "Just wrote new client request with block size " << block.size();

		qint64 outboundBytes = block.size();
		tenMillisecondsStats_.setOutboundBytes(tenMillisecondsStats_.outboundBytes()+outboundBytes);
		hundredMillisecondsStats_.setOutboundBytes(hundredMillisecondsStats_.outboundBytes()+outboundBytes);
		oneSecondsStats_.setOutboundBytes(oneSecondsStats_.outboundBytes()+outboundBytes);
		tenSecondsStats_.setOutboundBytes(tenSecondsStats_.outboundBytes()+outboundBytes);

		if(processedRequest->requestType() == AMDSClientRequestDefinitions::StartTimePlusCount){
			AMDSClientStartTimePlusCountDataRequest *processedStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(processedRequest);
			if(processedStartTimePlusCountDataRequest){
				QList<AMDSFlatArray> values;
				for(int x = 0, size = processedStartTimePlusCountDataRequest->data().count(); x < size; x++){
					AMDSFlatArray oneFlatArray;
					processedStartTimePlusCountDataRequest->data().at(x)->data(&oneFlatArray);
					values.append(oneFlatArray);
//					qDebug() << "Data point at " << x << oneFlatArray.vectorDouble().at(0);
					qDebug() << "Data at " << x << oneFlatArray.printData();
				}
			}
		}

		if(processedRequest->requestType() == AMDSClientRequestDefinitions::RelativeCountPlusCount){
			AMDSClientRelativeCountPlusCountDataRequest *processedRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(processedRequest);
			if(processedRelativeCountPlusCountDataRequest){
				QList<AMDSFlatArray> values;
				for(int x = 0, size = processedRelativeCountPlusCountDataRequest->data().count(); x < size; x++){
					AMDSFlatArray oneFlatArray;
					processedRelativeCountPlusCountDataRequest->data().at(x)->data(&oneFlatArray);
					values.append(oneFlatArray);
//					qDebug() << "Data point at " << x << oneFlatArray.vectorDouble().at(0);
					qDebug() << "Data at " << x << oneFlatArray.printData();
				}
			}
		}

		if(processedRequest->requestType() == AMDSClientRequestDefinitions::Continuous && processedRequest->responseType() != AMDSClientRequest::Error)
		{
//			data->setTime1(data->histogramData()->at(data->histogramData()->count() -1)->dwellStartTime());
//			data->histogramData()->clear();

//			processedRequest->startContinuousRequestTimer(500);
		}
		else
		{
			processedRequest->deleteLater();
			requestingSocket->disconnectFromHost();
		}
	}
	else
	{
		processedRequest->deleteLater();
		requestingSocket->disconnectFromHost();
	}
}

void AMDSTCPDataServer::sessionOpened()
{
//	qDebug() << "Session has been opened";
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

//	qDebug() << "Interface name here is " << interfaceName_;

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

//	qDebug() << "We will check against " << interfaceWithoutSuffix;

	QList<QNetworkAddressEntry> associatedAddresses = QNetworkInterface::interfaceFromName(interfaceWithoutSuffix).addressEntries();

	QList<QNetworkAddressEntry> associatedIPV4Addresses;
	for(int x = 0, size = associatedAddresses.count(); x < size; x++){
		qDebug() << "At " << x << associatedAddresses.at(x).ip() << associatedAddresses.at(x).ip().toIPv4Address();
		if(associatedAddresses.at(x).ip().toIPv4Address()) {
			associatedIPV4Addresses.append(associatedAddresses.at(x));
		}
	}

	if(interfaceOffset < associatedAddresses.count())
		address = associatedIPV4Addresses.at(interfaceOffset).ip();
		//address = associatedAddresses.at(interfaceOffset).ip();

	server_ = new QTcpServer(this);
	connect(server_, SIGNAL(newConnection()), this, SLOT(onNewClientConnected()));
	if(!server_->listen(address, port_))
	{
		emit error(AMDSErrorReport::Serious, AMDS_TCPDATASERVER_FAIL_TO_START_SERVER, QString("Unable to start server: %1").arg(server_->errorString()));
		return;
	}

	qDebug() << "Listening on " << server_->serverAddress().toString() << ":" << server_->serverPort();
}


void AMDSTCPDataServer::onNewClientConnected()
{
	QTcpSocket* clientSocket = server_->nextPendingConnection();

	if(clientSocket)
	{
		QString socketDescription = QString("%1:%2").arg(clientSocket->peerAddress().toString()).arg(clientSocket->peerPort());
		qDebug() << "Connection with " << socketDescription << " established";
		clientSockets_.insert(socketDescription, clientSocket);
		connect(clientSocket, SIGNAL(disconnected()), clientDisconnectSignalMapper_, SLOT(map()));
		clientDisconnectSignalMapper_->setMapping(clientSocket, socketDescription);
		connect(clientSocket, SIGNAL(readyRead()), clientRequestSignalMapper_, SLOT(map()));
		clientRequestSignalMapper_->setMapping(clientSocket, socketDescription);
	}
}

void AMDSTCPDataServer::onClientDisconnect(const QString &clientKey)
{
	QTcpSocket* socketToDisconnect = clientSockets_.value(clientKey);

	if(!socketToDisconnect)
		return;

	clientSockets_.remove(clientKey);
	qDebug() << "Disconnection from " << clientKey;
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

	if(clientRequest->requestType() == AMDSClientRequestDefinitions::Statistics){
		AMDSClientStatisticsRequest *clientStatisticsRequest = qobject_cast<AMDSClientStatisticsRequest*>(clientRequest);
		if(clientStatisticsRequest){
			clientStatisticsRequest->appendPacketStats(tenMillisecondsStats_);
			clientStatisticsRequest->appendPacketStats(hundredMillisecondsStats_);
			clientStatisticsRequest->appendPacketStats(oneSecondsStats_);
			clientStatisticsRequest->appendPacketStats(tenSecondsStats_);

			onClientRequestProcessed(clientRequest);
		}
	}
	else
		emit clientRequestRead(clientRequest);
}

void AMDSTCPDataServer::onContinuousDataRequestTimer(const QString &clientKey)
{
	Q_UNUSED(clientKey)

//	ClientDataRequest* request = continuousDataRequests_.value(clientKey, 0);

//	if(request == 0)
//		return;

//	emit requestData(request);

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
