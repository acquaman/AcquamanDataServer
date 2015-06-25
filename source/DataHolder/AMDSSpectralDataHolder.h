#ifndef AMDSSPECTRALDATAHOLDER_H
#define AMDSSPECTRALDATAHOLDER_H

#include "source/DataHolder/AMDSGenericFlatArrayDataHolder.h"

class AMDSLightWeightSpectralDataHolder : public AMDSLightWeightGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	virtual ~AMDSLightWeightSpectralDataHolder();

	virtual inline QList<AMDSAxisInfo> axes() const;
	virtual inline quint8 rank() const { return 1; }
	virtual inline quint32 size(int axisId) const;
	virtual inline AMDSnDIndex size() const;

	virtual inline bool data(AMDSFlatArray *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	virtual inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

class AMDSFullSpectralDataHolder : public AMDSFullGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullSpectralDataHolder();

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

QList<AMDSAxisInfo> AMDSLightWeightSpectralDataHolder::axes() const{
	QList<AMDSAxisInfo> retVal;
	retVal << AMDSAxisInfo("Axis0", valueFlatArray_.size(), "Generic Axis 0", "Generic Value");
	return retVal;
}

quint32 AMDSLightWeightSpectralDataHolder::size(int axisId) const{
	if(axisId != 0)
		return 0;
	return valueFlatArray_.size();
}

AMDSnDIndex AMDSLightWeightSpectralDataHolder::size() const{
	return AMDSnDIndex(valueFlatArray_.size());
}

bool AMDSLightWeightSpectralDataHolder::data(AMDSFlatArray *outputValues) const{
	return valueFlatArray_.replaceData(outputValues);
}

bool AMDSLightWeightSpectralDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}

void AMDSLightWeightSpectralDataHolder::setData(AMDSFlatArray *inputValues){
	inputValues->copyData(&valueFlatArray_);
}

#endif // AMDSSPECTRALDATAHOLDER_H
