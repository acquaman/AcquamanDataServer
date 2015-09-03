#ifndef AMDSAXISINFO_H
#define AMDSAXISINFO_H

#include <QtGlobal>
#include <QString>

class AMDSAxisInfo
{
public:
	/// Constructor.
	AMDSAxisInfo(const QString& name, quint32 size, const QString& description = QString(), const QString& units = QString(), bool isUniform = true );
	/// Destructor.
	virtual ~AMDSAxisInfo();

	/// Size: number of elements along the axis
	quint32 inline size() const { return size_; }
	/// A unique (programmer-specific) name for the axis
	QString inline name() const { return name_; }
	/// A human-readable description
	QString inline description() const { return description_; }
	/// Units of the axis
	QString inline units() const { return units_; }
	/// Indicates that the independent values along the axis are uniformly spaced with a constant starting value and increment
	bool inline isUniform() const { return isUniform_; }
	/// Relevent only when isUniform is true... these provide a fast way to compute the axis' independent values
	qint16 inline start() const { return start_; }
	/// Return the incrent information of the Axis
	quint16 inline increment() const { return increment_; }

	/// set the number of elements along the axis
	void setSize(quint32 size) { size_ = size; }
	/// set the naem of the axis
	void setName(const QString &name) { name_ = name; }
	/// set the description of the axis
	void setDescription(const QString &description) { description_ = description; }
	/// set the units of the axis
	void setUnits(const QString &units) { units_ = units; }
	/// set whether the values are uniformly spaces or not
	void setIsUniform(bool isUniform) { isUniform_ = isUniform; }
	void setStart(qint16 start) { start_ = start; }
	/// set the increment information
	void setIncrement(quint16 increment) { increment_ = increment; }

	/// return the axis information as string
	QString toString();

protected:
	/// Size: number of elements along the axis
	quint32 size_;
	/// A unique (programmer-specific) name for the axis
	QString name_;
	/// A human-readable description
	QString description_;
	/// Units of the axis
	QString units_;
	/// Indicates that the independent values along the axis are uniformly spaced with a constant starting value and increment
	bool isUniform_;
	/// Relevent only when isUniform is true... these provide a fast way to compute the axis' independent values
	qint16 start_;
	quint16 increment_;
};

#endif // AMDSAXISINFO_H
