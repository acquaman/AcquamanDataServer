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

	*this >> name;
	if(status() != QDataStream::Ok)
		return;
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

	*this >> name;
	if(status() != QDataStream::Ok)
		return;
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

void AMDSDataStream::write(const AMDSAxisInfo &axisInfo){
	*this << axisInfo.name();
	*this << axisInfo.description();
	*this << axisInfo.units();
	QDataStream::operator <<(axisInfo.size());
	QDataStream::operator <<(axisInfo.isUniform());
	QDataStream::operator <<(axisInfo.start());
	QDataStream::operator <<(axisInfo.increment());
}

void AMDSDataStream::write(const AMDSBufferGroupInfo &bufferGroupInfo){
	*this << bufferGroupInfo.name();
	*this << bufferGroupInfo.description();
	*this << bufferGroupInfo.units();
	QDataStream::operator <<((quint8)(bufferGroupInfo.axes().count()));
	for(int x = 0, size = bufferGroupInfo.axes().count(); x < size; x++)
		write(bufferGroupInfo.axes().at(x));
}

void AMDSDataStream::write(const AMDSPacketStats &packetStat){
	*this << packetStat.name();
	QDataStream::operator <<((quint64)(packetStat.inboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.outboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxInboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxOutboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxTotalBytes()));
}

void AMDSDataStream::encodeDataType(AMDSDataTypeDefinitions::DataType dataType){
	QDataStream::operator <<((quint8)dataType);
}

void AMDSDataStream::write(const AMDSFlatArray &flatArray){
	QDataStream::operator <<(flatArray.size());
	switch(flatArray.dataType()){
	case AMDSDataTypeDefinitions::Signed8:
		*this << flatArray.constVectorQint8();
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		*this << flatArray.constVectorQuint8();
		break;
	case AMDSDataTypeDefinitions::Signed16:
		*this << flatArray.constVectorQint16();
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		*this << flatArray.constVectorQuint16();
		break;
	case AMDSDataTypeDefinitions::Signed32:
		*this << flatArray.constVectorQint32();
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		*this << flatArray.constVectorQuint32();
		break;
	case AMDSDataTypeDefinitions::Signed64:
		*this << flatArray.constVectorQint64();
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		*this << flatArray.constVectorQuint64();
		break;
	case AMDSDataTypeDefinitions::Float:
		*this << flatArray.constVectorFloat();
		break;
	case AMDSDataTypeDefinitions::Double:
		*this << flatArray.constVectorDouble();
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}
}

void AMDSDataStream::encodeClientRequestType(const AMDSClientRequest &clientRequest){
	QDataStream::operator <<((quint8)clientRequest.requestType());
}

void AMDSDataStream::write(const AMDSClientRequest &clientRequest){
	clientRequest.writeToDataStream(this);
}

AMDSDataTypeDefinitions::DataType AMDSDataStream::decodeDataType(){
	quint8 dataTypeAsInt;
	QDataStream::operator >>(dataTypeAsInt);
	if(status() != QDataStream::Ok)
		return AMDSDataTypeDefinitions::InvalidType;

	AMDSDataTypeDefinitions::DataType dataType = (AMDSDataTypeDefinitions::DataType)dataTypeAsInt;
	return dataType;
}

void AMDSDataStream::read(AMDSFlatArray &flatArray){
	quint32 size;

	QDataStream::operator >>(size);
	if(status() != QDataStream::Ok)
		return;

	AMDSDataTypeDefinitions::DataType assumedDataType = flatArray.dataType();
	flatArray.clearAndReset(assumedDataType, size);

	switch(flatArray.dataType()){
	case AMDSDataTypeDefinitions::Signed8:
		*this >> flatArray.vectorQint8();
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		*this >> flatArray.vectorQuint8();
		break;
	case AMDSDataTypeDefinitions::Signed16:
		*this >> flatArray.vectorQint16();
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		*this >> flatArray.vectorQuint16();
		break;
	case AMDSDataTypeDefinitions::Signed32:
		*this >> flatArray.vectorQint32();
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		*this >> flatArray.vectorQuint32();
		break;
	case AMDSDataTypeDefinitions::Signed64:
		*this >> flatArray.vectorQint64();
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		*this >> flatArray.vectorQuint64();
		break;
	case AMDSDataTypeDefinitions::Float:
		*this >> flatArray.vectorFloat();
		break;
	case AMDSDataTypeDefinitions::Double:
		*this >> flatArray.vectorDouble();
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}
}

AMDSClientRequestDefinitions::RequestType AMDSDataStream::decodeRequestType(){
	quint8 clientRequestTypeAsInt;
	QDataStream::operator >>(clientRequestTypeAsInt);
	if(status() != QDataStream::Ok)
		return AMDSClientRequestDefinitions::InvalidRequest;

	AMDSClientRequestDefinitions::RequestType clientRequestType = (AMDSClientRequestDefinitions::RequestType)clientRequestTypeAsInt;
	return clientRequestType;
}

AMDSClientRequest* AMDSDataStream::decodeAndInstantiateClientRequestType(){
	AMDSClientRequestDefinitions::RequestType clientRequestType = decodeRequestType();
	if(clientRequestType >= AMDSClientRequestDefinitions::InvalidRequest)
		return 0;
	return AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
}

void AMDSDataStream::read(AMDSClientRequest &clientRequest){
	clientRequest.readFromDataStream(this);
}
