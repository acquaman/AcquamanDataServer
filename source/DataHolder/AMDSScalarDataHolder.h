#ifndef AMDSSCALARDATAHOLDER_H
#define AMDSSCALARDATAHOLDER_H

#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"

class AMDSLightWeightScalarDataHolder : public AMDSLightWeightGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, QObject *parent = 0);
	Q_INVOKABLE AMDSLightWeightScalarDataHolder(AMDSLightWeightScalarDataHolder *sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSLightWeightScalarDataHolder();

	/// implement the function to return axes information
	virtual inline QList<AMDSAxisInfo> axes() const { return QList<AMDSAxisInfo>(); }
	/// implement the function to return the rank information (size of Axes)
	virtual inline quint8 rank() const { return 0; }
	/// implement the function to return the lenght of a the specified axis
	virtual inline quint32 size(int axisId) const;
	/// implement the function to return the lenght of axes
	virtual inline AMDSnDIndex size() const { return AMDSnDIndex(); }
	/// implement the spanSize() function to return the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const { return 1; }

	/// overload functions to set a single value
	virtual inline bool setSingleValue(qint8 singleValue);
	virtual inline bool setSingleValue(quint8 singleValue);
	virtual inline bool setSingleValue(qint16 singleValue);
	virtual inline bool setSingleValue(quint16 singleValue);
	virtual inline bool setSingleValue(qint32 singleValue);
	virtual inline bool setSingleValue(quint32 singleValue);
	virtual inline bool setSingleValue(qint64 singleValue);
	virtual inline bool setSingleValue(quint64 singleValue);
	virtual inline bool setSingleValue(float singleValue);
	virtual inline bool setSingleValue(double singleValue);

	/// reimplement the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// reimplement the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

class AMDSFullScalarDataHolder : public AMDSFullGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullScalarDataHolder();

	/// overload functions to set a single value
	virtual inline bool setSingleValue(qint8 singleValue);
	virtual inline bool setSingleValue(quint8 singleValue);
	virtual inline bool setSingleValue(qint16 singleValue);
	virtual inline bool setSingleValue(quint16 singleValue);
	virtual inline bool setSingleValue(qint32 singleValue);
	virtual inline bool setSingleValue(quint32 singleValue);
	virtual inline bool setSingleValue(qint64 singleValue);
	virtual inline bool setSingleValue(quint64 singleValue);
	virtual inline bool setSingleValue(float singleValue);
	virtual inline bool setSingleValue(double singleValue);

	/// reimplement the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// reimplement the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

};

////////////////////////////////////////
// AMDSLightWeightScalarDataHolder inline implementations
////////////////////////////////////////
quint32 AMDSLightWeightScalarDataHolder::size(int axisId) const{
	Q_UNUSED(axisId)
	return 0;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(qint8 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Signed8)
		return false;

	valueFlatArray_.vectorQint8()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(quint8 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Unsigned8)
		return false;

	valueFlatArray_.vectorQuint8()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(qint16 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Signed16)
		return false;

	valueFlatArray_.vectorQint16()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(quint16 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Unsigned16)
		return false;

	valueFlatArray_.vectorQuint16()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(qint32 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Signed32)
		return false;

	valueFlatArray_.vectorQint32()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(quint32 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Unsigned32)
		return false;

	valueFlatArray_.vectorQuint32()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(qint64 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Signed64)
		return false;

	valueFlatArray_.vectorQint64()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(quint64 singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Unsigned64)
		return false;

	valueFlatArray_.vectorQuint64()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(float singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Float)
		return false;

	valueFlatArray_.vectorFloat()[0] = singleValue;
	return true;
}

bool AMDSLightWeightScalarDataHolder::setSingleValue(double singleValue){
	if(valueFlatArray_.dataType() != AMDSDataTypeDefinitions::Double)
		return false;

	valueFlatArray_.vectorDouble()[0] = singleValue;
	return true;
}

////////////////////////////////////////
// AMDSFullScalarDataHolder inline implementations
////////////////////////////////////////
bool AMDSFullScalarDataHolder::setSingleValue(qint8 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(quint8 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(qint16 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(quint16 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(qint32 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(quint32 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(qint64 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(quint64 singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(float singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

bool AMDSFullScalarDataHolder::setSingleValue(double singleValue){
	AMDSLightWeightScalarDataHolder *lightWeightScalarDataHolder = qobject_cast<AMDSLightWeightScalarDataHolder*>(lightWeightDataHolder_);
	if(lightWeightScalarDataHolder)
		return lightWeightScalarDataHolder->setSingleValue(singleValue);
	return false;
}

#endif // AMDSSCALARDATAHOLDER_H
