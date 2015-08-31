#ifndef AMDSGENERICFLATARRAYDATAHOLDER_H
#define AMDSGENERICFLATARRAYDATAHOLDER_H

#include "source/DataHolder/AMDSDataHolder.h"

class AMDSLightWeightGenericFlatArrayDataHolder : public AMDSLightWeightDataHolder
{
Q_OBJECT
public:
	AMDSLightWeightGenericFlatArrayDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	virtual ~AMDSLightWeightGenericFlatArrayDataHolder();

	/// Implement the function to return the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	virtual inline quint64 spanSize() const;

	/// Implement the PLUS operation of AMDSLightWeightDataHolder, which will plus the value of the two instances of AMDSLightWeightDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder);
	/// Implement the Division operation of AMDSLightWeightDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor);

	/// implement the data function to read data from valueFlatArray_ and write data to the given outputArray
	virtual inline bool data(AMDSFlatArray *outputArray) const { return valueFlatArray_.resetTargetArrayAndReplaceData(outputArray); }
	/// implement the setData function to update valueFlatArray_ with given inputArray
	virtual inline void setData(AMDSFlatArray *inputArray) { inputArray->copyDataToTargetArray(&valueFlatArray_); }
	/// implement the function to return the data string
	virtual inline QString printData() { return valueFlatArray_.printData(); }

	/// reimplement the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// reimplement the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

	/// Returns the flat data array
	inline AMDSFlatArray &dataArray() { return valueFlatArray_; }

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

	/// Reimplement the function to write this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// reimplement the function to read this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

////////////////////////////////////////
// AMDSLightWeightGenericFlatArrayDataHolder inline implementations
////////////////////////////////////////
quint64 AMDSLightWeightGenericFlatArrayDataHolder::spanSize() const{
	quint64 as64Bit = valueFlatArray_.size();
	return as64Bit;
}


#endif // AMDSGENERICFLATARRAYDATAHOLDER_H
