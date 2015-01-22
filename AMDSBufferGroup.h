#ifndef AMDSBUFFERGROUP_H
#define AMDSBUFFERGROUP_H

#include <QObject>

#include "AMDSBuffer.h"
#include "AMDSAxisInfo.h"
#include "AMDSnDIndex.h"

class AMDSBufferGroupInfo
{
public:
	AMDSBufferGroupInfo(const QString& name, const QString& description, const QString& units = QString(), const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>());

	QString inline name() const { return name_; }

	QString inline description() const { return description_; }

	QString inline units() const { return units_; }

	QList<AMDSAxisInfo> inline axes() const { return axes_; }

	quint8 inline rank() const { return axes_.count(); }

	/// Returns the length of all the axes
	AMDSnDIndex inline size() const;

	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	quint32 inline size(int axisId) const { return axes_.at(axisId).size(); }

	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	quint64 inline spanSize() const;

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

class AMDSBufferGroup : public QObject
{
Q_OBJECT
public:
	AMDSBufferGroup(QObject *parent = 0);
};

#endif // AMDSBUFFERGROUP_H
