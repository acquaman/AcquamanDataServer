#include "AMDSClientTCPSocket.h"

#include "source/AMDSDataStream.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientTCPSocket::AMDSClientTCPSocket(const QString host, const quint16 port, QObject *parent)
	:QObject(parent)
{
	socketKey_ = "";
	tcpSocket_ = new QTcpSocket(this);

	waitingMorePackages_ = false;
	readedBufferSize_ = 0;
	expectedBufferSize_ = 0;
	incomeDataBuffer_ = new QByteArray();

	connect(tcpSocket_, SIGNAL(readyRead()), this, SLOT(readFortune()));
	connect(tcpSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));

	tcpSocket_->connectToHost(host, port);
}

AMDSClientTCPSocket::~AMDSClientTCPSocket()
{
	tcpSocket_->abort();
	tcpSocket_->close();
}

void AMDSClientTCPSocket::onSocketError(QAbstractSocket::SocketError error)
{
	emit socketError(this, error);
}

void AMDSClientTCPSocket::readFortune()
{
	if (tcpSocket_->bytesAvailable() < (int)sizeof(quint32))
		return;

	AMDSDataStream *inDataStream = new AMDSDataStream(tcpSocket_);
	inDataStream->setVersion(QDataStream::Qt_4_0);

	if (!waitingMorePackages_) {
		*inDataStream >> expectedBufferSize_;

		if (tcpSocket_->bytesAvailable() < expectedBufferSize_) {
			// more data package is expecting, we need to buffer the current ones
			waitingMorePackages_ = true;
			incomeDataBuffer_->clear();

			readedBufferSize_ = tcpSocket_->bytesAvailable();
			incomeDataBuffer_->append(tcpSocket_->readAll());

			return; // finish reading this message, waiting for the future data
		}
	} else {
		// more data package is coming
		readedBufferSize_ += tcpSocket_->bytesAvailable();
		incomeDataBuffer_->append(tcpSocket_->readAll());

		if (readedBufferSize_ == expectedBufferSize_) {
			waitingMorePackages_ = false;

			inDataStream = new AMDSDataStream(incomeDataBuffer_);
		}
	}

	AMDSClientRequest *clientRequest = inDataStream->decodeAndInstantiateClientRequestType();
	inDataStream->read(*clientRequest);

	switch (clientRequest->requestType()) {
	case AMDSClientRequestDefinitions::Introspection:
	case AMDSClientRequestDefinitions::Statistics:
	case AMDSClientRequestDefinitions::StartTimePlusCount:
	case AMDSClientRequestDefinitions::RelativeCountPlusCount:
	case AMDSClientRequestDefinitions::StartTimeToEndTime:
	case AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter:
	case AMDSClientRequestDefinitions::Continuous:
			if (clientRequest->validateResponse())
				socketKey_ = clientRequest->socketKey();
			break;

	default:
		AMDSErrorMon::alert(this, 0, QString("The validateResponse() function is NOT implemented (or called) for %1").arg(clientRequest->requestType()));
		break;
	}

	emit newRequestDataReady(this, clientRequest);
}

void AMDSClientTCPSocket::requestData()
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::Statistics;

	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	sendData(clientRequest);
}

void AMDSClientTCPSocket::requestData(QString &bufferName)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::Introspection;

	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
	if(clientIntrospectionRequest) {
		clientIntrospectionRequest->setBufferName(bufferName);

		sendData(clientIntrospectionRequest);
	}
}

void AMDSClientTCPSocket::requestData(QString &bufferName, QDateTime &startTime, quint64 count)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::StartTimePlusCount;

	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientStartTimePlusCountDataRequest *clientStartTimePlusCountDataRequest = qobject_cast<AMDSClientStartTimePlusCountDataRequest*>(clientRequest);
	if(clientStartTimePlusCountDataRequest){
		clientStartTimePlusCountDataRequest->setBufferName(bufferName);
		clientStartTimePlusCountDataRequest->setStartTime(startTime);
		clientStartTimePlusCountDataRequest->setCount(count);

		sendData(clientStartTimePlusCountDataRequest);
	}
}

void AMDSClientTCPSocket::requestData(QString &bufferName, quint64 relativeCount, quint64 count)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::RelativeCountPlusCount;
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientRelativeCountPlusCountDataRequest *clientRelativeCountPlusCountDataRequest = qobject_cast<AMDSClientRelativeCountPlusCountDataRequest*>(clientRequest);
	if(clientRelativeCountPlusCountDataRequest){
		clientRelativeCountPlusCountDataRequest->setBufferName(bufferName);
		clientRelativeCountPlusCountDataRequest->setRelativeCount(relativeCount);
		clientRelativeCountPlusCountDataRequest->setCount(count);

		sendData(clientRelativeCountPlusCountDataRequest);
	}
}

void AMDSClientTCPSocket::requestData(QString &bufferName, QDateTime &startTime, QDateTime &endTime)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::StartTimeToEndTime;
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientStartTimeToEndTimeDataRequest *clientStartTimeToEndTimeDataRequest = qobject_cast<AMDSClientStartTimeToEndTimeDataRequest*>(clientRequest);
	if(clientStartTimeToEndTimeDataRequest){
		clientStartTimeToEndTimeDataRequest->setBufferName(bufferName);
		clientStartTimeToEndTimeDataRequest->setStartTime(startTime);
		clientStartTimeToEndTimeDataRequest->setEndTime(endTime);

		sendData(clientStartTimeToEndTimeDataRequest);
	}
}

void AMDSClientTCPSocket::requestData(QString &bufferName, QDateTime &middleTime, quint64 countBefore, quint64 countAfter)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter;
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest *clientMiddleTimePlusCountBeforeAndAfterDataRequest = qobject_cast<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest*>(clientRequest);
	if(clientMiddleTimePlusCountBeforeAndAfterDataRequest){

		clientMiddleTimePlusCountBeforeAndAfterDataRequest->setBufferName(bufferName);
		clientMiddleTimePlusCountBeforeAndAfterDataRequest->setMiddleTime(middleTime);
		clientMiddleTimePlusCountBeforeAndAfterDataRequest->setCountBefore(countBefore);
		clientMiddleTimePlusCountBeforeAndAfterDataRequest->setCountAfter(countAfter);

		sendData(clientMiddleTimePlusCountBeforeAndAfterDataRequest);
	}
}

void AMDSClientTCPSocket::requestData(QString &bufferName, quint64 updateInterval, QString handShakeSocketKey)
{
	AMDSClientRequestDefinitions::RequestType clientRequestType = AMDSClientRequestDefinitions::Continuous;
	AMDSClientRequest *clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMDSErrorMon::alert(this, 0, QString("AMDSClientTCPSocket::Failed to parse clientRequest for type: %1").arg(clientRequestType));
		return;
	}

	AMDSClientContinuousDataRequest *clientContinuousDataRequest = qobject_cast<AMDSClientContinuousDataRequest*>(clientRequest);
	if(clientContinuousDataRequest){
		clientContinuousDataRequest->setBufferName(bufferName);
		clientContinuousDataRequest->setUpdateInterval(updateInterval);
		if (handShakeSocketKey.length() > 0) {
			clientContinuousDataRequest->setHandShakeSocketKey(handShakeSocketKey);
		}

		sendData(clientContinuousDataRequest);
	}
}

void AMDSClientTCPSocket::sendData(AMDSClientRequest *clientRequest)
{
	QByteArray block;
	AMDSDataStream outDataStream(&block, QIODevice::WriteOnly);
	outDataStream.setVersion(QDataStream::Qt_4_0);
	outDataStream << (quint16)0;

	outDataStream.encodeClientRequestType(*clientRequest);
	outDataStream.write(*clientRequest);

	outDataStream.device()->seek(0);
	outDataStream << (quint16)(block.size() - sizeof(quint16));
	tcpSocket_->write(block);

	AMDSErrorMon::information(this, 0, QString("Send %1 data to server.").arg(block.size() - sizeof(quint16)));
}

