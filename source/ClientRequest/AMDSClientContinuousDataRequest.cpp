#include "AMDSClientContinuousDataRequest.h"

#include "source/Connection/AMDSDataStream.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Continuous);
	setBufferName(QString(""));

	setUpdateInterval(500);
	setHandShakeSocketKey("");

	setStartTime(QDateTime::currentDateTime());
	setLastFetchTime(startTime());
	setHandShakeTime(startTime());

	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SLOT(onDataRequestTimerTimeout()));
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QStringList &bufferNames, bool includeStatusData, bool enableFlattening, const quint32 msgUpdateInterval, const QString &msgHandShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::Continuous, responseType, "", includeStatusData, enableFlattening, bufferGroupInfo, parent)
{
	setBufferNames(bufferNames);
	setUpdateInterval(msgUpdateInterval);
	setHandShakeSocketKey(msgHandShakeSocketKey);

	setStartTime(QDateTime::currentDateTime());
	setLastFetchTime(startTime());
	setHandShakeTime(startTime());

	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SLOT(onDataRequestTimerTimeout()));
}

AMDSClientContinuousDataRequest::~AMDSClientContinuousDataRequest()
{
	foreach (QString bufferName, bufferDataRequestList_.keys()) {
		AMDSClientContinuousDataRequest *bufferDataRequest = bufferDataRequestList_.value(bufferName);
		if (bufferDataRequest)
			bufferDataRequest->deleteLater();
	}

	bufferDataRequestList_.clear();
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(const AMDSClientContinuousDataRequest &other) :
	AMDSClientDataRequest(other)
{
	(*this) = other;
}

AMDSClientContinuousDataRequest& AMDSClientContinuousDataRequest::operator =(const AMDSClientContinuousDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);

		setBufferNames(other.bufferNames());
		setBufferName(other.bufferName());

		setUpdateInterval(other.updateInterval());
		setHandShakeSocketKey(other.handShakeSocketKey());

		setStartTime(other.startTime());
		setLastFetchTime(other.lastFetchTime());
		setHandShakeTime(other.lastHandShakingTime());
	}

	return (*this);
}

void AMDSClientContinuousDataRequest::setSocketKey(const QString &socketKey)
{
	AMDSClientDataRequest::setSocketKey(socketKey);

	foreach (QString bufferName, bufferNames()) {
		AMDSClientContinuousDataRequest * dataRequest = bufferDataRequestList_.value(bufferName);
		if (dataRequest)
			dataRequest->setSocketKey(socketKey);
	}
}

void AMDSClientContinuousDataRequest::setBufferNames(const QStringList &names) {
	bufferNameList_.clear();
	bufferNameList_.append(names);
}

bool AMDSClientContinuousDataRequest::isExpired()
{
	QDateTime nowDateTime = QDateTime::currentDateTime();
	quint64 timeSpanInSecond =  nowDateTime.toTime_t() - lastHandShakingTime().toTime_t();

	if (timeSpanInSecond > 60)
		AMDSErrorMon::debug(this, 0, QString("%1::isExpired(): clientKey %2 --- lastHandShake: %3 vs. now: %4")
							.arg(metaObject()->className()).arg(socketKey()).arg(lastHandShakingTime().toString()).arg(nowDateTime.toString()));

	return timeSpanInSecond > 60;
}

int AMDSClientContinuousDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientDataRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << updateInterval();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UPDATE_INTERVAL;

	*dataStream << handShakeSocketKey();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_HANDSHAKE_SOCKET_KEY;

	*dataStream << bufferNames();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAMES;

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientContinuousDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientDataRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	quint32 readUpdateInterval;
	*dataStream >> readUpdateInterval;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UPDATE_INTERVAL;

	QString readHandShakeSocketKey;
	*dataStream >> readHandShakeSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_HANDSHAKE_SOCKET_KEY;

	QStringList readBufferNames;
	*dataStream >> readBufferNames;

	setUpdateInterval(readUpdateInterval);
	setHandShakeSocketKey(readHandShakeSocketKey);
	setBufferNames(readBufferNames);

	foreach (QString bufferName, readBufferNames) {
		AMDSClientContinuousDataRequest *bufferDataRequest = new AMDSClientContinuousDataRequest(*this);
		bufferDataRequest->setBufferName(bufferName);

		bufferDataRequestList_.insert(bufferName, bufferDataRequest);
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

bool AMDSClientContinuousDataRequest::startContinuousRequestTimer()
{
	bool messageExpired = isExpired();
	if (messageExpired) {
		setErrorMessage(QString("(msg %1) continuous update expired!").arg(socketKey()));
		AMDSErrorMon::alert(this, 0, errorMessage());

		emit clientRequestTaskAccomplished(this);
	} else {
		AMDSErrorMon::information(this, 0, QString("(msg %1) update interval: %2!").arg(socketKey()).arg(updateInterval()));
		continuousDataRequestTimer_.singleShot(updateInterval(), this, SLOT(onDataRequestTimerTimeout()));
	}

	return !messageExpired;
}

/**
  Handshaking logic:
	 - if the handShaking message contains no interested buffer name, the corresponding active message will be deregisterred;
	 - if the handShaking message contains buffer names, the active buffer names not in the handshake buffer names will NOT be tracked anymore
	 - if all the active buffername is deactived, the corresponding active message will be forced to be deregisterred;
*/
void AMDSClientContinuousDataRequest::handShaking(AMDSClientContinuousDataRequest *handShakingMessage)
{
	setHandShakeTime(QDateTime::currentDateTime());

	QStringList handShakeBufferNames = handShakingMessage->bufferNames();
	if (handShakeBufferNames.size() == 0) {
		AMDSErrorMon::alert(this, 0, QString("(msg %1): deregistered by request.").arg(socketKey()));
		emit clientRequestTaskAccomplished(this);
	} else {
		foreach (QString bufferName, bufferNames()) {
			if (!handShakeBufferNames.contains(bufferName)) {
				// this buffer name is no longer interested, removed from the active list
				AMDSClientRequest *dataRequest = bufferDataRequestList_.value(bufferName);
				bufferDataRequestList_.remove(bufferName);
				dataRequest->deleteLater();

				bufferNameList_.removeAt(bufferNameList_.indexOf(bufferName));

				AMDSErrorMon::alert(this, 0, QString("(msg %1): buffer (%2) is no longer traced.").arg(socketKey()).arg(bufferName));
			}
		}

		if (bufferDataRequestList_.size() == 0) {
			AMDSErrorMon::alert(this, 0, QString("(msg %1): force-deregistered by request since no more active interested buffer.").arg(socketKey()));
			emit clientRequestTaskAccomplished(this);
		}
	}
}

void AMDSClientContinuousDataRequest::onDataRequestTimerTimeout()
{
	emit sendNewContinuousDataRequest(this);
}
