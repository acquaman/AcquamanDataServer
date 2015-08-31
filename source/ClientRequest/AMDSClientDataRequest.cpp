#include "source/ClientRequest/AMDSClientDataRequest.h"

#include "source/AMDSDataStream.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	bufferName_ = "Invalid";
	includeStatusData_ = false;
	bufferGroupInfo_.setName("Invalid");
}

AMDSClientDataRequest::AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QString &bufferName, bool includeStatusData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
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
	AMDSClientRequest(other)
{
	(*this) = other;
}

AMDSClientDataRequest& AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		setBufferName(other.bufferName());
		setIncludeStatusData(other.includeStatusData());

		bufferGroupInfo_ = other.bufferGroupInfo_;

		uniformDataType_ = other.uniformDataType();
		clearData();
		for(int x = 0, size = other.data().count(); x < size; x++)
			appendData(other.data().at(x));
	}
	return (*this);
}

int AMDSClientDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if(errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;
	*dataStream << includeStatusData_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS;

	bool includeData = bufferGroupInfo_.includeData();
	*dataStream << includeData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA;

	// write data to the data stream
	if(includeData){

		dataStream->write(bufferGroupInfo_);

		quint8 uniformDataType = (quint8)uniformDataType_;
		*dataStream << uniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE;

		quint16 dataCount = data_.count();
		*dataStream << dataCount;
		if(dataStream->status() != QDataStream::Ok || dataCount == 0)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT;

		bool encodeDataTypeInDataHolder = true;
		AMDSDataTypeDefinitions::DataType underlyingDataType = data_.at(0)->dataType();
		if(underlyingDataType != AMDSDataTypeDefinitions::InvalidType)
			encodeDataTypeInDataHolder = false;
		*dataStream << encodeDataTypeInDataHolder;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE_IN_DATA_HOLDER;

		if(!encodeDataTypeInDataHolder){
			dataStream->encodeDataType(underlyingDataType);
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_TYPE;
		}

		dataStream->encodeDataHolderType(*(data_.at(0)));
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE;

		for(int x = 0, size = data_.count(); x < size; x++){
			dataStream->write(*(data_.at(x)), encodeDataTypeInDataHolder);
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA;
		}
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::readFromDataStream(dataStream);
	if(errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QString readBufferName;
	bool readIncludeStatusData;
	bool readIncludeData;
	AMDSBufferGroupInfo readBufferGroupInfo;
	quint8 readUniformDataType;
	quint16 readDataCount;
	bool decodeDataTypeInDataHolder;
	AMDSDataTypeDefinitions::DataType readDataType;
	QList<AMDSDataHolder*> readDataHolder;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	*dataStream >> readIncludeStatusData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS;

	*dataStream >> readIncludeData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA;

	if(readIncludeData){
		dataStream->read(readBufferGroupInfo);
		*dataStream >> readUniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE;

		*dataStream >> readDataCount;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT;

		*dataStream >> decodeDataTypeInDataHolder;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE_IN_DATA_HOLDER;

		if(!decodeDataTypeInDataHolder){
			readDataType = dataStream->decodeDataType();
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DECODE_DATA_TYPE;
		}
		else
			readDataType = AMDSDataTypeDefinitions::InvalidType;

		QString readDataHolderType;
		readDataHolderType = dataStream->decodeDataHolderType();
		if(readDataHolderType.isEmpty())
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_TYPE;

		AMDSDataHolder *oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
		if(!oneDataHolder)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INSTANTIATE_DATA_HOLDER;

		for(int x = 0; x < readDataCount; x++){
			oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
			dataStream->read(*oneDataHolder, readDataType);
			if(dataStream->status() != QDataStream::Ok)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA;
			readDataHolder.append(oneDataHolder);
		}
	}

	setBufferName(readBufferName);
	setIncludeStatusData(readIncludeStatusData);
	if(readIncludeData){
		setBufferGroupInfo(readBufferGroupInfo);

		// do some data setting here
		setUniformDataType((AMDSDataTypeDefinitions::DataType)readUniformDataType);

		clearData();
		data_.append(readDataHolder);
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

bool AMDSClientDataRequest::validateResponse()
{
	bool noError = true;
	if(responseType() == AMDSClientRequest::Error) {
		AMDSErrorMon::alert(this, 0, QString("(msg %1 --- %2) Failed to retrieve data. Error: %3").arg(socketKey()).arg(bufferName()).arg(errorMessage()));
		noError = false;
	} else {
		if (data().count() == 0)
			AMDSErrorMon::information(this, 0, QString("(msg %1 --- %2) No data for this message yet!").arg(socketKey()).arg(bufferName()));
		else {
			for(int x = 0, size = data().count(); x < size; x++){
				AMDSFlatArray oneFlatArray = AMDSFlatArray(uniformDataType(), bufferGroupInfo().spanSize());
				data().at(x)->data(&oneFlatArray);
				AMDSErrorMon::information(this, 0, QString("(msg %1 --- %2) Data at %3 - %4").arg(socketKey()).arg(bufferName()).arg(x).arg(oneFlatArray.printData()));
			}
		}
	}

	return noError;
}
