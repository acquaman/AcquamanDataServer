#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"

#include "source/Connection/AMDSDataStream.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSLightWeightGenericFlatArrayDataHolder::AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent) :
	AMDSLightWeightDataHolder(parent), valueFlatArray_(dataType, size)
{
}

AMDSLightWeightGenericFlatArrayDataHolder::~AMDSLightWeightGenericFlatArrayDataHolder()
{
}

AMDSDataHolder* AMDSLightWeightGenericFlatArrayDataHolder::operator +(AMDSDataHolder &dataHolder)
{
	AMDSLightWeightGenericFlatArrayDataHolder *addInputDataHolder = qobject_cast<AMDSLightWeightGenericFlatArrayDataHolder *>(&dataHolder);
	if (!addInputDataHolder) {
		AMDSErrorMon::alert(this, 0, QString("Invalid class type (%1) to do the PLUS operation").arg(dataHolder.metaObject()->className()));
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

AMDSFullGenericFlatArrayDataHolder::AMDSFullGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullDataHolder(axesStyle, dataTypeStyle, axes, parent)
{
	Q_UNUSED(dataType)
	Q_UNUSED(size)
}

AMDSFullGenericFlatArrayDataHolder::~AMDSFullGenericFlatArrayDataHolder()
{
}

bool AMDSFullGenericFlatArrayDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	return lightWeightDataHolder_->writeToDataStream(dataStream, encodeDataType);
}

bool AMDSFullGenericFlatArrayDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	return lightWeightDataHolder_->readFromDataStream(dataStream, decodeAsDataType);
}
