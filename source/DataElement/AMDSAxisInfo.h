#ifndef AMDSAXISINFO_H
#define AMDSAXISINFO_H

#include <QtGlobal>
#include <QString>
#include <QDataStream>

class AMDSAxisInfo
{
public:
	/// Constructor.
	AMDSAxisInfo(const QString& name, quint32 size, const QString& description = QString(), const QString& units = QString(), bool isUniform = true );
	AMDSAxisInfo(const AMDSAxisInfo &axisInfo );
	/// Destructor.
	virtual ~AMDSAxisInfo();

	/// read data from dataStream and write to the dwellStatusData
	bool readFromDataStream(QDataStream *dataStream);
	/// write status data of the dataArray to dataStream
	bool writeToDataStream(QDataStream *dataStream);
	/// return the axis information as string
	QString toString() const;

	/// Size: number of elements along the axis
	inline quint32 size() const { return size_; }
	/// A unique (programmer-specific) name for the axis
	inline QString name() const { return name_; }
	/// A human-readable description
	inline QString description() const { return description_; }
	/// Units of the axis
	inline QString units() const { return units_; }
	/// Indicates that the independent values along the axis are uniformly spaced with a constant starting value and increment
	inline bool isUniform() const { return isUniform_; }
	/// Relevent only when isUniform is true... these provide a fast way to compute the axis' independent values
	inline qint16 start() const { return start_; }
	/// Return the incrent information of the Axis
	inline quint16 increment() const { return increment_; }

	/// set the number of elements along the axis
	inline void setSize(quint32 size) { size_ = size; }
	/// set the naem of the axis
	inline void setName(const QString &name) { name_ = name; }
	/// set the description of the axis
	inline void setDescription(const QString &description) { description_ = description; }
	/// set the units of the axis
	inline void setUnits(const QString &units) { units_ = units; }
	/// set whether the values are uniformly spaces or not
	inline void setIsUniform(bool isUniform) { isUniform_ = isUniform; }
	inline void setStart(qint16 start) { start_ = start; }
	/// set the increment information
	inline void setIncrement(quint16 increment) { increment_ = increment; }

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
