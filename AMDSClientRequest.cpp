#include "AMDSClientRequest.h"

#include "AMDSDataStream.h"

#include <QDebug>

AMDSClientRequest::AMDSClientRequest(QObject *parent) :
	QObject(parent)
{
	socketKey_ = QString();
	errorMessage_ = QString();
	requestType_ = AMDSClientRequest::InvalidRequest;
	responseType_ = AMDSClientRequest::InvalidResponse;
}

AMDSClientRequest::AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent) :
	QObject(parent)
{
	socketKey_ = socketKey;
	errorMessage_ = errorMessage;
	requestType_ = requestType;
	responseType_ = responseType;
}

AMDSClientRequest::~AMDSClientRequest()
{
}

AMDSClientRequest::AMDSClientRequest(const AMDSClientRequest &other) :
	QObject(0)
{
	(*this) = other;
}

AMDSClientRequest& AMDSClientRequest::operator =(const AMDSClientRequest &other)
{
	if(this != &other){
		socketKey_ = other.socketKey();
		errorMessage_ = other.errorMessage();
		requestType_ = other.requestType();
		responseType_ = other.responseType();
	}
	return (*this);
}

bool AMDSClientRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientRequest::writeToDataStream";
//	*dataStream << (quint8)requestType_;
//	if(dataStream->status() != QDataStream::Ok)
//		return false;
	*dataStream << socketKey_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << errorMessage_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << (quint8)responseType_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientRequest::readFromDataStream";
//	quint8 readRequestType;
	QString readSocketKey;
	QString readErrorMessage;
	quint8 readResponseType;

//	dataStream >> readRequestType;
//	if(dataStream->status() != QDataStream::Ok)
//		return false;
	*dataStream >> readSocketKey;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readErrorMessage;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readResponseType;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setSocketKey(readSocketKey);
	setErrorMessage(readErrorMessage);
	setResponseType((AMDSClientRequest::ResponseType)readResponseType);

	return true;
}

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	bufferName_ = "Invalid";
	includeStatusData_ = false;
	bufferGroupInfo_.setName("Invalid");
}

AMDSClientDataRequest::AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QString &bufferName, bool includeStatusData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientRequest(socketKey, errorMessage, requestType, responseType, parent)
{
	bufferName_ = bufferName;
	includeStatusData_ = includeStatusData;
	bufferGroupInfo_ = bufferGroupInfo;
}

AMDSClientDataRequest::~AMDSClientDataRequest()
{
}

AMDSClientDataRequest::AMDSClientDataRequest(const AMDSClientDataRequest &other) :
	AMDSClientRequest(0)
{
	(*this) = other;
}

AMDSClientDataRequest& AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);
		bufferName_ = other.bufferName();
		includeStatusData_ = other.includeStatusData();
		bufferGroupInfo_ = other.bufferGroupInfo_;

		clearData();
		for(int x = 0, size = other.data().count(); x < size; x++)
			appendData(other.data().at(x));
	}
	return (*this);
}

bool AMDSClientDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientDataRequest::writeToDataStream";
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientDataRequest::writeToDataStream";

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << includeStatusData_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	bool includeData;
	if(bufferGroupInfo_.name() == "Invalid"){
		includeData = false;
		*dataStream << includeData;
		if(dataStream->status() != QDataStream::Ok)
			return false;
	}
	else{
		includeData = true;
		*dataStream << includeData;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		dataStream->write(bufferGroupInfo_);
		quint16 dataCount = data_.count();
		*dataStream << dataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		// do some data writing here
	}

	return true;
}

bool AMDSClientDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientDataRequest::readFromDataStream";
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientDataRequest::readFromDataStream";

	QString readBufferName;
	bool readIncludeStatusData;
	bool readIncludeData;
	AMDSBufferGroupInfo readBufferGroupInfo;
	quint16 readDataCount;
	QList<AMDSDataHolder*> readDataHolder;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readIncludeStatusData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readIncludeData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	if(readIncludeData){
		dataStream->read(readBufferGroupInfo);
		*dataStream >> readDataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		// do some data reading here
	}

	setBufferName(readBufferName);
	setIncludeStatusData(readIncludeStatusData);
	if(readIncludeData){
		setBufferGroupInfo(readBufferGroupInfo);
		// do some data setting here
	}

	return true;
}

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	requestType_ = AMDSClientRequest::Introspection;

	bufferName_ = "Invalid";
}

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequest::Introspection, responseType, parent)
{
	bufferName_ = bufferName;
}

AMDSClientIntrospectionRequest::~AMDSClientIntrospectionRequest()
{
}

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(const AMDSClientIntrospectionRequest &other) :
	AMDSClientRequest(0)
{
	(*this) = other;
}

AMDSClientIntrospectionRequest& AMDSClientIntrospectionRequest::operator =(const AMDSClientIntrospectionRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);
		bufferName_ = other.bufferName();

		clearBufferGroupInfos();
		for(int x = 0, size = other.bufferGroupInfos().count(); x < size; x++)
			appendBufferGroupInfo(other.bufferGroupInfos().at(x));
	}
	return (*this);
}

bool AMDSClientIntrospectionRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientIntrospectionRequest::writeToDataStream";
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientIntrospectionRequest::writeToDataStream";

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	quint16 bufferGroupInfosCount = bufferGroupInfos_.count();
	*dataStream << bufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = bufferGroupInfos_.count(); x < size; x++)
		dataStream->write(bufferGroupInfos_.at(x));

	return true;
}

bool AMDSClientIntrospectionRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientIntrospectionRequest::readFromDataStream";
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientIntrospectionRequest::readFromDataStream";

	QString readBufferName;
	quint16 readBufferGroupInfosCount;
	QList<AMDSBufferGroupInfo> readBufferGroupInfos;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readBufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = readBufferGroupInfosCount; x < size; x++){
		AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
		dataStream->read(oneBufferGroupInfo);
		if(oneBufferGroupInfo.name() == "Invalid")
			return false;
		readBufferGroupInfos.append(oneBufferGroupInfo);
	}

	setBufferName(readBufferName);
	for(int x = 0, size = readBufferGroupInfos.count(); x < size; x++)
		appendBufferGroupInfo(readBufferGroupInfos.at(x));

	return true;
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	requestType_ = AMDSClientRequest::Statistics;
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(AMDSClientRequest::ResponseType responseType, const QString socketKey, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequest::Statistics, responseType, parent)
{

}

AMDSClientStatisticsRequest::~AMDSClientStatisticsRequest()
{
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(const AMDSClientStatisticsRequest &other) :
	AMDSClientRequest(0)
{
	(*this) = other;
}

AMDSClientStatisticsRequest& AMDSClientStatisticsRequest::operator =(const AMDSClientStatisticsRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		clearPacketStats();
		for(int x = 0, size = other.packetStats().count(); x < size; x++)
			appendPacketStats(other.packetStats().at(x));
	}
	return (*this);
}

bool AMDSClientStatisticsRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientStatisticsRequest::writeToDataStream";
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStatisticsRequest::writeToDataStream";

	*dataStream << (quint32)packetStats_.count();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = packetStats().count(); x < size; x++)
		dataStream->write(packetStats().at(x));

	return true;
}

bool AMDSClientStatisticsRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientStatisticsRequest::readFromDataStream";
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStatisticsRequest::readFromDataStream";

	quint32 readPacketStatsCount;
	QList<AMDSPacketStats> readPacketStats;

	*dataStream >> readPacketStatsCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = readPacketStatsCount; x < size; x++){
		AMDSPacketStats onePacketStat("Invalid");
		dataStream->read(onePacketStat);
		if(onePacketStat.name() == "Invalid")
			return false;
		readPacketStats.append(onePacketStat);
	}

	for(int x = 0, size = readPacketStats.count(); x < size; x++)
		appendPacketStats(readPacketStats.at(x));

	return true;
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	requestType_ = AMDSClientRequest::StartTimePlusCount;

	startTime_ = QDateTime();
	count_ = 0;
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequest::StartTimePlusCount, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
{
	startTime_ = startTime;
	count_ = count;
}

AMDSClientStartTimePlusCountDataRequest::~AMDSClientStartTimePlusCountDataRequest()
{
}

AMDSClientStartTimePlusCountDataRequest::AMDSClientStartTimePlusCountDataRequest(const AMDSClientStartTimePlusCountDataRequest &other) :
	AMDSClientDataRequest(0)
{
	(*this) = other;
}

AMDSClientStartTimePlusCountDataRequest& AMDSClientStartTimePlusCountDataRequest::operator =(const AMDSClientStartTimePlusCountDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		startTime_ = other.startTime();
		count_ = other.count();
	}
	return (*this);
}

bool AMDSClientStartTimePlusCountDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientStartTimePlusCountDataRequest::writeToDataStream";
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStartTimePlusCountDataRequest::writeToDataStream";

	*dataStream << startTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << count_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientStartTimePlusCountDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientStartTimePlusCountDataRequest::readFromDataStream";
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStartTimePlusCountDataRequest::readFromDataStream";

	QDateTime readStartTime;
	quint64 readCount;

	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setStartTime(readStartTime);
	setCount(readCount);

	return true;
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(QObject *parent) :
	AMDSClientDataRequest(parent)
{
	requestType_ = AMDSClientRequest::Continuous;

	startTime_ = QDateTime();
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientDataRequest(socketKey, QString(), AMDSClientRequest::StartTimePlusCount, responseType, bufferName, includeStatusData, bufferGroupInfo, parent)
{
	startTime_ = startTime;
}

AMDSClientContinuousDataRequest::~AMDSClientContinuousDataRequest()
{
}

AMDSClientContinuousDataRequest::AMDSClientContinuousDataRequest(const AMDSClientContinuousDataRequest &other) :
	AMDSClientDataRequest(0)
{
	(*this) = other;
}

AMDSClientContinuousDataRequest& AMDSClientContinuousDataRequest::operator =(const AMDSClientContinuousDataRequest &other)
{
	if(this != &other){
		AMDSClientDataRequest::operator =(other);
		startTime_ = other.startTime();
	}
	return (*this);
}

bool AMDSClientContinuousDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientContinuousDataRequest::writeToDataStream";
	if(!AMDSClientDataRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientContinuousDataRequest::writeToDataStream";

	*dataStream << startTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientContinuousDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientContinuousDataRequest::readFromDataStream";
	if(!AMDSClientDataRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientContinuousDataRequest::readFromDataStream";

	QDateTime readStartTime;
	*dataStream >> readStartTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setStartTime(readStartTime);

	return true;
}
