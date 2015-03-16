#include "AMDSGenericFlatArrayDataHolder.h"

#include "AMDSDataStream.h"

AMDSLightWeightGenericFlatArrayDataHolder::AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent) :
	AMDSLightWeightDataHolder(parent), valueFlatArray_(dataType, size)
{
}

AMDSLightWeightGenericFlatArrayDataHolder::~AMDSLightWeightGenericFlatArrayDataHolder()
{
}

bool AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
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

bool AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
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
