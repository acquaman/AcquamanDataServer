#include "AMDSClientDataRequest.h"

AMDSPacketStats::AMDSPacketStats(const QString &name, quint64 inboundBytes, quint64 outboundBytes, quint64 maxInboundBytes, quint64 maxOutboundBytes, quint64 maxTotalBytes)
{
	name_ = name;
	inboundBytes_ = inboundBytes;
	outboundBytes_ = outboundBytes;
	maxInboundBytes_ = maxInboundBytes;
	maxOutboundBytes_ = maxOutboundBytes;
	maxTotalBytes_ = maxTotalBytes;
}

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	QObject(parent) {}

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& startTime,
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

AMDSClientDataRequest::AMDSClientDataRequest(int relativeCount,
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

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& startTime,
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

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& middleTime,
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

AMDSClientDataRequest::AMDSClientDataRequest(const QDateTime& lastFetch,
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

AMDSClientDataRequest::AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent) :
	QObject(parent)
{
	responseType_ = responseType;
	socketKey_ = socketKey;
	requestType_ = AMDSClientDataRequest::Introspection;
	bufferName_ = bufferName;
}

AMDSClientDataRequest::AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, QObject *parent) :
	QObject(parent)
{
	responseType_ = responseType;
	socketKey_ = socketKey;
	requestType_ = AMDSClientDataRequest::Statistics;
}

AMDSClientDataRequest::AMDSClientDataRequest(const AMDSClientDataRequest& other) :
	QObject(0)
{
	(*this) = other;
}

//QString AMDSClientDataRequest::socketKey() const
//{
//	return socketKey_;
//}

//AMDSClientDataRequest::RequestType AMDSClientDataRequest::requestType() const
//{
//	return requestType_;
//}

//AMDSClientDataRequest::ResponseType AMDSClientDataRequest::responseType() const
//{
//	return responseType_;
//}

//bool AMDSClientDataRequest::includeStatusData() const
//{
//	return includeStatusData_;
//}

//QDateTime AMDSClientDataRequest::time1() const
//{
//	return time1_;
//}

//QDateTime AMDSClientDataRequest::time2() const
//{
//	return time2_;
//}

//quint64 AMDSClientDataRequest::count1() const
//{
//	return count1_;
//}

//quint64 AMDSClientDataRequest::count2() const
//{
//	return count2_;
//}

//int AMDSClientDataRequest::indexOfAmptek() const
//{
//	return indexOfAmptek_;
//}

//AmptekSDD123DwellHistogramGroup* AMDSClientDataRequest::histogramData()
//{
//    return &histogramData_;
//}

//QList<AMDSBufferGroupInfo> AMDSClientDataRequest::bufferGroupInfos() const{
//	return bufferGroupInfos_;
//}

//void AMDSClientDataRequest::setError(const QString &errorMessage)
//{
//	responseType_ = Error;
//	errorMessage_ = errorMessage;
//}


AMDSClientDataRequest & AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
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

//QString AMDSClientDataRequest::lastError() const
//{
//	return errorMessage_;
//}

void AMDSClientDataRequest::startContinuousRequestTimer(int msecs)
{
	continuousDataRequestTimer_.singleShot(msecs, this, SIGNAL(sendNewContinuousDataRequest()));
}

//void AMDSClientDataRequest::setTime1(const QDateTime &time)
//{
//	time1_ = time;
//}

//void AMDSClientDataRequest::setRequestType(RequestType requestType){
//	requestType_ = requestType;
//}

//void AMDSClientDataRequest::setResponseType(ResponseType responseType){
//	responseType_ = responseType;
//}

//void AMDSClientDataRequest::setBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
//	bufferGroupInfo_ = bufferGroupInfo;
//}

//void AMDSClientDataRequest::appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo){
//	bufferGroupInfos_.append(bufferGroupInfo);
//}

//void AMDSClientDataRequest::clearBufferGroupInfos(){
//	bufferGroupInfos_.clear();
//}
