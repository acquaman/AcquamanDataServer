#include "AMDSDataStream.h"

#include "source/ClientRequest/AMDSClientRequestSupport.h"
#include "source/DataElement/AMDSEventDataSupport.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "util/AMErrorMonitor.h"

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

void AMDSDataStream::write(const AMDSAxisInfo &axisInfo){
	*this << axisInfo.name();
	*this << axisInfo.description();
	*this << axisInfo.units();
	QDataStream::operator <<(axisInfo.size());
	QDataStream::operator <<(axisInfo.isUniform());
	QDataStream::operator <<(axisInfo.start());
	QDataStream::operator <<(axisInfo.increment());
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

void AMDSDataStream::write(const AMDSBufferGroupInfo &bufferGroupInfo){
	*this << bufferGroupInfo.name();
	*this << bufferGroupInfo.description();
	*this << bufferGroupInfo.units();
	QDataStream::operator <<((quint8)(bufferGroupInfo.axes().count()));
	for(int x = 0, size = bufferGroupInfo.axes().count(); x < size; x++)
		write(bufferGroupInfo.axes().at(x));
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

void AMDSDataStream::write(const AMDSPacketStats &packetStat){
	*this << packetStat.name();
	QDataStream::operator <<((quint64)(packetStat.inboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.outboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxInboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxOutboundBytes()));
	QDataStream::operator <<((quint64)(packetStat.maxTotalBytes()));
}

void AMDSDataStream::encodeEventDataType(const AMDSEventData &eventData){
	QString eventDataClassNameAsString(eventData.metaObject()->className());
	*this << eventDataClassNameAsString;
}

void AMDSDataStream::write(const AMDSEventData &eventData){
	eventData.writeToDataStream(this);
}

QString AMDSDataStream::decodeEventDataType(){
	QString eventDataClassName;
	*this >> eventDataClassName;
	if(status() != QDataStream::Ok)
		return QString();

	return eventDataClassName;
}

AMDSEventData* AMDSDataStream::decodeAndInstantiateEventData(){
	QString eventDataClassName = decodeEventDataType();
	return AMDSEventDataSupport::instantiateEventDataFromClassName(eventDataClassName);
}

void AMDSDataStream::read(AMDSEventData &eventData){
	eventData.readFromDataStream(this);
}

void AMDSDataStream::encodeDataHolderType(const AMDSDataHolder &dataHolder){
	QString classNameAsString(dataHolder.metaObject()->className());
	*this << classNameAsString;
}

void AMDSDataStream::write(const AMDSDataHolder &dataHolder, bool encodeDataType){
	dataHolder.writeToDataStream(this, encodeDataType);
}

QString AMDSDataStream::decodeDataHolderType(){
	QString dataHolderClassName;
	*this >> dataHolderClassName;
	if(status() != QDataStream::Ok)
		return QString();

	return dataHolderClassName;
}

AMDSDataHolder* AMDSDataStream::decodeAndInstantiateDataHolder(){
	QString dataHolderClassName = decodeDataHolderType();
	return AMDSDataHolderSupport::instantiateDataHolderFromClassName(dataHolderClassName);
}

void AMDSDataStream::read(AMDSDataHolder &dataHolder, AMDSDataTypeDefinitions::DataType dataType){
	dataHolder.readFromDataStream(this, dataType);
}

void AMDSDataStream::encodeDataType(AMDSDataTypeDefinitions::DataType dataType){
	QDataStream::operator <<((quint8)dataType);
}

void AMDSDataStream::write(const AMDSFlatArray &flatArray){
	QDataStream::operator <<(flatArray.size());
	flatArray.write(this);
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

	flatArray.clearAndReset(flatArray.dataType(), size);
	flatArray.read(this);
}

void AMDSDataStream::encodeClientRequestType(const AMDSClientRequest &clientRequest){
	QDataStream::operator <<((quint8)clientRequest.requestType());
}

void AMDSDataStream::write(const AMDSClientRequest &clientRequest){
	int errorCode = clientRequest.writeToDataStream(this);
	if (errorCode != AMDS_CLIENTREQUEST_SUCCESS) {
		QString errorMessage = AMDSClientRequestDefinitions::errorMessage(errorCode, AMDSClientRequestDefinitions::Write, clientRequest.requestType());
		AMErrorMon::alert(0, errorCode, errorMessage);
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

	AMDSClientRequest* clientRequest = AMDSClientRequestSupport::instantiateClientRequestFromType(clientRequestType);
	if (!clientRequest) {
		AMErrorMon::information(0, AMDS_SERVER_INFO_DS_FAILED_TO_PARSE_REQUEST, QString("AMDSDataStream::Failed to parse clientRequest for type: %s").arg(clientRequestType));
	}

	return clientRequest;
}

void AMDSDataStream::read(AMDSClientRequest &clientRequest){
	int errorCode = clientRequest.readFromDataStream(this);
	if (errorCode != AMDS_CLIENTREQUEST_SUCCESS) {
		QString errMessage = AMDSClientRequestDefinitions::errorMessage(errorCode, AMDSClientRequestDefinitions::Read, clientRequest.requestType());
		AMErrorMon::alert(0, errorCode, errMessage);
	}
}
