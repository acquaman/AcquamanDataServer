#ifndef AMDSDATAHOLDER_H
#define AMDSDATAHOLDER_H

#include "AMDSAxisInfo.h"
#include "AMDSnDIndex.h"
#include "AMDSEventData.h"

class AMDSDataHolder
{
public:
	enum AxesStyle{
		UniformAxes = 0,
		DefinedAxes = 1,
		InvalidAxes = 2
	};

	AMDSDataHolder();
	virtual ~AMDSDataHolder();

	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return AMDSDataHolder::UniformAxes; }
	inline const AMDSEventData* eventData() const { return eventData_; }

	virtual inline QList<AMDSAxisInfo> axes() const { return QList<AMDSAxisInfo>(); }
	virtual inline quint8 rank() const { return 0; }
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const { return AMDSnDIndex(); }
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const;
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const { return 0; }

	virtual inline bool data(double *outputValues) const = 0;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	virtual inline bool operator <(const QDateTime &rhs);
	virtual inline bool operator >(const QDateTime &rhs);
	virtual inline bool operator ==(const QDateTime &rhs);

protected:
	AMDSEventData *eventData_;
};

class AMDSFullDataHolder : public AMDSDataHolder
{
public:
	AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>());
	virtual ~AMDSFullDataHolder();

	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return axesStyle_; }

	virtual inline QList<AMDSAxisInfo> axes() const { return axes_; }
	virtual inline quint8 rank() const { return axes_.count(); }
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const;
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const { return axes_.at(axisId).size(); }
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;

	virtual inline bool data(double *outputValues) const = 0;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

protected:
	QList<AMDSAxisInfo> axes_;
	AMDSDataHolder::AxesStyle axesStyle_;
};

////////////////////////////////////////
// AMDSDataHolder inline implementations
////////////////////////////////////////
quint32 AMDSDataHolder::size(int axisId) const{
	Q_UNUSED(axisId)
	return 0;
}

bool AMDSDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return true;
}

bool AMDSDataHolder::operator <(const QDateTime &rhs){
	return eventData()->eventTime() < rhs;
}

bool AMDSDataHolder::operator >(const QDateTime &rhs){
	return eventData()->eventTime() > rhs;
}

bool AMDSDataHolder::operator ==(const QDateTime &rhs){
	return eventData()->eventTime() == rhs;
}

////////////////////////////////////////
// AMDSFullDataHolder inline implementations
////////////////////////////////////////
AMDSnDIndex AMDSFullDataHolder::size() const {
	quint8 rank = axes_.count();
	AMDSnDIndex s(rank, AMDSnDIndex::DoNotInit);
	for(quint8 i=0; i<rank; ++i)
		s[i] = axes_.at(i).size();
	return s;
}

quint64 AMDSFullDataHolder::spanSize() const {
	quint64 aSize = 1;
	for(quint8 i=axes_.count()-1; i>=0; --i)
		aSize *= axes_.at(i).size();
	return aSize;
}

bool AMDSFullDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	axes_ = axes;
	return true;
}

#endif // AMDSDATAHOLDER_H
