#include "AMDSScalarDataHolder.h"

#include "AMDSDataStream.h"

AMDSLightWeightScalarDataHolder::AMDSLightWeightScalarDataHolder(QObject *parent) :
	AMDSLightWeightDataHolder(parent)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightScalarDataHolder::~AMDSLightWeightScalarDataHolder()
{
}

bool AMDSLightWeightScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream) const{
	// This will take care of encoding and writing our eventData to the stream
	if(!AMDSLightWeightDataHolder::writeToDataStream(dataStream))
		return false;

	AMDSFlatArray flatDataArray = AMDSFlatArray();
	data(&flatDataArray);
	dataStream->write(flatDataArray);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSLightWeightScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream){
	// This will take care of decoding and instantiating our eventData from the stream (and deleting any old eventData memory we allocated)
	if(!AMDSLightWeightDataHolder::readFromDataStream(dataStream))
		return false;

	// something about data here

	return true;
}

AMDSFullScalarDataHolder::AMDSFullScalarDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullDataHolder(axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightScalarDataHolder(this);
}

AMDSFullScalarDataHolder::~AMDSFullScalarDataHolder()
{
}

bool AMDSFullScalarDataHolder::writeToDataStream(AMDSDataStream *dataStream) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullDataHolder::writeToDataStream(dataStream))
		return false;

	return true;
}

bool AMDSFullScalarDataHolder::readFromDataStream(AMDSDataStream *dataStream){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullDataHolder::readFromDataStream(dataStream))
		return false;

	return true;
}
