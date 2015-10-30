#include "AMDSImageDataHolder.h"

AMDSLightWeightImageDataHolder::AMDSLightWeightImageDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint16 fastAxisSize, quint16 slowAxisSize, QObject *parent) :
	AMDSLightWeightGenericFlatArrayDataHolder(dataType, ((quint32)fastAxisSize)*((quint32)slowAxisSize), parent)
{
	eventData_ = new AMDSLightWeightEventData();
	fastAxisSize_ = fastAxisSize;
}

AMDSLightWeightImageDataHolder::~AMDSLightWeightImageDataHolder()
{
}

bool AMDSLightWeightImageDataHolder::writeToDataStream(QDataStream *dataStream) const
{
	if (!AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(dataStream))
		return false;

	*dataStream << fastAxisSize_;
	if (dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSLightWeightImageDataHolder::readFromDataStream(QDataStream *dataStream)
{
	if (!AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(dataStream))
		return false;

	*dataStream >> fastAxisSize_;
	if (dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

AMDSFullImageDataHolder::AMDSFullImageDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint16 fastAxisSize, quint16 slowAxisSize, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, ((quint32)fastAxisSize)*((quint32)slowAxisSize), axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightImageDataHolder(dataType, fastAxisSize, slowAxisSize, this);
}

AMDSFullImageDataHolder::~AMDSFullImageDataHolder()
{
}
