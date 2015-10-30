#ifndef AMDSIMAGEDATAHOLDER_H
#define AMDSIMAGEDATAHOLDER_H

#include "DataHolder/AMDSGenericFlatArrayDataHolder.h"

class AMDSLightWeightImageDataHolder : public AMDSLightWeightGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightImageDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint16 fastAxisSize = 1, quint16 slowAxisSize = 1, QObject *parent = 0);
	virtual ~AMDSLightWeightImageDataHolder();

	/// implement the function to return axes information
	virtual inline QList<AMDSAxisInfo> axes() const;
	/// implement the function to return the rank information (size of Axes)
	virtual inline quint8 rank() const { return 2; }
	/// implement the function to return the lenght of a the specified axis
	virtual inline quint32 size(int axisId) const;
	/// implement the function to return the lenght of axes
	virtual inline AMDSnDIndex size() const;

	/// reimplement the function to set the axes information, return True is the set is successful
	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

protected:
	/// reimplement the function to write this AMDSDataHolder to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) const;
	/// reimplement the function to read this AMDSDataHolder from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream);

protected:
	/// the size of fast axis
	quint16 fastAxisSize_;
};

class AMDSFullImageDataHolder : public AMDSFullGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullImageDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint16 fastAxisSize = 1, quint16 slowAxisSize = 1, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullImageDataHolder();
};

////////////////////////////////////////
// AMDSLightWeightImageDataHolder inline implementations
////////////////////////////////////////
QList<AMDSAxisInfo> AMDSLightWeightImageDataHolder::axes() const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	QList<AMDSAxisInfo> retVal;
	retVal << AMDSAxisInfo("FastAxis", fastAxisAs32, "Generic Fast Axis", "Generic Value");
	retVal << AMDSAxisInfo("SlowAxis", slowAxisAs32, "Generic Slow Axis", "Generic Value");
	return retVal;
}

quint32 AMDSLightWeightImageDataHolder::size(int axisId) const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	switch(axisId){
	case 0:
		return fastAxisAs32;
	case 1:
		return slowAxisAs32;
	default:
		return 0;
	}
}

AMDSnDIndex AMDSLightWeightImageDataHolder::size() const{
	quint32 fastAxisAs32 = (quint32)fastAxisSize_;
	quint32 slowAxisAs32 = valueFlatArray_.size()/fastAxisAs32;
	return AMDSnDIndex(fastAxisAs32, slowAxisAs32);
}


bool AMDSLightWeightImageDataHolder::setAxes(const QList<AMDSAxisInfo> &axes){
	Q_UNUSED(axes)
	return false;
}


#endif // AMDSIMAGEDATAHOLDER_H
