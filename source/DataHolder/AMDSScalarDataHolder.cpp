#include "source/DataHolder/AMDSScalarDataHolder.h"

#include "source/AMDSDataStream.h"

AMDSLightWeightScalarDataHolder::AMDSLightWeightScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, QObject *parent) :
	AMDSLightWeightGenericFlatArrayDataHolder(dataType, 1, parent)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightScalarDataHolder::~AMDSLightWeightScalarDataHolder()
{
}

bool AMDSLightWeightScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	return AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType);
}

bool AMDSLightWeightScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	return AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType);
}

AMDSFullScalarDataHolder::AMDSFullScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, 1, axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightScalarDataHolder(dataType, this);
}

AMDSFullScalarDataHolder::~AMDSFullScalarDataHolder()
{
}

bool AMDSFullScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	return true;
}

bool AMDSFullScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	return true;
}
