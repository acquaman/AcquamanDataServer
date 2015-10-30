#include "AMDSDataHolder.h"

#include "DataHolder/AMDSDataHolderSupport.h"
#include "DataElement/AMDSEventDataSupport.h"
#include "util/AMDSMetaObjectSupport.h"

// ======== implementation of AMDSDataHolder ===========
AMDSDataHolder *AMDSDataHolder::decodeAndInstantiateDataHolder(QDataStream *dataStream)
{
	QString dataHolderClassName;
	*dataStream >> dataHolderClassName;
	if(dataStream->status() != QDataStream::Ok)
		return 0;

	AMDSDataHolder *dataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(dataHolderClassName);
	if (dataHolder) {
		if (!dataHolder->readFromDataStream(dataStream))
			dataHolder = 0;
	}

	return dataHolder;
}

bool AMDSDataHolder::encodeAndwriteDataHolder(QDataStream *dataStream, AMDSDataHolder *dataHolder)
{
	return dataHolder->writeToDataStream(dataStream);
}

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

bool AMDSDataHolder::writeToDataStream(QDataStream *dataStream) const
{

	*dataStream << QString(metaObject()->className());
	if (dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSDataHolder::readFromDataStream(QDataStream *dataStream)
{
	Q_UNUSED(dataStream)

	/// NOTE: the class name is read out in the static function

	return true;
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

bool AMDSLightWeightDataHolder::writeToDataStream(QDataStream *dataStream) const
{
	if (! AMDSDataHolder::writeToDataStream(dataStream)) {
		return false;
	}

	if(!eventData_->writeToDataStream(dataStream))
		return false;

	return true;
}

bool AMDSLightWeightDataHolder::readFromDataStream(QDataStream *dataStream)
{
	if (! AMDSDataHolder::readFromDataStream(dataStream))
		return false;

	if(eventData_)
		eventData_->deleteLater();

	eventData_ = AMDSEventData::decodeAndInstantiateEventData(dataStream);
	if(eventData_ == 0)
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

bool AMDSFullDataHolder::writeToDataStream(QDataStream *dataStream) const
{
	if (!AMDSDataHolder::writeToDataStream(dataStream)) {
		return false;
	}

	/// write the lightWeightDataHolder to dataStream
	if(!lightWeightDataHolder_->writeToDataStream(dataStream))
		return false;

	/// write the data of AMDSFullDataHolder
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
		if (!axes_.at(x).writeToDataStream(dataStream))
			return false;
	}

	return true;
}

bool AMDSFullDataHolder::readFromDataStream(QDataStream *dataStream)
{
	if (!AMDSDataHolder::readFromDataStream(dataStream))
		return false;

	/// initialize and read the lightWeightDataHolder
	if(lightWeightDataHolder_)
		lightWeightDataHolder_->deleteLater();

	AMDSDataHolder *asDataHolder = AMDSDataHolder::decodeAndInstantiateDataHolder(dataStream);
	if(AMDSMetaObjectSupport::inheritsClass(asDataHolder->metaObject(), &(AMDSLightWeightDataHolder::staticMetaObject), &(AMDSDataHolder::staticMetaObject)))
		lightWeightDataHolder_ = qobject_cast<AMDSLightWeightDataHolder*>(asDataHolder);
	else
		return false;

	/// read the data of AMDSFullDataHolder
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
		oneAxisInfo.readFromDataStream(dataStream);
		if(oneAxisInfo.name() == "Invalid")
			return false;
		readAxes.append(oneAxisInfo);
	}

	axesStyle_ = (AMDSDataHolder::AxesStyle)readAxesStyleAsInt;
	dataTypeStyle_ = (AMDSDataHolder::DataTypeStyle)readDataTypeStyleAsInt;
	setAxes(readAxes);

	return true;
}
