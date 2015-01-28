#include "AMDSTcpDataServer.h"

#include "AMDSClientDataRequest.h"

#include <QTime>

#include "AMDSDataStream.h"

AMDSTcpDataServer::AMDSTcpDataServer(QObject *parent) :
	QObject(parent)
{
	session_ = 0;
	server_ = 0;
	port_ = 0;
	clientDisconnectSignalMapper_ = 0;
	clientRequestSignalMapper_ = 0;
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

	QDataStream incoming(requestingSocket);

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

	// Process request
	qint8 requestType;  // see enum ClientDataRequest::RequestType for code
	incoming >> requestType;

	qint8 responseTypeId;   // see enum ClientDataRequest::ResponseType for code
	incoming >> responseTypeId;

	qint8 indexOfAmptek;    // which Amptek we wish to obtain data from
	incoming >> indexOfAmptek;

	bool includeStatusData; // Does the client want us to include the status data in the response
	incoming >> includeStatusData;

	qDebug() << "Received request with " << requestType << responseTypeId << indexOfAmptek << includeStatusData;

	QTime time1;
	QTime time2;
	int count1;
	int count2;

	AMDSClientDataRequest::ResponseType responseRequestType;
	responseRequestType = (AMDSClientDataRequest::ResponseType)responseTypeId;

	if(requestType == 5){
		qDebug() << "Request for introspection";

		AMDSClientDataRequest *introspectionRequest = new AMDSClientDataRequest(responseRequestType, clientKey);
		emit requestData(introspectionRequest);
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








