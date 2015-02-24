#include "AMDSClientDataRequestV1.h"

AMDSClientDataRequestV1::AMDSClientDataRequestV1(QObject *parent) :
	QObject(parent) {}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(const QDateTime& startTime,
					     quint64 count,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     const QString &bufferName,
					     QObject *parent) :
	QObject(parent)
{
	time1_ = startTime;
	count1_ = count;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = StartTimePlusCount;
	socketKey_ = socketKey;
//	indexOfAmptek_ = amptekIndex;
	bufferName_ = bufferName;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(int relativeCount,
					     quint64 count,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     const QString &bufferName,
					     QObject *parent) :
	QObject(parent)
{
	count1_ = relativeCount;
	count2_ = count;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = RelativeCountPlusCount;
	socketKey_ = socketKey;
//	indexOfAmptek_ = amptekIndex;
	bufferName_ = bufferName;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(const QDateTime& startTime,
					     const QDateTime& endTime,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     const QString &bufferName,
					     QObject* parent) :
	QObject(parent)
{
	time1_ = startTime;
	time2_ = endTime;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = StartTimeToEndTime;
	socketKey_ = socketKey;
//	indexOfAmptek_ = amptekIndex;
	bufferName_ = bufferName;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(const QDateTime& middleTime,
					     quint64 countBefore,
					     quint64 countAfter,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     const QString &bufferName,
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
//	indexOfAmptek_ = amptekIndex;
	bufferName_ = bufferName;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(const QDateTime& lastFetch,
					     bool includeStatusData,
					     ResponseType responseType,
					     const QString& socketKey,
					     const QString &bufferName,
					     QObject* parent) :
	QObject(parent)
{
	time1_ = lastFetch;
	includeStatusData_ = includeStatusData;
	responseType_ = responseType;
	requestType_ = Continuous;
	socketKey_ = socketKey;
//	indexOfAmptek_ = amptekIndex;
	bufferName_ = bufferName;
	connect(&continuousDataRequestTimer_, SIGNAL(timeout()), this, SIGNAL(sendNewContinuousDataRequest()));
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent) :
	QObject(parent)
{
	responseType_ = responseType;
	socketKey_ = socketKey;
	requestType_ = AMDSClientDataRequestV1::Introspection;
	bufferName_ = bufferName;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(ResponseType responseType, const QString &socketKey, QObject *parent) :
	QObject(parent)
{
	responseType_ = responseType;
	socketKey_ = socketKey;
	requestType_ = AMDSClientDataRequestV1::Statistics;
}

AMDSClientDataRequestV1::AMDSClientDataRequestV1(const AMDSClientDataRequestV1& other) :
	QObject(0)
{
	(*this) = other;
}

//QString AMDSClientDataRequestV1::socketKey() const
//{
//	return socketKey_;
//}

//AMDSClientDataRequestV1::RequestType AMDSClientDataRequestV1::requestType() const
//{
//	return requestType_;
//}

//AMDSClientDataRequestV1::ResponseType AMDSClientDataRequestV1::responseType() const
//{
//	return responseType_;
//}

//bool AMDSClientDataRequestV1::includeStatusData() const
//{
//	return includeStatusData_;
//}

//QDateTime AMDSClientDataRequestV1::time1() const
//{
//	return time1_;
//}

//QDateTime AMDSClientDataRequestV1::time2() const
//{
//	return time2_;
//}

//quint64 AMDSClientDataRequestV1::count1() const
//{
//	return count1_;
//}

//quint64 AMDSClientDataRequestV1::count2() const
//{
//	return count2_;
//}

//int AMDSClientDataRequestV1::indexOfAmptek() const
//{
//	return indexOfAmptek_;
//}

//AmptekSDD123DwellHistogramGroup* AMDSClientDataRequestV1::histogramData()
//{
//    return &histogramData_;
//}

//QList<AMDSBufferGroupInfo> AMDSClientDataRequestV1::bufferGroupInfos() const{
//	return bufferGroupInfos_;
//}

//void AMDSClientDataRequestV1::setError(const QString &errorMessage)
//{
//	responseType_ = Error;
//	errorMessage_ = errorMessage;
//}


AMDSClientDataRequestV1 & AMDSClientDataRequestV1::operator =(const AMDSClientDataRequestV1 &other)
{
	socketKey_ = other.socketKey();
	requestType_ = other.requestType();
	responseType_ = other.responseType();
	includeStatusData_ = other.includeStatusData();
	time1_ = other.time1();
	time2_ = other.time2();
	count1_ = other.count1();
	count2_ = other.count2();
//	this->histogramData_ = other.histogramData_;
//	this->indexOfAmptek_ = other.indexOfAmptek_;
	bufferName_ = other.bufferName();
	errorMessage_ = other.errorMessage_;

	return (*this);
}

//QString AMDSClientDataRequestV1::lastError() const
//{
//	return errorMessage_;
//}

void AMDSClientDataRequestV1::startContinuousRequestTimer(int msecs)
{
	continuousDataRequestTimer_.singleShot(msecs, this, SIGNAL(sendNewContinuousDataRequest()));
}

//void AMDSClientDataRequestV1::setTime1(const QDateTime &time)
//{
//	time1_ = time;
//}

//void AMDSClientDataRequestV1::setRequestType(RequestType requestType){
//	requestType_ = requestType;
//}

//void AMDSClientDataRequestV1::setResponseType(ResponseType responseType){
//	responseType_ = responseType;
//}

//void AMDSClientDataRequestV1::setBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
//	bufferGroupInfo_ = bufferGroupInfo;
//}

//void AMDSClientDataRequestV1::appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
//	bufferGroupInfos_.append(bufferGroupInfo);
//}

//void AMDSClientDataRequestV1::clearBufferGroupInfos(){
//	bufferGroupInfos_.clear();
//}
