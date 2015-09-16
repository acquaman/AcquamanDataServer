#include "AMDSDataHolder.h"

#include "source/Connection/AMDSDataStream.h"
#include "source/DataHolder/AMDSDataHolderSupport.h"
#include "source/DataElement/AMDSEventDataSupport.h"
#include "source/util/AMDSMetaObjectSupport.h"

// ======== implementation of AMDSDataHolder ===========
AMDSDataHolder::AMDSDataHolder(QObject *parent)
	:QObject(parent)
{
}

AMDSDataHolder::~AMDSDataHolder()
{
}

AMDSDataHolder& AMDSDataHolder::operator =(AMDSDataHolder &dataHolder)
{
	if (this != &dataHolder) {
		cloneData(&dataHolder);
	}

	return (*this);
}

// ======== implementation of AMDSLightWeightDataHolder ===========
AMDSLightWeightDataHolder::AMDSLightWeightDataHolder(QObject *parent) :
	AMDSDataHolder(parent)
{
	eventData_ = 0;
}

AMDSLightWeightDataHolder::AMDSLightWeightDataHolder(AMDSLightWeightDataHolder &sourceLightWeightDataHolder, QObject *parent)
	:AMDSDataHolder(parent)
{
	(*this) = sourceLightWeightDataHolder;
}

AMDSLightWeightDataHolder::~AMDSLightWeightDataHolder()
{
	if (eventData_) {
		eventData_->deleteLater();
		eventData_ = 0;
	}
}

AMDSDataTypeDefinitions::DataType AMDSLightWeightDataHolder::dataType() const{
	if(dataTypeStyle() != AMDSDataHolder::UniformDataType)
		return AMDSDataTypeDefinitions::InvalidType;

	AMDSFlatArray oneFlatArray;
	data(&oneFlatArray);
	return oneFlatArray.dataType();
}

void AMDSLightWeightDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSLightWeightDataHolder *sourceLightWeightDataHolder = qobject_cast<AMDSLightWeightDataHolder *>(sourceDataHolder);
	if (sourceLightWeightDataHolder) {
		if (eventData_)
			eventData_->deleteLater();

		eventData_ = AMDSEventDataSupport::instantiateEventDataFromInstance(sourceLightWeightDataHolder->eventData());
		(*eventData_) = *(sourceLightWeightDataHolder->eventData()); // copy the values of the eventData over
	}
}

bool AMDSLightWeightDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	Q_UNUSED(encodeDataType)
	dataStream->encodeEventDataType(*eventData_);
	if(!eventData_->writeToDataStream(dataStream))
		return false;

	return true;
}

bool AMDSLightWeightDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	Q_UNUSED(decodeAsDataType)
	if(eventData_)
		eventData_->deleteLater();
	eventData_ = dataStream->decodeAndInstantiateEventData();
	if(!eventData_->readFromDataStream(dataStream))
		return false;

	return true;
}

// ======== implementation of AMDSFullDataHolder ===========
AMDSFullDataHolder::AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent)
	: AMDSDataHolder(parent)
{
	axesStyle_ = axesStyle;
	dataTypeStyle_ = dataTypeStyle;
	axes_ = axes;
	lightWeightDataHolder_ = 0;
}

AMDSFullDataHolder::AMDSFullDataHolder(AMDSFullDataHolder &sourceFullDataHolder, QObject *parent)
	:AMDSDataHolder(parent)
{
	(*this) = sourceFullDataHolder;
}

AMDSFullDataHolder::~AMDSFullDataHolder()
{
	if (lightWeightDataHolder_) {
		lightWeightDataHolder_->deleteLater();
		lightWeightDataHolder_ = 0;
	}

	axes_.clear();
}

AMDSDataTypeDefinitions::DataType AMDSFullDataHolder::dataType() const{
	if(dataTypeStyle() != AMDSDataHolder::UniformDataType)
		return AMDSDataTypeDefinitions::InvalidType;

	AMDSFlatArray oneFlatArray;
	data(&oneFlatArray);
	return oneFlatArray.dataType();
}

void AMDSFullDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSFullDataHolder *sourceFullDataHolder = qobject_cast<AMDSFullDataHolder *>(sourceDataHolder);
	if (sourceFullDataHolder) {
		axes_.clear();
		if (lightWeightDataHolder_)
			lightWeightDataHolder_->deleteLater();

		AMDSDataHolder *newDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromInstance(sourceFullDataHolder->lightWeightDataHolder());
		lightWeightDataHolder_ = qobject_cast<AMDSLightWeightDataHolder *>(newDataHolder);
		(*lightWeightDataHolder_) = (*sourceFullDataHolder->lightWeightDataHolder());

		foreach (AMDSAxisInfo axisInfo, sourceFullDataHolder->axes()) {
			AMDSAxisInfo copyAxisInfo(axisInfo);
			axes_.append(copyAxisInfo);
		}

		axesStyle_ = sourceFullDataHolder->axesStyle();
		dataTypeStyle_ = sourceFullDataHolder->dataTypeStyle();
	}
}

bool AMDSFullDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	dataStream->encodeDataHolderType(*lightWeightDataHolder_);
	if(!lightWeightDataHolder_->writeToDataStream(dataStream, encodeDataType))
		return false;

	*dataStream << (quint8)axesStyle_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << (quint8)dataTypeStyle_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	quint8 axesCount = axes_.count();
	*dataStream << axesCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	for(int x = 0, size = axes_.count(); x < size; x++){
		dataStream->write(axes_.at(x));
		if(dataStream->status() != QDataStream::Ok)
			return false;
	}

	return true;
}

bool AMDSFullDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	if(lightWeightDataHolder_)
		lightWeightDataHolder_->deleteLater();

	AMDSDataHolder *asDataHolder = dataStream->decodeAndInstantiateDataHolder();
	if(AMDSMetaObjectSupport::inheritsClass(asDataHolder->metaObject(), &(AMDSLightWeightDataHolder::staticMetaObject), &(AMDSDataHolder::staticMetaObject)))
		lightWeightDataHolder_ = qobject_cast<AMDSLightWeightDataHolder*>(asDataHolder);
	else
		return false;

	if(!lightWeightDataHolder_->readFromDataStream(dataStream, decodeAsDataType))
		return false;

	quint8 readAxesStyleAsInt;
	quint8 readDataTypeStyleAsInt;
	quint8 readAxesCount;
	QList<AMDSAxisInfo> readAxes;

	*dataStream >> readAxesStyleAsInt;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readDataTypeStyleAsInt;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readAxesCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	for(int x = 0, size = readAxesCount; x < size; x++){
		AMDSAxisInfo oneAxisInfo("Invalid", 0);
		dataStream->read(oneAxisInfo);
		if(oneAxisInfo.name() == "Invalid")
			return false;
		readAxes.append(oneAxisInfo);
	}

	AMDSDataHolder::AxesStyle readAxesStyle = (AMDSDataHolder::AxesStyle)readAxesStyleAsInt;
	AMDSDataHolder::DataTypeStyle readDataTypeStyle = (AMDSDataHolder::DataTypeStyle)readDataTypeStyleAsInt;

	axesStyle_ = readAxesStyle;
	dataTypeStyle_ = readDataTypeStyle;
	setAxes(readAxes);

	return false;
}
