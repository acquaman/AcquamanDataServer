#include "AMDSScalarDataHolder.h"

#include "AMDSDataStream.h"

AMDSLightWeightScalarDataHolder::AMDSLightWeightScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, QObject *parent) :
	AMDSLightWeightDataHolder(parent), valueFlatArray_(dataType, 1)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightScalarDataHolder::~AMDSLightWeightScalarDataHolder()
{
}

bool AMDSLightWeightScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our eventData to the stream
	if(!AMDSLightWeightDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	if(encodeDataType)
		dataStream->encodeDataType(valueFlatArray_.dataType());
	dataStream->write(valueFlatArray_);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSLightWeightScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our eventData from the stream (and deleting any old eventData memory we allocated)
	if(!AMDSLightWeightDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	AMDSDataTypeDefinitions::DataType readDataType;
	if(decodeAsDataType == AMDSDataTypeDefinitions::InvalidType)
		readDataType = dataStream->decodeDataType();
	else
		readDataType = decodeAsDataType;
	if(readDataType == AMDSDataTypeDefinitions::InvalidType)
		return false;

	valueFlatArray_.clearAndReset(readDataType, 1);
	dataStream->read(valueFlatArray_);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

AMDSFullScalarDataHolder::AMDSFullScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullDataHolder(axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightScalarDataHolder(dataType, this);
}

AMDSFullScalarDataHolder::~AMDSFullScalarDataHolder()
{
}

bool AMDSFullScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	return true;
}

bool AMDSFullScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	return true;
}
