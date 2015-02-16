#include "AMDSTcpDataServer.h"

#include <QDateTime>
#include <QTimer>

#include "AMDSDataStream.h"

AMDSTcpDataServer::AMDSTcpDataServer(QObject *parent) :
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

	tenMillisecondStatsTimer_ = new QTimer();
	hundredMillisecondStatsTimer_ = new QTimer();
	oneSecondStatsTimer_ = new QTimer();
	tenSecondStatsTimer_ = new QTimer();

	connect(tenMillisecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onTenMillisecondStatsTimerTimeout()));
	connect(hundredMillisecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondStatsTimerTimeout()));
	connect(oneSecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onOneSecondStatsTimerTimeout()));
	connect(tenSecondStatsTimer_, SIGNAL(timeout()), this, SLOT(onTenSecondStatsTimerTimeout()));

	tenMillisecondStatsTimer_->start(10);
	hundredMillisecondStatsTimer_->start(100);
	oneSecondStatsTimer_->start(1000);
	tenSecondStatsTimer_->start(10000);
}

AMDSTcpDataServer::~AMDSTcpDataServer()
{
	stop();
}

void AMDSTcpDataServer::displayClients()
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

void AMDSTcpDataServer::displayDetails()
{
	if(!server_->isListening())
		qDebug() << "Server is failing to listen: " << server_->errorString();
	else
	{
		qDebug() << "Server is listening on " << interfaceName_ << " (" << server_->serverAddress().toString() << server_->serverPort() << ")";
	}

}

void AMDSTcpDataServer::start(const QString &interfaceName, quint16 port)
{
	interfaceName_ = interfaceName;
	port_ = port;

	qDebug() << "InterfaceName and port " << interfaceName_ << port_;

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

		qDebug() << "What's the id right now" << id;

		QNetworkConfiguration config = manager.configurationFromIdentifier(id);
		if((config.state() & QNetworkConfiguration::Discovered) != QNetworkConfiguration::Discovered)
		{
			config = manager.defaultConfiguration();
		}

		session_ = new QNetworkSession(config, this);
		connect(session_, SIGNAL(opened()), this, SLOT(sessionOpened()));
		qDebug() << "Trying to open session";
		session_->open();
	}
	else
	{
		qDebug() << "Just call session opened";
		sessionOpened();
	}
	connect(clientDisconnectSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientDisconnect(QString)));
	connect(clientRequestSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onClientSentRequest(QString)));
	connect(continuousDataRequestSignalMapper_, SIGNAL(mapped(QString)), this, SLOT(onContinuousDataRequestTimer(QString)));
}

void AMDSTcpDataServer::stop()
{
	qDebug() << "Stopping the server...";
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

	qDebug() << "Server stopped";
}


void AMDSTcpDataServer::onDataRequestReady(AMDSClientDataRequest *data)
{
	qDebug() << "Trying to process in onDataRequestReady";

	QTcpSocket* requestingSocket = clientSockets_.value(data->socketKey(), 0);
	if(requestingSocket != 0)
	{
		QByteArray block;
		//QDataStream output(&block, QIODevice::WriteOnly);
		AMDSDataStream output(&block, QIODevice::WriteOnly);
		output.setVersion(QDataStream::Qt_4_0);
		output << (quint32)0;

//		output << (quint8)data->requestType();

		QString outputString;
		if(data->responseType() == AMDSClientDataRequest::Error)
			outputString = data->lastError();
//		else if(data->requestType() == AMDSClientDataRequest::Introspection){
//			qDebug() << "Introspection request";
//			//output.write(data->bufferGroupInfos().at(0).axes().at(0));
//			output << (quint16)data->bufferGroupInfos().count();
//			for(int x = 0, size = data->bufferGroupInfos().count(); x < size; x++)
//				output.write(data->bufferGroupInfos().at(x));
//			outputString.append("Another part of the message");
//		}
		outputString.append("Fake message");

		output.write(*data);

		output << outputString;
		output.device()->seek(0);
		output << (quint32)(block.size() - sizeof(quint32));
		requestingSocket->write(block);

		qint64 outboundBytes = block.size();
		tenMillisecondsStats_.setOutboundBytes(tenMillisecondsStats_.outboundBytes()+outboundBytes);
		hundredMillisecondsStats_.setOutboundBytes(hundredMillisecondsStats_.outboundBytes()+outboundBytes);
		oneSecondsStats_.setOutboundBytes(oneSecondsStats_.outboundBytes()+outboundBytes);
		tenSecondsStats_.setOutboundBytes(tenSecondsStats_.outboundBytes()+outboundBytes);

		if(data->requestType() == AMDSClientDataRequest::StartTimePlusCount){
			qDebug() << "Sending out a start time plus count with data count " << data->data().count();
			QVector<double> values = QVector<double>(data->data().count());
			for(int x = 0, size = data->data().count(); x < size; x++){
				data->data().at(x)->data(values.data()+x);
				qDebug() << "Data point at " << x << values.at(x);
			}
		}

		if(data->requestType() == AMDSClientDataRequest::Continuous && data->responseType() != AMDSClientDataRequest::Error)
		{
//			data->setTime1(data->histogramData()->at(data->histogramData()->count() -1)->dwellStartTime());
//			data->histogramData()->clear();
			data->startContinuousRequestTimer(500);
		}
		else
		{
			data->deleteLater();
			requestingSocket->disconnectFromHost();
		}

//		QByteArray block;
//		QDataStream output(&block, QIODevice::WriteOnly);
//		output.setVersion(QDataStream::Qt_4_0);
//		output << (quint32)0;

//		QString outputString;

//		if(data->responseType() == ClientDataRequest::Error)
//			outputString = data->lastError();
//		else
//		{
//			// Todo: Standardize response mechanism, dependent on data->responseType()
//			for(int iDwell = 0, dwellSize = data->histogramData()->count(); iDwell < dwellSize; iDwell++)
//			{
//				outputString.append(data->histogramData()->at(iDwell)->dwellStartTime().toString("HH:mm:ss.zzz"));
//				outputString.append("\n");
//				for(int iDataPoint = 0, dataPointSize = data->histogramData()->at(iDwell)->spectrum().count(); iDataPoint< dataPointSize; iDataPoint++)
//				{
//					outputString.append(QString("%1").arg(data->histogramData()->at(iDwell)->spectrum().at(iDataPoint)));
//					outputString.append(", ");
//				}
//				outputString.append("\n");
//			}
//		}

//		output << outputString;
//		output.device()->seek(0);
//		output << (quint32)(block.size() - sizeof(quint32));
//		requestingSocket->write(block);
//		if(data->requestType() == ClientDataRequest::Continuous && data->responseType() != ClientDataRequest::Error)
//		{
//			data->setTime1(data->histogramData()->at(data->histogramData()->count() -1)->dwellStartTime());
//			data->histogramData()->clear();
//			data->startContinuousRequestTimer(500);
//		}
//		else
//		{
//			data->deleteLater();
//			requestingSocket->disconnectFromHost();
//		}

	}
	else
	{
		data->deleteLater();
		requestingSocket->disconnectFromHost();
	}
}

void AMDSTcpDataServer::sessionOpened()
{
	qDebug() << "Session has been opened";
	QSettings settings;
	if(session_)
	{
		QNetworkConfiguration config = session_->configuration();
		QString id;
		if(config.type() == QNetworkConfiguration::UserChoice)
			id = session_->sessionProperty("UserChoiceConfiguration").toString();
		else
			id = config.identifier();


		settings.beginGroup("Session");
		settings.setValue("DefaultSessionConfig", id);
		settings.endGroup();
	}

	QHostAddress address;
	QString interfaceWithoutSuffix = interfaceName_;
	int interfaceOffset = 0;

	qDebug() << "Interface name here is " << interfaceName_;

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

	qDebug() << "We will check against " << interfaceWithoutSuffix;

	QList<QNetworkAddressEntry> associatedAddresses = QNetworkInterface::interfaceFromName(interfaceWithoutSuffix).addressEntries();

	QList<QNetworkAddressEntry> associatedIPV4Addresses;
	for(int x = 0, size = associatedAddresses.count(); x < size; x++){
		qDebug() << "At " << x << associatedAddresses.at(x).ip() << associatedAddresses.at(x).ip().toIPv4Address();
		if(associatedAddresses.at(x).ip().toIPv4Address())
			associatedIPV4Addresses.append(associatedAddresses.at(x));
	}

	if(interfaceOffset < associatedAddresses.count())
		address = associatedIPV4Addresses.at(interfaceOffset).ip();
		//address = associatedAddresses.at(interfaceOffset).ip();

	server_ = new QTcpServer(this);
	connect(server_, SIGNAL(newConnection()), this, SLOT(onNewClientConnected()));
	if(!server_->listen(address, port_))
	{
		qDebug() << "Unable to start server: " << server_->errorString();
		return;
	}
	qDebug() << "Listening on " << server_->serverAddress().toString() << ":" << server_->serverPort();
}



void AMDSTcpDataServer::onNewClientConnected()
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

void AMDSTcpDataServer::onClientDisconnect(const QString &clientKey)
{
	QTcpSocket* socketToDisconnect = clientSockets_.value(clientKey);

	if(!socketToDisconnect)
		return;

	clientSockets_.remove(clientKey);
	qDebug() << "Disconnection from " << clientKey;
	socketToDisconnect->deleteLater();
}

void AMDSTcpDataServer::onClientSentRequest(const QString &clientKey)
{
	QTcpSocket* requestingSocket = clientSockets_.value(clientKey, 0);

	if(requestingSocket == 0)
		return;

//	QDataStream incoming(requestingSocket);
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

	AMDSClientDataRequest clientDataRequest;
	qDebug() << "About to read the CDR from stream";
	incoming.read(clientDataRequest);

	// Process request
	quint8 requestType;  // see enum ClientDataRequest::RequestType for code
	incoming >> requestType;

	quint8 responseTypeId;   // see enum ClientDataRequest::ResponseType for code
	incoming >> responseTypeId;

//	quint8 indexOfAmptek;    // which Amptek we wish to obtain data from
//	incoming >> indexOfAmptek;

	QString bufferName;
	incoming >> bufferName;

	bool includeStatusData; // Does the client want us to include the status data in the response
	incoming >> includeStatusData;

//	qDebug() << "Received request with " << requestType << responseTypeId << indexOfAmptek << includeStatusData;
	qDebug() << "Received request with " << requestType << responseTypeId << bufferName << includeStatusData;
	qDebug() << "Versus CDR with " << clientDataRequest.requestType() << clientDataRequest.responseType() << clientDataRequest.bufferName() << clientDataRequest.includeStatusData();

	QTime time1;
	QTime time2;
	int count1;
	int count2;

	AMDSClientDataRequest::ResponseType responseRequestType;
	responseRequestType = (AMDSClientDataRequest::ResponseType)responseTypeId;

	if(requestType == AMDSClientDataRequest::Introspection){
		qDebug() << "Request for introspection";

		AMDSClientDataRequest *introspectionRequest = new AMDSClientDataRequest(responseRequestType, clientKey, bufferName);
		emit requestData(introspectionRequest);
	}
	else if(requestType == AMDSClientDataRequest::Continuous){
		qDebug() << "Request for continuous on " << bufferName;
	}
	else if(requestType == AMDSClientDataRequest::StartTimePlusCount){
		qDebug() << "Request for startTimePlusCount on " << bufferName;

		AMDSClientDataRequest *startTimePlusCountRequest = new AMDSClientDataRequest(clientDataRequest.time1(), clientDataRequest.count1(), clientDataRequest.includeStatusData(), clientDataRequest.responseType(), clientKey, clientDataRequest.bufferName());
		emit requestData(startTimePlusCountRequest);

	}
	else if(requestType == AMDSClientDataRequest::Statistics){
		qDebug() << "Request for statistics";

		AMDSClientDataRequest *statisticsRequest = new AMDSClientDataRequest(responseRequestType, clientKey);
		statisticsRequest->setRequestType(AMDSClientDataRequest::Statistics);
		statisticsRequest->appendPacketStats(tenMillisecondsStats_);
		statisticsRequest->appendPacketStats(hundredMillisecondsStats_);
		statisticsRequest->appendPacketStats(oneSecondsStats_);
		statisticsRequest->appendPacketStats(tenSecondsStats_);

		onDataRequestReady(statisticsRequest);
	}


//	if(requestType == 0)
//	{
//		incoming >> time1;
//		ClientDataRequest* dataRequest = new ClientDataRequest(time1, includeStatusData, responseRequestType, clientKey, indexOfAmptek);
//		continuousDataRequests_.insert(dataRequest->socketKey(), dataRequest);
//		connect(dataRequest, SIGNAL(sendNewContinuousDataRequest()), continuousDataRequestSignalMapper_, SLOT(map()));
//		continuousDataRequestSignalMapper_->setMapping(dataRequest, dataRequest->socketKey());
//		emit requestData(dataRequest);
//	}
//	else if(requestType == 1)
//	{
//		incoming >> time1;
//		incoming >> count1;
//		emit requestData(new ClientDataRequest(time1, count1, includeStatusData, responseRequestType, clientKey, indexOfAmptek));
//	}
//	else if(requestType == 2)
//	{
//		incoming >> count1;
//		incoming >> count2;
//		emit requestData(new ClientDataRequest(count1, count2, includeStatusData, responseRequestType, clientKey, indexOfAmptek));
//	}
//	else if(requestType == 3)
//	{
//		incoming >> time1;
//		incoming >> time2;
//		emit requestData(new ClientDataRequest(time1, time2, includeStatusData, responseRequestType, clientKey, indexOfAmptek));
//	}
//	else if(requestType == 4)
//	{
//		incoming >> time1;
//		incoming >> count1;
//		incoming >> count2;
//		emit requestData(new ClientDataRequest(time1, count1, count2, includeStatusData, responseRequestType, clientKey, indexOfAmptek));
//	}
}

void AMDSTcpDataServer::onContinuousDataRequestTimer(const QString &clientKey)
{
//	ClientDataRequest* request = continuousDataRequests_.value(clientKey, 0);

//	if(request == 0)
//		return;

//	emit requestData(request);

}

void AMDSTcpDataServer::onTenMillisecondStatsTimerTimeout(){
	if(tenMillisecondsStats_.inboundBytes() > tenMillisecondsStats_.maxInboundBytes())
		tenMillisecondsStats_.setMaxInboundBytes(tenMillisecondsStats_.inboundBytes());
	if(tenMillisecondsStats_.outboundBytes() > tenMillisecondsStats_.maxOutboundBytes())
		tenMillisecondsStats_.setMaxOutboundBytes(tenMillisecondsStats_.outboundBytes());
	if( (tenMillisecondsStats_.inboundBytes() + tenMillisecondsStats_.outboundBytes()) > tenMillisecondsStats_.maxTotalBytes())
		tenMillisecondsStats_.setMaxTotalBytes(tenMillisecondsStats_.inboundBytes() + tenMillisecondsStats_.outboundBytes());

	tenMillisecondsStats_.setInboundBytes(0);
	tenMillisecondsStats_.setOutboundBytes(0);
}

void AMDSTcpDataServer::onHundredMillisecondStatsTimerTimeout(){
	if(hundredMillisecondsStats_.inboundBytes() > hundredMillisecondsStats_.maxInboundBytes())
		hundredMillisecondsStats_.setMaxInboundBytes(hundredMillisecondsStats_.inboundBytes());
	if(hundredMillisecondsStats_.outboundBytes() > hundredMillisecondsStats_.maxOutboundBytes())
		hundredMillisecondsStats_.setMaxOutboundBytes(hundredMillisecondsStats_.outboundBytes());
	if( (hundredMillisecondsStats_.inboundBytes() + hundredMillisecondsStats_.outboundBytes()) > hundredMillisecondsStats_.maxTotalBytes())
		hundredMillisecondsStats_.setMaxTotalBytes(hundredMillisecondsStats_.inboundBytes() + hundredMillisecondsStats_.outboundBytes());

	hundredMillisecondsStats_.setInboundBytes(0);
	hundredMillisecondsStats_.setOutboundBytes(0);
}

void AMDSTcpDataServer::onOneSecondStatsTimerTimeout(){
	if(oneSecondsStats_.inboundBytes() > oneSecondsStats_.maxInboundBytes())
		oneSecondsStats_.setMaxInboundBytes(oneSecondsStats_.inboundBytes());
	if(oneSecondsStats_.outboundBytes() > oneSecondsStats_.maxOutboundBytes())
		oneSecondsStats_.setMaxOutboundBytes(oneSecondsStats_.outboundBytes());
	if( (oneSecondsStats_.inboundBytes() + oneSecondsStats_.outboundBytes()) > oneSecondsStats_.maxTotalBytes())
		oneSecondsStats_.setMaxTotalBytes(oneSecondsStats_.inboundBytes() + oneSecondsStats_.outboundBytes());

	oneSecondsStats_.setInboundBytes(0);
	oneSecondsStats_.setOutboundBytes(0);

//	QString updateString;
//	updateString.append(QString("\n10ms: %1").arg(tenMillisecondsStats_.allStats()));
//	updateString.append(QString("\n100ms: %1").arg(hundredMillisecondsStats_.allStats()));
//	updateString.append(QString("\n1s: %1").arg(oneSecondsStats_.allStats()));
//	updateString.append(QString("\n10s: %1").arg(tenSecondsStats_.allStats()));
//	qDebug() << "One second update " << updateString;
}

void AMDSTcpDataServer::onTenSecondStatsTimerTimeout(){
	if(tenSecondsStats_.inboundBytes() > tenSecondsStats_.maxInboundBytes())
		tenSecondsStats_.setMaxInboundBytes(tenSecondsStats_.inboundBytes());
	if(tenSecondsStats_.outboundBytes() > tenSecondsStats_.maxOutboundBytes())
		tenSecondsStats_.setMaxOutboundBytes(tenSecondsStats_.outboundBytes());
	if( (tenSecondsStats_.inboundBytes() + tenSecondsStats_.outboundBytes()) > tenSecondsStats_.maxTotalBytes())
		tenSecondsStats_.setMaxTotalBytes(tenSecondsStats_.inboundBytes() + tenSecondsStats_.outboundBytes());

	tenSecondsStats_.setInboundBytes(0);
	tenSecondsStats_.setOutboundBytes(0);
}
