#ifndef AMDSSPECTRALDATAHOLDER_H
#define AMDSSPECTRALDATAHOLDER_H

#include "DataHolder/AMDSGenericFlatArrayDataHolder.h"
#include "DataElement/AMDSDwellStatusData.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDSRunTimeSupport.h"

class AMDSLightWeightSpectralDataHolder : public AMDSLightWeightGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	Q_INVOKABLE AMDSLightWeightSpectralDataHolder(AMDSLightWeightSpectralDataHolder *sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSLightWeightSpectralDataHolder();

	/// implement the function to return axes information
	virtual inline QList<AMDSAxisInfo> axes() const;
	/// implement the function to return the rank information (size of Axes)
	virtual inline quint8 rank() const { return 1; }
	/// implement the function to return the lenght of a the specified axis
	virtual inline quint32 size(int axisId) const;
	/// implement the function to return the lenght of axes
	virtual inline AMDSnDIndex size() const;
};

class AMDSFullSpectralDataHolder : public AMDSFullGenericFlatArrayDataHolder
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, AMDSDataHolder::AxesStyle axesStyle = AMDSDataHolder::UniformAxes, AMDSDataHolder::DataTypeStyle dataTypeStyle = AMDSDataHolder::UniformDataType, const QList<AMDSAxisInfo>& axes = QList<AMDSAxisInfo>(), QObject *parent = 0);
	virtual ~AMDSFullSpectralDataHolder();
};

class AMDSDwellSpectralDataHolder: public AMDSLightWeightSpectralDataHolder
{
	Q_OBJECT
public:
	Q_INVOKABLE AMDSDwellSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	Q_INVOKABLE AMDSDwellSpectralDataHolder(AMDSDwellSpectralDataHolder *sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSDwellSpectralDataHolder();

	/// Implement the PLUS operation of AMDSLightWeightDataHolder, which will plus the value of the two instances of AMDSLightWeightDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder);
	/// Implement the Division operation of AMDSLightWeightDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor);

	/// reimplement the function to clear the data of the instance
	virtual void clear();
	/// implement the function to return the data string
	virtual QString printData();

	/// function to copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);

	/// The status data relating to the detector response
	inline AMDSDwellStatusData dwellStatusData() const { return dwellStatusData_; }

public slots:
	/// Sets the status data for the histogram
	void setDwellStatusData(const AMDSDwellStatusData &dwellStatusData);

protected:
	/// reimplement the function to write this AMDSDataHolder to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) ;
	/// reimplement the function to read this AMDSDataHolder from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream);

protected:
	/// the dwell status
	AMDSDwellStatusData dwellStatusData_;
};

////////////////////////////////////////
// AMDSLightWeightSpectralDataHolder inline implementations
////////////////////////////////////////
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

#endif // AMDSSPECTRALDATAHOLDER_H
