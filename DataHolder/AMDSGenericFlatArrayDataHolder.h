#ifndef AMDSGENERICFLATARRAYDATAHOLDER_H
#define AMDSGENERICFLATARRAYDATAHOLDER_H

#include "DataHolder/AMDSDataHolder.h"

class AMDSLightWeightGenericFlatArrayDataHolder : public AMDSLightWeightDataHolder
{
Q_OBJECT
public:
	AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	virtual ~AMDSLightWeightGenericFlatArrayDataHolder();

	virtual inline QList<AMDSAxisInfo> axes() const = 0;
	virtual inline quint8 rank() const  = 0;
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const  = 0;
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const  = 0;
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;

	virtual inline bool data(AMDSFlatArray *outputValues) const;

	virtual inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	AMDSFlatArray valueFlatArray_;
};

class AMDSFullGenericFlatArrayDataHolder : public AMDSFullDataHolder
{
Q_OBJECT
public:
	AMDSFullGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullGenericFlatArrayDataHolder();

	virtual inline bool data(AMDSFlatArray *outputValues) const;

	virtual inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

quint64 AMDSLightWeightGenericFlatArrayDataHolder::spanSize() const{
	quint64 as64Bit = valueFlatArray_.size();
	return as64Bit;
}

bool AMDSLightWeightGenericFlatArrayDataHolder::data(AMDSFlatArray *outputValues) const{
	return valueFlatArray_.replaceData(outputValues);
}

void AMDSLightWeightGenericFlatArrayDataHolder::setData(AMDSFlatArray *inputValues){
	inputValues->copyData(&valueFlatArray_);
}

bool AMDSFullGenericFlatArrayDataHolder::data(AMDSFlatArray *outputValues) const{
	return lightWeightDataHolder_->data(outputValues);
}

void AMDSFullGenericFlatArrayDataHolder::setData(AMDSFlatArray *inputValues){
	lightWeightDataHolder_->setData(inputValues);
}

#endif // AMDSGENERICFLATARRAYDATAHOLDER_H
