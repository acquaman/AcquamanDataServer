#ifndef AMDSBUFFERGROUPINFO_H
#define AMDSBUFFERGROUPINFO_H

#include "AMDSAxisInfo.h"
#include "AMDSnDIndex.h"

class AMDSBufferGroupInfo
{
public:
	AMDSBufferGroupInfo(const QString& name = QString(), const QString& description = QString(), const QString& units = QString(), const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>());
	/// Copy constructor
	AMDSBufferGroupInfo(const AMDSBufferGroupInfo& other);
	/// Assignment operator
	AMDSBufferGroupInfo& operator=(const AMDSBufferGroupInfo& other);

	inline QString name() const { return name_; }
	inline QString description() const { return description_; }
	inline QString units() const { return units_; }

	inline QList<AMDSAxisInfo> axes() const { return axes_; }

	inline quint8 rank() const { return axes_.count(); }

	/// Returns the length of all the axes
	inline AMDSnDIndex size() const;

	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	inline quint32 size(int axisId) const { return axes_.at(axisId).size(); }

	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	inline quint64 spanSize() const;

	void setName(const QString &name) { name_ = name; }
	void setDescription(const QString &description) { description_ = description; }
	void setUnits(const QString &units) { units_ = units; }

	void setAxes(const QList<AMDSAxisInfo> &axes) { axes_ = axes; }

protected:
	QString name_;
	QString description_;
	QString units_;

	QList<AMDSAxisInfo> axes_;
};

AMDSnDIndex AMDSBufferGroupInfo::size() const {
	quint8 rank = axes_.count();
	AMDSnDIndex s(rank, AMDSnDIndex::DoNotInit);
	for(quint8 i=0; i<rank; ++i)
		s[i] = axes_.at(i).size();
	return s;
}

quint64 AMDSBufferGroupInfo::spanSize() const {
	quint64 aSize = 1;
	for(quint8 i=axes_.count()-1; i>=0; --i)
		aSize *= axes_.at(i).size();
	return aSize;
}

#endif // AMDSBUFFERGROUPINFO_H
