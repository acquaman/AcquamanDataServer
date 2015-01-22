#ifndef AMDSAXISINFO_H
#define AMDSAXISINFO_H

#include <QtGlobal>
#include <QString>

class AMDSAxisInfo
{
public:
	/// Constructor.
	AMDSAxisInfo(const QString& name, quint32 size, const QString& description = QString(), const QString& units = QString() );
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
	quint16 inline increment() const { return increment_; }

	void setSize(quint32 size) { size_ = size; }
	void setName(const QString &name) { name_ = name; }
	void setDescription(const QString &description) { description_ = description; }
	void setUnits(const QString &units) { units_ = units; }
	void setIsUniform(bool isUniform) { isUniform_ = isUniform; }
	void setStart(qint16 start) { start_ = start; }
	void setIncrement(quint16 increment) { increment_ = increment; }

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
