#include "AMDSDataStream.h"

#include "ClientRequest/AMDSClientRequestSupport.h"

AMDSDataStream::AMDSDataStream() :
	QDataStream()
{
}

AMDSDataStream::AMDSDataStream(QIODevice *device) :
	QDataStream(device)
{
}

AMDSDataStream::AMDSDataStream(QByteArray *array, QIODevice::OpenMode openMode) :
	QDataStream(array, openMode)
{
}

AMDSDataStream::AMDSDataStream(const QByteArray &array) :
	QDataStream(array)
{
}

AMDSDataStream::~AMDSDataStream()
{
}

void AMDSDataStream::read(AMDSAxisInfo &axisInfo){
	QString name;
	QString description;
	QString units;
	quint32 size;
	bool isUniform;
	qint16 start;
	quint16 increment;

//	qDebug() << "Reading an axisInfo";
	*this >> name;
	if(status() != QDataStream::Ok)
		return;
//	qDebug() << "Named: " << name;
	*this >> description;
	if(status() != QDataStream::Ok)
		return;
	*this >> units;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(size);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(isUniform);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(start);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(increment);
	if(status() != QDataStream::Ok)
		return;

	axisInfo.setName(name);
	axisInfo.setDescription(description);
	axisInfo.setUnits(units);
	axisInfo.setSize(size);
	axisInfo.setIsUniform(isUniform);
	axisInfo.setStart(start);
	axisInfo.setIncrement(increment);
}

void AMDSDataStream::read(AMDSBufferGroupInfo &bufferGroupInfo){
	QString name;
	QString description;
	QString units;
	quint8 axesCount;
	QList<AMDSAxisInfo> axes;

//	qDebug() << "Reading a bufferGroupInfo";
	*this >> name;
	if(status() != QDataStream::Ok)
		return;
//	qDebug() << "Named: " << name;
	*this >> description;
	if(status() != QDataStream::Ok)
		return;
	*this >> units;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(axesCount);
	if(status() != QDataStream::Ok)
		return;

	for(int x = 0, size = axesCount; x < size; x++){
		AMDSAxisInfo oneAxisInfo("Invalid", 0);
		read(oneAxisInfo);
		if(oneAxisInfo.name() == "Invalid")
			return;
		axes.append(oneAxisInfo);
	}

	bufferGroupInfo.setName(name);
	bufferGroupInfo.setDescription(description);
	bufferGroupInfo.setUnits(units);
	bufferGroupInfo.setAxes(axes);
}

void AMDSDataStream::read(AMDSPacketStats &packetStat){
	QString name;
	quint64 inboundBytes;
	quint64 outboundBytes;
	quint64 maxInboundBytes;
	quint64 maxOutboundBytes;
	quint64 maxTotalBytes;

	*this >> name;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(inboundBytes);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(outboundBytes);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(maxInboundBytes);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(maxOutboundBytes);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(maxTotalBytes);
	if(status() != QDataStream::Ok)
		return;

	packetStat.setName(name);
	packetStat.setInboundBytes(inboundBytes);
	packetStat.setOutboundBytes(outboundBytes);
	packetStat.setMaxInboundBytes(maxInboundBytes);
	packetStat.setMaxOutboundBytes(maxOutboundBytes);
	packetStat.setMaxTotalBytes(maxTotalBytes);
}

#include "AMDSScalarDataHolder.h"

//void AMDSDataStream::read(AMDSClientDataRequestV1 &clientDataRequest){
//	QString socketKey;
//	QString errorMessage;
//	quint8 requestType;
//	quint8 responseType;
//	bool includeStatusData;
//	QDateTime time1;
//	QDateTime time2;
//	quint64 count1;
//	quint64 count2;
//	QString bufferName;
//	QList<AMDSBufferGroupInfo> bufferGroupInfos;
//	QList<AMDSPacketStats> packetStats;
//	QList<AMDSDataHolder*> data;
//	quint16 dataCount;
//	QVector<double> totalVector;

//	*this >> socketKey;
//	if(status() != QDataStream::Ok)
//		return;
//	*this >> errorMessage;
//	if(status() != QDataStream::Ok)
//		return;
//	QDataStream::operator >>(requestType);
//	if(status() != QDataStream::Ok)
//		return;
//	QDataStream::operator >>(responseType);
//	if(status() != QDataStream::Ok)
//		return;
//	QDataStream::operator >>(includeStatusData);
//	if(status() != QDataStream::Ok)
//		return;
//	*this >> time1;
//	if(status() != QDataStream::Ok)
//		return;
//	*this >> time2;
//	if(status() != QDataStream::Ok)
//		return;
//	QDataStream::operator >>(count1);
//	if(status() != QDataStream::Ok)
//		return;
//	QDataStream::operator >>(count2);
//	if(status() != QDataStream::Ok)
//		return;
//	*this >> bufferName;
//	if(status() != QDataStream::Ok)
//		return;

//	switch(requestType){
//	case AMDSClientDataRequestV1::Continuous:
//		break;
//	case AMDSClientDataRequestV1::StartTimePlusCount:{

//		quint16 bufferGroupCount;

//		QDataStream::operator >>(bufferGroupCount);
//		if(status() != QDataStream::Ok)
//			return;
//		for(int x = 0, size = bufferGroupCount; x < size; x++){
//			AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
//			read(oneBufferGroupInfo);
//			if(oneBufferGroupInfo.name() == "Invalid")
//				return;
//			bufferGroupInfos.append(oneBufferGroupInfo);
//		}

//		if(bufferGroupInfos.count() > 0){
//			QDataStream::operator >>(dataCount);
//			if(status() != QDataStream::Ok)
//				return;

//			quint64 totalSize = bufferGroupInfos.at(0).spanSize()*dataCount;
//			totalVector = QVector<double>(totalSize);
//			for(quint16 x = 0; x < totalSize; x++)
//				QDataStream::operator >>(totalVector[x]);

//		}

//		break;}
//	case AMDSClientDataRequestV1::RelativeCountPlusCount:
//		break;
//	case AMDSClientDataRequestV1::StartTimeToEndTime:
//		break;
//	case AMDSClientDataRequestV1::MiddleTimePlusCountBeforeAndAfter:
//		break;
//	case AMDSClientDataRequestV1::Introspection:{
//		quint16 bufferGroupCount;

//		QDataStream::operator >>(bufferGroupCount);
//		if(status() != QDataStream::Ok)
//			return;
//		for(int x = 0, size = bufferGroupCount; x < size; x++){
//			AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
//			read(oneBufferGroupInfo);
//			if(oneBufferGroupInfo.name() == "Invalid")
//				return;
//			bufferGroupInfos.append(oneBufferGroupInfo);
//		}
//		break;}
//	case AMDSClientDataRequestV1::Statistics:{
//		quint16 packetStatsCount;

//		QDataStream::operator >>(packetStatsCount);
//		if(status() != QDataStream::Ok)
//			return;
//		for(int x = 0, size = packetStatsCount; x < size; x++){
//			AMDSPacketStats onePacketStat("Invalid");
//			read(onePacketStat);
//			if(onePacketStat.name() == "Invalid")
//				return;
//			packetStats.append(onePacketStat);
//		}
//		break;}
//	default:
//		break;
//	}

//	clientDataRequest.setSocketKey(socketKey);
//	clientDataRequest.setErrorMessage(errorMessage);
//	clientDataRequest.setRequestType((AMDSClientDataRequestV1::RequestType)requestType);
//	clientDataRequest.setResponseType((AMDSClientDataRequestV1::ResponseType)responseType);
//	clientDataRequest.setIncludeStatusData(includeStatusData);
//	clientDataRequest.setTime1(time1);
//	clientDataRequest.setTime2(time2);
//	clientDataRequest.setCount1(count1);
//	clientDataRequest.setCount2(count2);
//	clientDataRequest.setBufferName(bufferName);

//	switch(requestType){
//	case AMDSClientDataRequestV1::Continuous:
//		break;
//	case AMDSClientDataRequestV1::StartTimePlusCount:{
//		qDebug() << "Going to fill the actual data in star time plus count";

//		for(int x = 0, size = bufferGroupInfos.count(); x < size; x++)
//			clientDataRequest.appendBufferGroupInfo(bufferGroupInfos.at(x));

//		int totalCounter = 0;
//		clientDataRequest.clearData();
//		for(quint16 x = 0; x < dataCount; x++){
//			AMDSScalarDataHolder *oneScalerDataHolder = new AMDSScalarDataHolder();
//			oneScalerDataHolder->setSingleValue(totalVector.at(totalCounter++));

//			clientDataRequest.appendData(oneScalerDataHolder);
//			double oneStoredValue;
//			oneScalerDataHolder->data(&oneStoredValue);
//			qDebug() << "One value was " << oneStoredValue;
//		}

//		break;}
//	case AMDSClientDataRequestV1::RelativeCountPlusCount:
//		break;
//	case AMDSClientDataRequestV1::StartTimeToEndTime:
//		break;
//	case AMDSClientDataRequestV1::MiddleTimePlusCountBeforeAndAfter:
//		break;
//	case AMDSClientDataRequestV1::Introspection:{
//		for(int x = 0, size = bufferGroupInfos.count(); x < size; x++)
//			clientDataRequest.appendBufferGroupInfo(bufferGroupInfos.at(x));

//		break;}
//	case AMDSClientDataRequestV1::Statistics:{
//		for(int x = 0, size = packetStats.count(); x < size; x++)
//			clientDataRequest.appendPacketStats(packetStats.at(x));
//		break;}
//	default:
//		break;
//	}
//}

void AMDSDataStream::write(const AMDSAxisInfo &axisInfo){
//	qDebug() << "Writing an axisInfo named " << axisInfo.name();
	*this << axisInfo.name();
	*this << axisInfo.description();
	*this << axisInfo.units();
	QDataStream::operator <<(axisInfo.size());
	QDataStream::operator <<(axisInfo.isUniform());
	QDataStream::operator <<(axisInfo.start());
	QDataStream::operator <<(axisInfo.increment());
}

void AMDSDataStream::write(const AMDSBufferGroupInfo &bufferGroupInfo){
//	qDebug() << "Writing a bufferGroupInfo named " << bufferGroupInfo.name();
	*this << bufferGroupInfo.name();
	*this << bufferGroupInfo.description();
	*this << bufferGroupInfo.units();
	QDataStream::operator <<((quint8)(bufferGroupInfo.axes().count()));
//	qDebug() << "This bufferGroupInfo has " << (quint8)(bufferGroupInfo.axes().count()) << " axes";
	for(int x = 0, size = bufferGroupInfo.axes().count(); x < size; x++){
//		qDebug() << "Attempting axis " << x << " as " << bufferGroupInfo.axes().at(x).name();
		write(bufferGroupInfo.axes().at(x));
	}
}

void AMDSDataStream::write(const AMDSPacketStats &packetStat){
	*this << packetStat.name();
	QDataStream::operator <<((quint64)(packetStat.inboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.outboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxInboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxOutboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxTotalBytes()));
}

//void AMDSDataStream::write(const AMDSClientDataRequestV1 &clientDataRequest){
//	*this << clientDataRequest.socketKey();
//	*this << clientDataRequest.lastError();
//	QDataStream::operator <<((quint8)(clientDataRequest.requestType()));
//	QDataStream::operator <<((quint8)(clientDataRequest.responseType()));
//	QDataStream::operator <<(clientDataRequest.includeStatusData());
//	*this << clientDataRequest.time1();
//	*this << clientDataRequest.time2();
//	QDataStream::operator <<(clientDataRequest.count1());
//	QDataStream::operator <<(clientDataRequest.count2());
//	*this << clientDataRequest.bufferName();

//	switch(clientDataRequest.requestType()){
//	case AMDSClientDataRequestV1::Continuous:
//		break;
//	case AMDSClientDataRequestV1::StartTimePlusCount:
//		qDebug() << "Processing a start time plus count CDR with data count " << clientDataRequest.data().count() << " and BGI count " << clientDataRequest.bufferGroupInfos().count();

//		QDataStream::operator <<((quint16)(clientDataRequest.bufferGroupInfos().count()));
//		if( (clientDataRequest.data().count() > 0) && (clientDataRequest.data().at(0)->axesStyle() == AMDSDataHolder::UniformAxes) && (clientDataRequest.bufferGroupInfos().count() > 0) ){
//			write(clientDataRequest.bufferGroupInfos().at(0));

//			qDebug() << "We have uniform axes, so the total count should be the count " << clientDataRequest.data().count();
//			qDebug() << " multiplied by the spanSize " << clientDataRequest.bufferGroupInfos().at(0).spanSize();

//			QDataStream::operator <<((quint16)(clientDataRequest.data().count()));
//			for(int x = 0, xSize = clientDataRequest.data().count(); x < xSize; x++){
//				QVector<double> oneClientDataRequestVector = QVector<double>(clientDataRequest.bufferGroupInfos().at(0).spanSize());
//				clientDataRequest.data().at(x)->data(oneClientDataRequestVector.data());
//				for(int y = 0, ySize = clientDataRequest.bufferGroupInfos().at(0).spanSize(); y < ySize; y++)
//					QDataStream::operator <<(oneClientDataRequestVector[y]);
//			}
//		}

//		break;
//	case AMDSClientDataRequestV1::RelativeCountPlusCount:
//		break;
//	case AMDSClientDataRequestV1::StartTimeToEndTime:
//		break;
//	case AMDSClientDataRequestV1::MiddleTimePlusCountBeforeAndAfter:
//		break;
//	case AMDSClientDataRequestV1::Introspection:
//		QDataStream::operator <<((quint16)(clientDataRequest.bufferGroupInfos().count()));
//		for(int x = 0, size = clientDataRequest.bufferGroupInfos().count(); x < size; x++)
//			write(clientDataRequest.bufferGroupInfos().at(x));
//		break;
//	case AMDSClientDataRequestV1::Statistics:
//		QDataStream::operator <<((quint16)(clientDataRequest.packetStats().count()));
//		for(int x = 0, size = clientDataRequest.packetStats().count(); x < size; x++)
//			write(clientDataRequest.packetStats().at(x));
//		break;
//	default:
//		break;
//	}
//}

void AMDSDataStream::encodeClientRequestType(const AMDSClientRequest &clientRequest){
	qDebug() << "About to encode " << (quint8)clientRequest.requestType();
	QDataStream::operator <<((quint8)clientRequest.requestType());
}

void AMDSDataStream::write(const AMDSClientRequest &clientRequest){
	qDebug() << "In write for an AMDSClientRequest";
	qDebug() << "Actual type is " << clientRequest.metaObject()->className();

	qDebug() << "About to call writeToStream";
	clientRequest.writeToDataStream(this);
}

AMDSClientRequestDefinitions::RequestType AMDSDataStream::decodeRequestType(){
	quint8 clientRequestTypeAsInt;
	qDebug() << "Try to decode";
	QDataStream::operator >>(clientRequestTypeAsInt);
	if(status() != QDataStream::Ok)
		return AMDSClientRequestDefinitions::InvalidRequest;

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)clientRequestTypeAsInt;
	qDebug() << "Decoded as " << clientRequestType;
	return clientRequestType;
}

AMDSClientRequest* AMDSDataStream::decodeAndInstantiateClientRequestType(){
	AMDSClientRequestDefinitions::RequestType clientRequestType = decodeRequestType();
	qDebug() << "Request type decoded as " << clientRequestType;
	if(clientRequestType >= AMDSClientRequestDefinitions::InvalidRequest)
		return 0;
	return AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
}

void AMDSDataStream::read(AMDSClientRequest &clientRequest){
	qDebug() << "In read for AMDSClientRequest";
	qDebug() << "Actual type is " << clientRequest.metaObject()->className();

	qDebug() << "About to call readFromStream";
	clientRequest.readFromDataStream(this);
}
