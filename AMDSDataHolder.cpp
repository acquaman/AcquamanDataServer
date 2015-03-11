#include "AMDSDataHolder.h"

#include "AMDSDataStream.h"
#include "AMDSDataHolderSupport.h"
#include "AMDSMetaObjectSupport.h"

AMDSDataHolder::AMDSDataHolder(QObject *parent) :
	QObject(parent)
{
}

AMDSDataHolder::~AMDSDataHolder()
{
}

AMDSLightWeightDataHolder::AMDSLightWeightDataHolder(QObject *parent) :
	AMDSDataHolder(parent)
{
	eventData_ = 0;
}

AMDSLightWeightDataHolder::~AMDSLightWeightDataHolder()
{
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

AMDSFullDataHolder::AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSDataHolder(parent)
{
	axesStyle_ = axesStyle;
	dataTypeStyle_ = dataTypeStyle;
	axes_ = axes;
	lightWeightDataHolder_ = 0;
}

AMDSFullDataHolder::~AMDSFullDataHolder()
{
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
