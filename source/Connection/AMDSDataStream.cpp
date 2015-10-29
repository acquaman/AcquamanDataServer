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

	flatArray.read(this, size);
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
