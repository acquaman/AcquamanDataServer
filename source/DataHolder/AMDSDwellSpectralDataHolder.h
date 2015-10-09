#ifndef AMDSDWELLSPECTRALDATAHOLDER_H
#define AMDSDWELLSPECTRALDATAHOLDER_H

#include "DataElement/AMDSStatusData.h"
#include "DataHolder/AMDSSpectralDataHolder.h"

class AMDSDwellSpectralDataHolder: public AMDSLightWeightSpectralDataHolder
{
	Q_OBJECT
public:
	Q_INVOKABLE AMDSDwellSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	Q_INVOKABLE AMDSDwellSpectralDataHolder(AMDSDwellSpectralDataHolder *sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSDwellSpectralDataHolder();

	/// function to copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);
	/// Implement the PLUS operation of AMDSLightWeightDataHolder, which will plus the value of the two instances of AMDSLightWeightDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder);
	/// Implement the Division operation of AMDSLightWeightDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor);
	/// The status data relating to the detector response
	AMDSStatusData dwellStatusData() const;

public slots:
	/// Sets the status data for the histogram
	void setDwellStatusData(const AMDSStatusData &dwellStatusData);

protected:
	/// the dwell status
	AMDSStatusData dwellStatusData_;
};

#endif // AMDSDWELLSPECTRALDATAHOLDER_H
