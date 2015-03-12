#ifndef AMDSSPECTRALDATAHOLDER_H
#define AMDSSPECTRALDATAHOLDER_H

#include "AMDSDataHolder.h"

class AMDSLightWeightSpectralDataHolder : public AMDSLightWeightDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	virtual ~AMDSLightWeightSpectralDataHolder();

	virtual inline quint8 rank() const { return 1; }
	virtual inline quint32 size(int axisId) const;
	virtual inline quint64 spanSize() const;

	virtual inline bool data(AMDSFlatArray *outputValues) const;
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

	inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

protected:
	AMDSFlatArray valueFlatArray_;
};

class AMDSFullSpectralDataHolder : public AMDSFullDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullSpectralDataHolder();

	virtual inline bool data(AMDSFlatArray *outputValues) const;

	inline void setData(AMDSFlatArray *inputValues);

	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);
};

quint32 AMDSLightWeightSpectralDataHolder::size(int axisId) const{
	if(axisId != 0)
		return 0;
	return valueFlatArray_.size();
}

quint64 AMDSLightWeightSpectralDataHolder::spanSize() const{
	return valueFlatArray_.size();
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

bool AMDSFullSpectralDataHolder::data(AMDSFlatArray *outputValues) const{
	return lightWeightDataHolder_->data(outputValues);
}

void AMDSFullSpectralDataHolder::setData(AMDSFlatArray *inputValues){
	AMDSLightWeightSpectralDataHolder *lightWeightSpectralDataHolder = qobject_cast<AMDSLightWeightSpectralDataHolder*>(lightWeightDataHolder_);
	if(lightWeightSpectralDataHolder)
		lightWeightSpectralDataHolder->setData(inputValues);
}

#endif // AMDSSPECTRALDATAHOLDER_H
