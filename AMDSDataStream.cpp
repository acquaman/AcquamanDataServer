#include "AMDSDataStream.h"

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

void AMDSDataStream::read(AMDSClientDataRequest &clientDataRequest){
	QString socketKey;
	QString errorMessage;
	quint8 requestType;
	quint8 responseType;
	bool includeStatusData;
	QDateTime time1;
	QDateTime time2;
	quint64 count1;
	quint64 count2;
	QString bufferName;
	QList<AMDSBufferGroupInfo> bufferGroupInfos;
	QList<AMDSPacketStats> packetStats;

	*this >> socketKey;
	if(status() != QDataStream::Ok)
		return;
	*this >> errorMessage;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(requestType);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(responseType);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(includeStatusData);
	if(status() != QDataStream::Ok)
		return;
	*this >> time1;
	if(status() != QDataStream::Ok)
		return;
	*this >> time2;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(count1);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(count2);
	if(status() != QDataStream::Ok)
		return;
	*this >> bufferName;
	if(status() != QDataStream::Ok)
		return;

	switch(requestType){
	case AMDSClientDataRequest::Continuous:
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		break;
	case AMDSClientDataRequest::Introspection:{
		quint16 bufferGroupCount;

		QDataStream::operator >>(bufferGroupCount);
		if(status() != QDataStream::Ok)
			return;
		for(int x = 0, size = bufferGroupCount; x < size; x++){
			AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
			read(oneBufferGroupInfo);
			if(oneBufferGroupInfo.name() == "Invalid")
				return;
			bufferGroupInfos.append(oneBufferGroupInfo);
		}
		break;}
	case AMDSClientDataRequest::Statistics:{
		quint16 packetStatsCount;

		QDataStream::operator >>(packetStatsCount);
		if(status() != QDataStream::Ok)
			return;
		for(int x = 0, size = packetStatsCount; x < size; x++){
			AMDSPacketStats onePacketStat("Invalid");
			read(onePacketStat);
			if(onePacketStat.name() == "Invalid")
				return;
			packetStats.append(onePacketStat);
		}
		break;}
	default:
		break;
	}

	clientDataRequest.setSocketKey(socketKey);
	clientDataRequest.setErrorMessage(errorMessage);
	clientDataRequest.setRequestType((AMDSClientDataRequest::RequestType)requestType);
	clientDataRequest.setResponseType((AMDSClientDataRequest::ResponseType)responseType);
	clientDataRequest.setIncludeStatusData(includeStatusData);
	clientDataRequest.setTime1(time1);
	clientDataRequest.setTime2(time2);
	clientDataRequest.setCount1(count1);
	clientDataRequest.setCount2(count2);
	clientDataRequest.setBufferName(bufferName);

	switch(requestType){
	case AMDSClientDataRequest::Continuous:
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		break;
	case AMDSClientDataRequest::Introspection:{
		for(int x = 0, size = bufferGroupInfos.count(); x < size; x++)
			clientDataRequest.appendBufferGroupInfo(bufferGroupInfos.at(x));

		break;}
	case AMDSClientDataRequest::Statistics:{
		for(int x = 0, size = packetStats.count(); x < size; x++)
			clientDataRequest.appendPacketStats(packetStats.at(x));
		break;}
	default:
		break;
	}
}

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

void AMDSDataStream::write(const AMDSClientDataRequest &clientDataRequest){
	*this << clientDataRequest.socketKey();
	*this << clientDataRequest.lastError();
	QDataStream::operator <<((quint8)(clientDataRequest.requestType()));
	QDataStream::operator <<((quint8)(clientDataRequest.responseType()));
	QDataStream::operator <<(clientDataRequest.includeStatusData());
	*this << clientDataRequest.time1();
	*this << clientDataRequest.time2();
	QDataStream::operator <<(clientDataRequest.count1());
	QDataStream::operator <<(clientDataRequest.count2());
	*this << clientDataRequest.bufferName();

	switch(clientDataRequest.requestType()){
	case AMDSClientDataRequest::Continuous:
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		break;
	case AMDSClientDataRequest::Introspection:
		QDataStream::operator <<((quint16)(clientDataRequest.bufferGroupInfos().count()));
		for(int x = 0, size = clientDataRequest.bufferGroupInfos().count(); x < size; x++)
			write(clientDataRequest.bufferGroupInfos().at(x));
		break;
	case AMDSClientDataRequest::Statistics:
		QDataStream::operator <<((quint16)(clientDataRequest.packetStats().count()));
		for(int x = 0, size = clientDataRequest.packetStats().count(); x < size; x++)
			write(clientDataRequest.packetStats().at(x));
		break;
	default:
		break;
	}
}
