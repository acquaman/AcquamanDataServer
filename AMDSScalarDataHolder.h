#ifndef AMDSSCALARDATAHOLDER_H
#define AMDSSCALARDATAHOLDER_H

#include "AMDSDataHolder.h"

class AMDSLightWeightScalarDataHolder : public AMDSLightWeightDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightScalarDataHolder(QObject *parent = 0);
	virtual ~AMDSLightWeightScalarDataHolder();

	virtual inline quint8 rank() const { return 0; }
	virtual inline quint32 size(int axisId) const;
	virtual inline quint64 spanSize() const { return 1; }

	virtual inline bool data(AMDSFlatArray *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	inline void setSingleValue(double singleValue) { singleValue_ = singleValue; }
	inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	double singleValue_;
};

class AMDSFullScalarDataHolder : public AMDSFullDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullScalarDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullScalarDataHolder();

	virtual inline bool data(AMDSFlatArray *outputValues) const;

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
};

quint32 AMDSLightWeightScalarDataHolder::size(int axisId) const{
	Q_UNUSED(axisId)
	return 0;
}

bool AMDSLightWeightScalarDataHolder::data(AMDSFlatArray *outputValues) const{
	AMDSFlatArray asFlatArray = AMDSFlatArray(AMDSDataTypeDefinitions::Double, 1);
	asFlatArray.vectorDouble()[0] = singleValue_;
//	return asFlatArray.copyData(outputValues);
	return asFlatArray.replaceData(outputValues);
}

bool AMDSLightWeightScalarDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

void AMDSLightWeightScalarDataHolder::setData(AMDSFlatArray *inputValues){
	if(inputValues->dataType() != AMDSDataTypeDefinitions::Double)
		return;
	setSingleValue(inputValues->vectorDouble().at(0));
}

bool AMDSFullScalarDataHolder::data(AMDSFlatArray *outputValues) const{
	return lightWeightDataHolder_->data(outputValues);
}

#endif // AMDSSCALARDATAHOLDER_H
