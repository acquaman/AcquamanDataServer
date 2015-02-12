#include "AMDSClientDataRequest.h"

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	QObject(parent) {}

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& startTime,
					     int count,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     int amptekIndex,
					     QObject *parent) :
	QObject(parent)
{
	time1_ = startTime;
	count1_ = count;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = StartTimePlusCount;
	socketKey_ = socketKey;
	indexOfAmptek_ = amptekIndex;
}

AMDSClientDataRequest::AMDSClientDataRequest(int relativeCount,
					     int count,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     int amptekIndex,
					     QObject *parent) :
	QObject(parent)
{
	count1_ = relativeCount;
	count2_ = count;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = RelativeCountPlusCount;
	socketKey_ = socketKey;
	indexOfAmptek_ = amptekIndex;
}

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& startTime,
					     const QDateTime& endTime,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     int amptekIndex,
					     QObject* parent) :
	QObject(parent)
{
	time1_ = startTime;
	time2_ = endTime;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = StartTimeToEndTime;
	socketKey_ = socketKey;
	indexOfAmptek_ = amptekIndex;
}

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& middleTime,
					     int countBefore,
					     int countAfter,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     int amptekIndex,
					     QObject* parent) :
	QObject(parent)
{
	time1_ = middleTime;
	count1_ = countBefore;
	count2_ = countAfter;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = MiddleTimePlusCountBeforeAndAfter;
	socketKey_ = socketKey;
	indexOfAmptek_ = amptekIndex;
}

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& lastFetch,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     int amptekIndex,
					     QObject* parent) :
	QObject(parent)
{
	time1_ = lastFetch;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = Continuous;
	socketKey_ = socketKey;
	indexOfAmptek_ = amptekIndex;
	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SIGNAL(sendNewContinuousDataRequest()));
}

AMDSClientDataRequest::AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, QObject *parent) :
	QObject(parent)
{
	responseType_ = responseType;
	socketKey_ = socketKey;
	requestType_ = AMDSClientDataRequest::Introspection;
}

AMDSClientDataRequest::AMDSClientDataRequest(const AMDSClientDataRequest& other) :
	QObject(0)
{
	(*this) = other;
}

QString AMDSClientDataRequest::socketKey() const
{
	return socketKey_;
}

AMDSClientDataRequest::RequestType AMDSClientDataRequest::requestType() const
{
	return requestType_;
}

AMDSClientDataRequest::ResponseType AMDSClientDataRequest::responseType() const
{
	return responseType_;
}

bool AMDSClientDataRequest::includeStatusData() const
{
	return includeStatusData_;
}

QDateTime AMDSClientDataRequest::time1() const
{
	return time1_;
}

QDateTime AMDSClientDataRequest::time2() const
{
	return time2_;
}

int AMDSClientDataRequest::count1() const
{
	return count1_;
}

int AMDSClientDataRequest::count2() const
{
	return count2_;
}

int AMDSClientDataRequest::indexOfAmptek() const
{
	return indexOfAmptek_;
}

//AmptekSDD123DwellHistogramGroup* AMDSClientDataRequest::histogramData()
//{
//    return &histogramData_;
//}

QList<AMDSBufferGroupInfo> AMDSClientDataRequest::bufferGroupInfos() const{
	return bufferGroupInfos_;
}

void AMDSClientDataRequest::setError(const QString &errorMessage)
{
	responseType_ = Error;
	errorMessage_ = errorMessage;
}


AMDSClientDataRequest & AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
{
	this->socketKey_ = other.socketKey_;
	this->requestType_ = other.requestType_;
	this->responseType_ = other.responseType_;
	this->includeStatusData_ = other.includeStatusData_;
	this->time1_ = other.time1_;
	this->time2_ = other.time2_;
	this->count1_ = other.count1_;
	this->count2_ = other.count2_;
//	this->histogramData_ = other.histogramData_;
	this->indexOfAmptek_ = other.indexOfAmptek_;
	this->errorMessage_ = other.errorMessage_;

	return (*this);
}

QString AMDSClientDataRequest::lastError() const
{
	return errorMessage_;
}

void AMDSClientDataRequest::startContinuousRequestTimer(int msecs)
{
	continuousDataRequestTimer_.singleShot(msecs, this, SIGNAL(sendNewContinuousDataRequest()));
}

void AMDSClientDataRequest::setTime1(const QDateTime &time)
{
	time1_ = time;
}

void AMDSClientDataRequest::setRequestType(RequestType requestType){
	requestType_ = requestType;
}

void AMDSClientDataRequest::setResponseType(ResponseType responseType){
	responseType_ = responseType;
}

//void AMDSClientDataRequest::setBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
//	bufferGroupInfo_ = bufferGroupInfo;
//}

void AMDSClientDataRequest::appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
	bufferGroupInfos_.append(bufferGroupInfo);
}

void AMDSClientDataRequest::clearBufferGroupInfos(){
	bufferGroupInfos_.clear();
}
