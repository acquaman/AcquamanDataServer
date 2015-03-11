#ifndef AMDSDATAHOLDER_H
#define AMDSDATAHOLDER_H

#include <QObject>

#include "AMDSAxisInfo.h"
#include "AMDSnDIndex.h"
#include "AMDSEventData.h"
#include "AMDSDataTypeDefinitions.h"
#include "AMDSFlatArray.h"


class AMDSDataStream;

class AMDSDataHolder : public QObject
{
Q_OBJECT
public:
	enum AxesStyle{
		UniformAxes = 0,
		DefinedAxes = 1,
		InvalidAxes = 2
	};

	enum DataTypeStyle{
		UniformDataType = 0,
		DefinedDataType = 1,
		InvalidDataType = 2
	};

	AMDSDataHolder(QObject *parent = 0);
	virtual ~AMDSDataHolder();

	inline virtual AMDSDataHolder::AxesStyle axesStyle() const = 0;
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const = 0;
	inline virtual const AMDSEventData* eventData() const = 0;
	inline virtual AMDSDataTypeDefinitions::DataType dataType() const = 0;

	virtual inline QList<AMDSAxisInfo> axes() const = 0;
	virtual inline quint8 rank() const = 0;
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const = 0;
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const = 0;
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const  = 0;

	virtual inline bool data(AMDSFlatArray *outputValues) const = 0;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes) = 0;

	virtual inline bool operator <(const QDateTime &rhs) = 0;
	virtual inline bool operator >(const QDateTime &rhs) = 0;
	virtual inline bool operator ==(const QDateTime &rhs) = 0;

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered. By default the data type is encoded into the stream; however, this can be disabled and moved to a higher level if need be.
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType = true) const = 0;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered. By default the data type is decoded from the stream; however, passing a particular data type will assume that there is no data type encoded in the stream.
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType = AMDSDataTypeDefinitions::InvalidType) = 0;
};

class AMDSLightWeightDataHolder : public AMDSDataHolder
{
Q_OBJECT
public:
	AMDSLightWeightDataHolder(QObject *parent = 0);
	virtual ~AMDSLightWeightDataHolder();

	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return AMDSDataHolder::UniformAxes; }
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const { return AMDSDataHolder::UniformDataType; }
	inline const AMDSEventData* eventData() const { return eventData_; }
	inline virtual AMDSDataTypeDefinitions::DataType dataType() const;

	virtual inline QList<AMDSAxisInfo> axes() const { return QList<AMDSAxisInfo>(); }
	virtual inline quint8 rank() const { return 0; }
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const { return AMDSnDIndex(); }
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const;
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const { return 0; }

	virtual inline bool data(AMDSFlatArray *outputValues) const = 0;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	virtual inline bool operator <(const QDateTime &rhs);
	virtual inline bool operator >(const QDateTime &rhs);
	virtual inline bool operator ==(const QDateTime &rhs);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	AMDSEventData *eventData_;
};

class AMDSFullDataHolder : public AMDSDataHolder
{
Q_OBJECT
public:
	AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullDataHolder();

	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return axesStyle_; }
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const { return dataTypeStyle_; }
	inline const AMDSEventData* eventData() const { return lightWeightDataHolder_->eventData(); }
	inline virtual AMDSDataTypeDefinitions::DataType dataType() const;

	virtual inline QList<AMDSAxisInfo> axes() const { return axes_; }
	virtual inline quint8 rank() const { return axes_.count(); }
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const;
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const { return axes_.at(axisId).size(); }
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;

	virtual inline bool data(AMDSFlatArray *outputValues) const = 0;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	virtual inline bool operator <(const QDateTime &rhs) { return lightWeightDataHolder_->operator <(rhs); }
	virtual inline bool operator >(const QDateTime &rhs) { return lightWeightDataHolder_->operator >(rhs); }
	virtual inline bool operator ==(const QDateTime &rhs) { return lightWeightDataHolder_->operator ==(rhs); }

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	AMDSLightWeightDataHolder *lightWeightDataHolder_;
	QList<AMDSAxisInfo> axes_;
	AMDSDataHolder::AxesStyle axesStyle_;
	AMDSDataHolder::DataTypeStyle dataTypeStyle_;
};

////////////////////////////////////////
// AMDSDataHolder inline implementations
////////////////////////////////////////
quint32 AMDSLightWeightDataHolder::size(int axisId) const{
	Q_UNUSED(axisId)
	return 0;
}

AMDSDataTypeDefinitions::DataType AMDSLightWeightDataHolder::dataType() const{
	if(dataTypeStyle() != AMDSDataHolder::UniformDataType)
		return AMDSDataTypeDefinitions::InvalidType;

	AMDSFlatArray oneFlatArray;
	data(&oneFlatArray);
	return oneFlatArray.dataType();
}

bool AMDSLightWeightDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

bool AMDSLightWeightDataHolder::operator <(const QDateTime &rhs){
	return eventData()->eventTime() < rhs;
}

bool AMDSLightWeightDataHolder::operator >(const QDateTime &rhs){
	return eventData()->eventTime() > rhs;
}

bool AMDSLightWeightDataHolder::operator ==(const QDateTime &rhs){
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

AMDSDataTypeDefinitions::DataType AMDSFullDataHolder::dataType() const{
	if(dataTypeStyle() != AMDSDataHolder::UniformDataType)
		return AMDSDataTypeDefinitions::InvalidType;

	AMDSFlatArray oneFlatArray;
	data(&oneFlatArray);
	return oneFlatArray.dataType();
}

bool AMDSFullDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	axes_ = axes;
	return true;
}

#endif // AMDSDATAHOLDER_H
