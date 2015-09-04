#ifndef AMDSDATAHOLDER_H
#define AMDSDATAHOLDER_H

#include <QObject>

#include "source/DataElement/AMDSDataTypeDefinitions.h"
#include "source/DataElement/AMDSAxisInfo.h"
#include "source/DataElement/AMDSnDIndex.h"
#include "source/DataElement/AMDSEventData.h"
#include "source/DataElement/AMDSFlatArray.h"


class AMDSDataStream;

#define AMDS_SERVER_ERR_DATA_HOLDER 20300
#define AMDS_SERVER_ERR_UNMATCH_DATA_HOLDER_FOR_PLUS 20301

class AMDSDataHolder : public QObject
{
Q_OBJECT
public:
	/// Definition of the AxesStyle
	enum AxesStyle{
		UniformAxes = 0,
		DefinedAxes = 1,
		InvalidAxes = 2
	};

	/// Definition of the DataType
	enum DataTypeStyle{
		UniformDataType = 0,
		DefinedDataType = 1,
		InvalidDataType = 2
	};

	AMDSDataHolder(QObject *parent = 0);
	virtual ~AMDSDataHolder();

	/// pure virtual function to return the axes style
	inline virtual AMDSDataHolder::AxesStyle axesStyle() const = 0;
	/// pure virtual function to return the dataType style
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const = 0;
	/// pure virtual function to return the eventData
	inline virtual const AMDSEventData* eventData() const = 0;
	/// pure virtual function to return the dataType
	virtual AMDSDataTypeDefinitions::DataType dataType() const = 0;

	/// pure virtual function to return the axes
	virtual inline QList<AMDSAxisInfo> axes() const = 0;
	/// pure virtual function to return the rank
	virtual inline quint8 rank() const = 0;
	/// Returns the length of all the axes
	virtual inline AMDSnDIndex size() const = 0;
	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const = 0;
	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const  = 0;

	/// pure virtual function to copy the data of the instance to outputArray
	virtual bool data(AMDSFlatArray *outputArray) const = 0;
	/// pure virtual function to copy the data of inputArray to local instance
	virtual void setData(AMDSFlatArray *inputArray) = 0;
	/// pure virtual function to return the data in the dataholder
	virtual QString printData() = 0;

	/// pure virtual function to set the Axes information
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes) = 0;

	/// pure virtual functions to get the eventTime and event time related comparation operations
	virtual QDateTime eventTime() = 0;
	virtual bool operator <(const QDateTime &rhs) = 0;
	virtual bool operator >(const QDateTime &rhs) = 0;
	virtual bool operator ==(const QDateTime &rhs) = 0;

	/// pure virtual function to define the PLUS operation of AMDSDataHolder, which will plus the value of the two instances of AMDSDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder) = 0;
	/// pure virtual function to define the Division operation of AMDSDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor) = 0;

	/// pure virtual function copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder) = 0;
	/// implement the = operation of AMDSDataHolder, which will copy the value of source instance to the target one
	virtual AMDSDataHolder& operator =(AMDSDataHolder &dataHolder);

	/// pure virtual function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered. By default the data type is encoded into the stream; however, this can be disabled and moved to a higher level if need be.
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType = true) const = 0;
	/// pure virtual function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered. By default the data type is decoded from the stream; however, passing a particular data type will assume that there is no data type encoded in the stream.
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType = AMDSDataTypeDefinitions::InvalidType) = 0;
};

class AMDSLightWeightDataHolder : public AMDSDataHolder
{
Q_OBJECT
public:
	AMDSLightWeightDataHolder(QObject *parent = 0);
	AMDSLightWeightDataHolder(AMDSLightWeightDataHolder &sourceLightWeightDataHolder, QObject *parent = 0);
	virtual ~AMDSLightWeightDataHolder();

	/// implement the axesStyle function
	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return AMDSDataHolder::UniformAxes; }
	/// implement the dataTypeStyle function
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const { return AMDSDataHolder::UniformDataType; }
	/// implement the eventData() function
	inline const AMDSEventData* eventData() const { return eventData_; }
	/// implement the dataType() function
	virtual AMDSDataTypeDefinitions::DataType dataType() const;
	/// implement the setAxes function
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	/// implement the event time and related comparison functions
	virtual QDateTime eventTime() { return eventData()->eventTime(); }
	virtual bool operator <(const QDateTime &rhs) { return eventData()->eventTime() < rhs; }
	virtual bool operator >(const QDateTime &rhs) { return eventData()->eventTime() > rhs; }
	virtual bool operator ==(const QDateTime &rhs) { return eventData()->eventTime() == rhs; }

	/// implement the function copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);

	/// implement the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// implement the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	/// the instance of event data, which provides the event information about the dataHolder
	AMDSEventData *eventData_;
};

class AMDSFullDataHolder : public AMDSDataHolder
{
Q_OBJECT
public:
	AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	AMDSFullDataHolder(AMDSFullDataHolder &sourceFullDataHolder, QObject *parent = 0);
	virtual ~AMDSFullDataHolder();

	/// implement the axesStyle() function
	inline virtual AMDSDataHolder::AxesStyle axesStyle() const { return axesStyle_; }
	/// implement the dataTypeStyle() function
	inline virtual AMDSDataHolder::DataTypeStyle dataTypeStyle() const { return dataTypeStyle_; }
	/// implement the eventData() function
	inline const AMDSEventData* eventData() const { return lightWeightDataHolder_->eventData(); }
	/// implement the dataType() function
	virtual AMDSDataTypeDefinitions::DataType dataType() const;

	/// implement the axes() function to retun the axes information
	virtual inline QList<AMDSAxisInfo> axes() const { return axes_; }
	/// implement the rank() function to retun size of the axes
	virtual inline quint8 rank() const { return axes_.count(); }
	/// implement the size() function to return the length of all the axes
	virtual inline AMDSnDIndex size() const;
	/// implement the size() function to return the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	virtual inline quint32 size(int axisId) const { return axes_.at(axisId).size(); }
	/// implement the spanSize() function to return the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;

	/// implement the data() function to copy the data of the instance to outputArray
	virtual bool data(AMDSFlatArray *outputValues) const { return lightWeightDataHolder_->data(outputValues); }
	/// implement the setData() function to copy the data of inputArray to local instance
	virtual void setData(AMDSFlatArray *inputValues) { return lightWeightDataHolder_->setData(inputValues); }
	/// implement the function to return the data string
	virtual QString printData() { return lightWeightDataHolder_->printData(); }
	/// implement the setAxes() function to set the axes information of the instance
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	/// implement the eventTime() function and related comparison operators
	virtual QDateTime eventTime() { return eventData()->eventTime(); }
	virtual bool operator <(const QDateTime &rhs) { return lightWeightDataHolder_->operator <(rhs); }
	virtual bool operator >(const QDateTime &rhs) { return lightWeightDataHolder_->operator >(rhs); }
	virtual bool operator ==(const QDateTime &rhs) { return lightWeightDataHolder_->operator ==(rhs); }

	/// implement the function copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);

	/// implement the PLUS operation of AMDSDataHolder, which will plus the value of the two instances of AMDSDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder) { return lightWeightDataHolder_->operator +(dataHolder); }
	/// implement the Division operation of AMDSDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor)  { return lightWeightDataHolder_->operator /(divisor); }

	/// implment the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// implent the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	/// instance of AMDSLightWightDataHolder as the real holder for data
	AMDSLightWeightDataHolder *lightWeightDataHolder_;

	/// instance of the Axis information
	QList<AMDSAxisInfo> axes_;
	/// defintion of the axes style
	AMDSDataHolder::AxesStyle axesStyle_;
	/// defintion of the data type style
	AMDSDataHolder::DataTypeStyle dataTypeStyle_;
};

////////////////////////////////////////
// AMDSDataHolder inline implementations
////////////////////////////////////////
bool AMDSLightWeightDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
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
	for(qint8 i=axes_.count()-1; i>=0; --i)
		aSize *= axes_.at(i).size();
	return aSize;
}

bool AMDSFullDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	axes_ = axes;
	return true;
}

#endif // AMDSDATAHOLDER_H
