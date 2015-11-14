#include "AMDSClientTCPSocket.h"

#include <QDataStream>

#include "ClientRequest/AMDSClientRequest.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDSRunTimeSupport.h"

AMDSClientTCPSocket::AMDSClientTCPSocket(const QString host, const quint16 port, QObject *parent)
	:QObject(parent)
{
	hostName_ = host;
	port_ = port;

	socketKey_ = "";
	tcpSocket_ = new QTcpSocket(this);

	waitingMorePackages_ = false;
	readedBufferSize_ = 0;
	expectedBufferSize_ = 0;

	connect(tcpSocket_, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));
	connect(tcpSocket_, SIGNAL(readyRead()), this, SLOT(readClientRequestMessage()));
	connect(tcpSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));

	tcpSocket_->connectToHost(host, port);
}

AMDSClientTCPSocket::~AMDSClientTCPSocket()
{
	incomeDataBuffer_.clear();
	tcpSocket_->disconnectFromHost();
	tcpSocket_->close();
}

void AMDSClientTCPSocket::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
	if (socketState == QAbstractSocket::ConnectedState) {
		if (clientRequestsToSend.count() > 0) {
			foreach (AMDSClientRequest *clientRequest, clientRequestsToSend) {
				sendData(clientRequest);
				clientRequest->deleteLater();
			}

			clientRequestsToSend.clear();
		}
	}
}

void AMDSClientTCPSocket::onSocketError(QAbstractSocket::SocketError error)
{
	emit socketError(this, error);
}

void AMDSClientTCPSocket::readClientRequestMessage()
{
	if (tcpSocket_->bytesAvailable() < (int)sizeof(quint32))
		return;

	QDataStream *inDataStream = 0;
	if (!waitingMorePackages_) {
		inDataStream= new QDataStream(tcpSocket_);
		inDataStream->setVersion(QDataStream::Qt_4_0);
		*inDataStream >> expectedBufferSize_;

		if (tcpSocket_->bytesAvailable() < expectedBufferSize_) {
			//more data package is expecting, we need to buffer the current ones
			waitingMorePackages_ = true;
			incomeDataBuffer_.clear();

			readedBufferSize_ = tcpSocket_->bytesAvailable();
			incomeDataBuffer_.append(tcpSocket_->readAll());
		}
	} else {
		// more data package is coming
		readedBufferSize_ += tcpSocket_->bytesAvailable();
		incomeDataBuffer_.append(tcpSocket_->readAll());

		if (readedBufferSize_ == expectedBufferSize_) {
			waitingMorePackages_ = false;

			inDataStream = new QDataStream(incomeDataBuffer_);
		}
	}

	// finish reading this message, waiting for the future data
	if (waitingMorePackages_ || !inDataStream)
		return;

	AMDSClientRequest *clientRequest = AMDSClientRequest::decodeAndInstantiateClientRequest(inDataStream);
	if (clientRequest == 0)
		return;

	switch (clientRequest->requestType()) {
	case AMDSClientRequestDefinitions::Introspection:
	case AMDSClientRequestDefinitions::Statistics:
	case AMDSClientRequestDefinitions::StartTimePlusCount:
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
	case AMDSClientRequestDefinitions::Continuous:
			if (clientRequest->validateResponse()) {
				socketKey_ = clientRequest->socketKey();
//				clientRequest->printData();
			}
			break;

	default:
		if(AMDSRunTimeSupport::debugAtLevel(1))
			AMErrorMon::alert(this, AMDS_SERVER_ALT_VALIDATE_RESPONSE_NOT_IMPLEMENTED, QString("The validateResponse() function is NOT implemented (or called) for %1").arg(clientRequest->requestType()));
		break;
	}

	emit newRequestDataReady(this, clientRequest);
}

void AMDSClientTCPSocket::sendClientRequest(AMDSClientRequest *clientRequest)
{
	clientRequestsToSend.append(clientRequest);
	onSocketStateChanged(tcpSocket_->state());
}

void AMDSClientTCPSocket::sendData(AMDSClientRequest *clientRequest)
{
	QByteArray block;
	QDataStream outDataStream(&block, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_4_0);
	outDataStream << (quint16)0;

	AMDSClientRequest::encodeAndwriteClientRequest(&outDataStream, clientRequest);

	outDataStream.device()->seek(0);
	outDataStream << (quint16)(block.size() - sizeof(quint16));
	tcpSocket_->write(block);

	if(AMDSRunTimeSupport::debugAtLevel(2))
		AMErrorMon::information(this, AMDS_SERVER_INFO_SOCKET_SEND_DATA, QString("Send %1 data to server.").arg(block.size() - sizeof(quint16)));
}

