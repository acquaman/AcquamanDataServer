#ifndef AMDSIMAGEDATAHOLDER_H
#define AMDSIMAGEDATAHOLDER_H

#include "DataHolder/AMDSGenericFlatArrayDataHolder.h"

class AMDSLightWeightImageDataHolder : public AMDSLightWeightGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightImageDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint16 fastAxisSize = 1, quint16 slowAxisSize = 1, QObject *parent = 0);
	virtual ~AMDSLightWeightImageDataHolder();

	virtual inline QList<AMDSAxisInfo> axes() const;
	virtual inline quint8 rank() const { return 2; }
	virtual inline quint32 size(int axisId) const;
	virtual inline AMDSnDIndex size() const;

	virtual inline bool data(AMDSFlatArray *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	virtual inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	quint16 fastAxisSize_;
};

class AMDSFullImageDataHolder : public AMDSFullGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullImageDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint16 fastAxisSize = 1, quint16 slowAxisSize = 1, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullImageDataHolder();
};

QList<AMDSAxisInfo> AMDSLightWeightImageDataHolder::axes() const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	QList<AMDSAxisInfo> retVal;
	retVal << AMDSAxisInfo("FastAxis", fastAxisAs32, "Generic Fast Axis", "Generic Value");
	retVal << AMDSAxisInfo("SlowAxis", slowAxisAs32, "Generic Slow Axis", "Generic Value");
	return retVal;
}

quint32 AMDSLightWeightImageDataHolder::size(int axisId) const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	switch(axisId){
	case 0:
		return fastAxisAs32;
	case 1:
		return slowAxisAs32;
	default:
		return 0;
	}
}

AMDSnDIndex AMDSLightWeightImageDataHolder::size() const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	return AMDSnDIndex(fastAxisAs32, slowAxisAs32);
}

bool AMDSLightWeightImageDataHolder::data(AMDSFlatArray *outputValues) const{
	return valueFlatArray_.replaceData(outputValues);
}

bool AMDSLightWeightImageDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

void AMDSLightWeightImageDataHolder::setData(AMDSFlatArray *inputValues){
	inputValues->copyData(&valueFlatArray_);
}

#endif // AMDSIMAGEDATAHOLDER_H
