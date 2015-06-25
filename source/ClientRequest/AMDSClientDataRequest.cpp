#include "source/ClientRequest/AMDSClientDataRequest.h"

#include "source/AMDSDataStream.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"

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

		uniformDataType_ = other.uniformDataType();
		clearData();
		for(int x = 0, size = other.data().count(); x < size; x++)
			appendData(other.data().at(x));
	}
	return (*this);
}

bool AMDSClientDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;

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
		quint8 uniformDataType = (quint8)uniformDataType_;
		*dataStream << uniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		quint16 dataCount = data_.count();
		*dataStream << dataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;

		bool encodeDataTypeInDataHolder = true;
		AMDSDataTypeDefinitions::DataType underlyingDataType = data_.at(0)->dataType();
		if(underlyingDataType != AMDSDataTypeDefinitions::InvalidType)
			encodeDataTypeInDataHolder = false;
		*dataStream << encodeDataTypeInDataHolder;
		if(dataStream->status() != QDataStream::Ok)
			return false;

		if(!encodeDataTypeInDataHolder){
			dataStream->encodeDataType(underlyingDataType);
			if(dataStream->status() != QDataStream::Ok)
				return false;
		}

		dataStream->encodeDataHolderType(*(data_.at(0)));
		if(dataStream->status() != QDataStream::Ok)
			return false;

		for(int x = 0, size = data_.count(); x < size; x++){
			dataStream->write(*(data_.at(x)), encodeDataTypeInDataHolder);
			if(dataStream->status() != QDataStream::Ok)
				return false;
		}
	}

	return true;
}

bool AMDSClientDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;

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
		return false;
	*dataStream >> readIncludeStatusData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readIncludeData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	if(readIncludeData){
		dataStream->read(readBufferGroupInfo);
		*dataStream >> readUniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		*dataStream >> readDataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;

		*dataStream >> decodeDataTypeInDataHolder;
		if(dataStream->status() != QDataStream::Ok)
			return false;

		if(!decodeDataTypeInDataHolder){
			readDataType = dataStream->decodeDataType();
			if(dataStream->status() != QDataStream::Ok)
				return false;
		}
		else
			readDataType = AMDSDataTypeDefinitions::InvalidType;

		QString readDataHolderType;
		readDataHolderType = dataStream->decodeDataHolderType();
		if(readDataHolderType.isEmpty())
			return false;

		AMDSDataHolder *oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
		if(!oneDataHolder)
			return false;

		for(int x = 0; x < readDataCount; x++){
			oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
			dataStream->read(*oneDataHolder, readDataType);
			if(dataStream->status() != QDataStream::Ok)
				return false;
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
		for(quint16 x = 0; x < readDataCount; x++)
			appendData(readDataHolder.at(x));

	}

	return true;
}