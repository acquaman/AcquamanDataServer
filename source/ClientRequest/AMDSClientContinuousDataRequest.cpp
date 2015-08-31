#include "AMDSClientContinuousDataRequest.h"

#include "source/AMDSDataStream.h"
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

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QStringList &bufferNames, bool includeStatusData, const quint32 msgUpdateInterval, const QString &msgHandShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::Continuous, responseType, QString(""), includeStatusData, bufferGroupInfo, parent)
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
	bufferNameList_ = names;
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

bool AMDSClientContinuousDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << updateInterval();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << handShakeSocketKey();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	*dataStream << bufferNames();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientContinuousDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;

	quint32 readUpdateInterval;
	*dataStream >> readUpdateInterval;

	QString readHandShakeSocketKey;
	*dataStream >> readHandShakeSocketKey;

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

	return true;
}

bool AMDSClientContinuousDataRequest::startContinuousRequestTimer()
{
	bool messageExpired = isExpired();
	if (messageExpired) {
		setErrorMessage(QString("(msg %1) continuous update expired!").arg(socketKey()));
		AMDSErrorMon::alert(this, 0, errorMessage());
	} else {
		AMDSErrorMon::information(this, 0, QString("(msg %1) update interval: %2!").arg(socketKey()).arg(updateInterval()));
		continuousDataRequestTimer_.singleShot(updateInterval(), this, SLOT(onDataRequestTimerTimeout()));
	}

	return !messageExpired;
}

void AMDSClientContinuousDataRequest::onDataRequestTimerTimeout()
{
	emit sendNewContinuousDataRequest(this);
}
