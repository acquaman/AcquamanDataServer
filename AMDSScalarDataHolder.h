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

	virtual inline bool data(double *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	inline void setSingleValue(double singleValue) { singleValue_ = singleValue; }

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

bool AMDSScalarDataHolder::data(double *outputValues) const{
	outputValues[0] = singleValue_;
	return true;
}

bool AMDSScalarDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

#endif // AMDSSCALARDATAHOLDER_H
