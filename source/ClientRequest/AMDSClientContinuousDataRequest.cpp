#include "AMDSClientContinuousDataRequest.h"

#include "source/AMDSDataStream.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Continuous);

	setUpdateInterval(500);
	setHandShakeSocketKey("");

	setStartTime(QDateTime::currentDateTime());
	setLastFetchTime(startTime());
	setHandShakeTime(startTime());

	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SLOT(onDataRequestTimerTimeout()));
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const quint32 msgUpdateInterval, const QString &msgHandShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequestDefinitions::Continuous, responseType, bufferName, includeStatusData, flattenResultData, bufferGroupInfo, parent)
{
	setUpdateInterval(msgUpdateInterval);
	setHandShakeSocketKey(msgHandShakeSocketKey);

	setStartTime(QDateTime::currentDateTime());
	setLastFetchTime(startTime());
	setHandShakeTime(startTime());

	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SLOT(onDataRequestTimerTimeout()));
}

AMDSClientContinuousDataRequest::~AMDSClientContinuousDataRequest()
{
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

		setUpdateInterval(other.updateInterval());
		setHandShakeSocketKey(other.handShakeSocketKey());

		setStartTime(other.startTime());
		setLastFetchTime(other.lastFetchTime());
		setHandShakeTime(other.lastHandShakingTime());
	}

	return (*this);
}

bool AMDSClientContinuousDataRequest::isExpired()
{
	QDateTime nowDateTime = QDateTime::currentDateTime();
	quint64 timeSpanInSecond =  nowDateTime.toTime_t() - lastHandShakingTime().toTime_t();

	if (timeSpanInSecond > 60)
		AMDSErrorMon::debug(this, 0, QString("AMDSClientContinuousDataRequest::isExpired(): clientKey %1 --- lastHandShake: %2 vs. now: %3").arg(socketKey()).arg(lastHandShakingTime().toString()).arg(nowDateTime.toString()));

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

	setUpdateInterval(readUpdateInterval);
	setHandShakeSocketKey(readHandShakeSocketKey);

	return true;
}

bool AMDSClientContinuousDataRequest::startContinuousRequestTimer()
{
	bool messageExpired = isExpired();
	if (messageExpired) {
		setErrorMessage("Message (%1:%2) continuous update expired!");
		AMDSErrorMon::alert(this, 0, QString("Message (%1:%2) continuous update expired!").arg(socketKey()).arg(bufferName()));
	} else {
		AMDSErrorMon::information(this, 0, QString("Message (%1) update interval: %2!").arg(socketKey()).arg(updateInterval()));
		continuousDataRequestTimer_.singleShot(updateInterval(), this, SLOT(onDataRequestTimerTimeout()));
	}

	return !messageExpired;
}

void AMDSClientContinuousDataRequest::onDataRequestTimerTimeout()
{
	emit sendNewContinuousDataRequest(this);
}
