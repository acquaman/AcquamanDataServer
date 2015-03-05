#ifndef AMDSSCALARDATAHOLDER_H
#define AMDSSCALARDATAHOLDER_H

#include "AMDSDataHolder.h"

class AMDSScalarDataHolder : public AMDSDataHolder
{
public:
	AMDSScalarDataHolder();
	virtual ~AMDSScalarDataHolder();

	virtual inline quint8 rank() const { return 0; }
	virtual inline quint32 size(int axisId) const;
	virtual inline quint64 spanSize() const { return 1; }

	virtual inline bool data(AMDSFlatArray *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	inline void setSingleValue(double singleValue) { singleValue_ = singleValue; }
	inline void setData(AMDSFlatArray *inputValues);

protected:
	double singleValue_;
};

class AMDSFullScalarDataHolder : public AMDSFullDataHolder, public AMDSScalarDataHolder
{
public:
	AMDSFullScalarDataHolder(AMDSDataHolder::AxesStyle axesStyle, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>());
	virtual ~AMDSFullScalarDataHolder();

protected:
};

quint32 AMDSScalarDataHolder::size(int axisId) const{
	Q_UNUSED(axisId)
	return 0;
}

bool AMDSScalarDataHolder::data(AMDSFlatArray *outputValues) const{
	AMDSFlatArray asFlatArray = AMDSFlatArray(AMDSDataTypeDefinitions::Double, 1);
	asFlatArray.vectorDouble()[0] = singleValue_;
//	return asFlatArray.copyData(outputValues);
	return asFlatArray.replaceData(outputValues);
}

bool AMDSScalarDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

void AMDSScalarDataHolder::setData(AMDSFlatArray *inputValues){
	if(inputValues->dataType() != AMDSDataTypeDefinitions::Double)
		return;
	setSingleValue(inputValues->vectorDouble().at(0));
}

#endif // AMDSSCALARDATAHOLDER_H
