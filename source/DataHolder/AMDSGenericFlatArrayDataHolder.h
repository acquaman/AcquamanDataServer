#ifndef AMDSGENERICFLATARRAYDATAHOLDER_H
#define AMDSGENERICFLATARRAYDATAHOLDER_H

#include "DataHolder/AMDSDataHolder.h"

class AMDSLightWeightGenericFlatArrayDataHolder : public AMDSLightWeightDataHolder
{
Q_OBJECT
public:
	AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	AMDSLightWeightGenericFlatArrayDataHolder(AMDSLightWeightGenericFlatArrayDataHolder* sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSLightWeightGenericFlatArrayDataHolder();

	/// Implement the function to return the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;
	/// Returns the flat data array
	inline AMDSFlatArray &dataArray() { return valueFlatArray_; }

	/// Implement the PLUS operation of AMDSLightWeightDataHolder, which will plus the value of the two instances of AMDSLightWeightDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder);
	/// Implement the Division operation of AMDSLightWeightDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor);

	/// Implement the function to clear the data of the instance
	virtual void clear() { valueFlatArray_.clear(); }
	/// implement the data function to read data from valueFlatArray_ and write data to the given outputArray
	virtual bool data(AMDSFlatArray *outputArray) const { return valueFlatArray_.resetTargetArrayAndReplaceData(outputArray); }
	/// implement the setData function to update valueFlatArray_ with given inputArray
	virtual void setData(AMDSFlatArray *inputArray) { inputArray->resetTargetArrayAndReplaceData(&valueFlatArray_); }
	/// implement the function to return the data string
	virtual QString printData();

	/// reimplement the function copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);

	/// NOTE: although this is public, but it is NOT suggested to use by the classes other than DataHolder classes
	/// reimplement the function to write this AMDSDataHolder to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) ;
	/// reimplement the function to read this AMDSDataHolder from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream);

protected:
	/// the data array instance
	AMDSFlatArray valueFlatArray_;
};

class AMDSFullGenericFlatArrayDataHolder : public AMDSFullDataHolder
{
Q_OBJECT
public:
	AMDSFullGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullGenericFlatArrayDataHolder();
};

////////////////////////////////////////
// AMDSLightWeightGenericFlatArrayDataHolder inline implementations
////////////////////////////////////////
quint64 AMDSLightWeightGenericFlatArrayDataHolder::spanSize() const{
	quint64 as64Bit = valueFlatArray_.size();
	return as64Bit;
}


#endif // AMDSGENERICFLATARRAYDATAHOLDER_H
