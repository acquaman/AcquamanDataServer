#include "AMDSGenericFlatArrayDataHolder.h"

#include "DataHolder/AMDSDataHolderSupport.h"
#include "util/AMDSRunTimeSupport.h"

AMDSLightWeightGenericFlatArrayDataHolder::AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent)
	:AMDSLightWeightDataHolder(parent), valueFlatArray_(dataType, size)
{
}

AMDSLightWeightGenericFlatArrayDataHolder::AMDSLightWeightGenericFlatArrayDataHolder(AMDSLightWeightGenericFlatArrayDataHolder* sourceDataHolder, QObject *parent)
	:AMDSLightWeightDataHolder(parent), valueFlatArray_(sourceDataHolder->valueFlatArray_.dataType(), sourceDataHolder->valueFlatArray_.size())
{
	cloneData(sourceDataHolder);
}

AMDSLightWeightGenericFlatArrayDataHolder::~AMDSLightWeightGenericFlatArrayDataHolder()
{
	valueFlatArray_.clear();
}

AMDSDataHolder* AMDSLightWeightGenericFlatArrayDataHolder::operator +(AMDSDataHolder &dataHolder)
{
	AMDSLightWeightGenericFlatArrayDataHolder *addInputDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder *>(&dataHolder);
	if (!addInputDataHolder) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SERVER_ERR_UNMATCH_DATA_HOLDER_FOR_PLUS, QString("Invalid class type (%1) to do the PLUS operation").arg(dataHolder.metaObject()->className()));
		return 0;
	}

	AMDSFlatArray data = dataArray() + addInputDataHolder->dataArray();

	AMDSDataHolder* targetDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(metaObject()->className());
	targetDataHolder->setData(&data);
	return targetDataHolder;
}

AMDSDataHolder* AMDSLightWeightGenericFlatArrayDataHolder::operator /(const quint32 divisor)
{
	AMDSFlatArray data = dataArray() / divisor;

	AMDSDataHolder* targetDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(metaObject()->className());
	targetDataHolder->setData(&data);
	return targetDataHolder;
}

QString AMDSLightWeightGenericFlatArrayDataHolder::toString() const
{
	return QString("%1 : %2").arg(eventTime().toString()).arg(valueFlatArray_.toString());
}

void AMDSLightWeightGenericFlatArrayDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSLightWeightDataHolder::cloneData(sourceDataHolder);

	AMDSLightWeightGenericFlatArrayDataHolder *sourceLightWeightGenericFlatArrayDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder *>(sourceDataHolder);
	sourceLightWeightGenericFlatArrayDataHolder->dataArray().resetTargetArrayAndReplaceData(&valueFlatArray_);
}

bool AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(QDataStream *dataStream)
{
	// This will take care of encoding and writing our eventData to the stream
	if(!AMDSLightWeightDataHolder::writeToDataStream(dataStream))
		return false;

	// write array data to datastream
	if (!valueFlatArray_.writeToDataStream(dataStream))
		return false;

	return true;
}

bool AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(QDataStream *dataStream){
	// decoding and instantiating our eventData from the stream (and deleting any old eventData memory we allocated)
	if(!AMDSLightWeightDataHolder::readFromDataStream(dataStream))
		return false;

	// read array data from the datastream
	if ( !valueFlatArray_.readFromDataStream(dataStream))
		return false;

	return true;
}


/// ============ implementation of AMDSFullGenericFlatArrayDataHolder ============
AMDSFullGenericFlatArrayDataHolder::AMDSFullGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullDataHolder(axesStyle, dataTypeStyle, axes, parent)
{
	Q_UNUSED(dataType)
	Q_UNUSED(size)
}

AMDSFullGenericFlatArrayDataHolder::~AMDSFullGenericFlatArrayDataHolder()
{
}
