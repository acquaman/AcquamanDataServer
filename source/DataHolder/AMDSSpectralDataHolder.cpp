#include "AMDSSpectralDataHolder.h"

#include "source/Connection/AMDSDataStream.h"

AMDSLightWeightSpectralDataHolder::AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent) :
	AMDSLightWeightGenericFlatArrayDataHolder(dataType, size, parent)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightSpectralDataHolder::AMDSLightWeightSpectralDataHolder(AMDSLightWeightSpectralDataHolder *sourceDataHolder, QObject *parent)
	:AMDSLightWeightGenericFlatArrayDataHolder(sourceDataHolder, parent)
{

}

AMDSLightWeightSpectralDataHolder::~AMDSLightWeightSpectralDataHolder()
{
}

bool AMDSLightWeightSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	return AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType);
}

bool AMDSLightWeightSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	return AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType);
}

AMDSFullSpectralDataHolder::AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, size, axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightSpectralDataHolder(dataType, size, this);
}

AMDSFullSpectralDataHolder::~AMDSFullSpectralDataHolder()
{
}

bool AMDSFullSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	return true;
}

bool AMDSFullSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	return true;
}
