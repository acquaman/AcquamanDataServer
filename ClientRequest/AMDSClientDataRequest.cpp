#include "AMDSClientDataRequest.h"

#include "AMDSDataStream.h"
#include "AMDSScalarDataHolder.h"
#include "AMDSDataHolderSupport.h"

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
		// do some data writing here

//		AMDSFlatArray oneFlatArray(AMDSDataTypeDefinitions::Signed8, 0);
//		data_.at(0)->data(&oneFlatArray);
//		dataStream->encodeDataType(oneFlatArray.dataType());
//		for(int x = 0, xSize = data_.count(); x < xSize; x++){
//			AMDSFlatArray oneFlatArray(AMDSDataTypeDefinitions::Signed8, 0);
//			data_.at(x)->data(&oneFlatArray);
//			dataStream->write(oneFlatArray);
//		}

//		dataStream->encodeDataHolderType(*(data_.at(0)));
//		dataStream->write(*(data_.at(0)));

		dataStream->encodeDataHolderType(*(data_.at(0)));
		for(int x = 0, size = data_.count(); x < size; x++)
			dataStream->write(*(data_.at(x)));
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
	AMDSDataTypeDefinitions::DataType readDataType;
	QList<AMDSFlatArray> readFlatArrays;
	QList<AMDSDataHolder*> readDataHolder;
	AMDSDataHolder *oneDataHolder;

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
		// do some data reading here

//		readDataType = dataStream->decodeDataType();
//		if(dataStream->status() != QDataStream::Ok)
//			return false;


//		for(quint16 x = 0; x < readDataCount; x++){
//			AMDSFlatArray oneFlatArray(readDataType, 1);
//			dataStream->read(oneFlatArray);
//			readFlatArrays.append(oneFlatArray);
//		}

//		oneDataHolder = dataStream->decodeAndInstantiateDataHolder();
//		dataStream->read(*oneDataHolder);
//		if(dataStream->status() != QDataStream::Ok)
//			return false;

		QString readDataHolderType;
		readDataHolderType = dataStream->decodeDataHolderType();
		if(readDataHolderType.isEmpty())
			return false;

		AMDSDataHolder *oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
		if(!oneDataHolder)
			return false;

		for(int x = 0; x < readDataCount; x++){
			oneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(readDataHolderType);
			dataStream->read(*oneDataHolder);
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
		for(quint16 x = 0; x < readDataCount; x++){
			appendData(readDataHolder.at(x));
//			AMDSLightWeightScalarDataHolder *oneScalarDataHolder = new AMDSLightWeightScalarDataHolder();
//			oneScalarDataHolder->setData(&(readFlatArrays[x]));

//			appendData(oneScalarDataHolder);
//			AMDSFlatArray oneStoredValue(AMDSDataTypeDefinitions::InvalidType, 0);
//			oneScalarDataHolder->data(&oneStoredValue);
		}

//		data_[0] = oneDataHolder;
	}

	return true;
}
