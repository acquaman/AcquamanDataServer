#include "AMDSSpectralDataHolder.h"

#include "AMDSDataStream.h"

AMDSLightWeightSpectralDataHolder::AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent) :
	AMDSLightWeightDataHolder(parent), valueFlatArray_(dataType, size)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightSpectralDataHolder::~AMDSLightWeightSpectralDataHolder()
{
}

bool AMDSLightWeightSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
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

bool AMDSLightWeightSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
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

AMDSFullSpectralDataHolder::AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullDataHolder(axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightSpectralDataHolder(dataType, size, this);
}

AMDSFullSpectralDataHolder::~AMDSFullSpectralDataHolder()
{
}

bool AMDSFullSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	return true;
}

bool AMDSFullSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	return true;
}
