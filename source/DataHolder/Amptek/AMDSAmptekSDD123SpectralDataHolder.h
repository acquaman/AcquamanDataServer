#ifndef AMDSAMPTEKSDD123SPECTRALDATAHOLDER_H
#define AMDSAMPTEKSDD123SPECTRALDATAHOLDER_H

#include "DataHolder/AMDSSpectralDataHolder.h"
#include "Detector/Amptek/AmptekEventDefinitions.h"

class AMDSAmptekSDD123SpectralDataHolder: public AMDSLightWeightSpectralDataHolder
{
	Q_OBJECT
public:
	Q_INVOKABLE AMDSAmptekSDD123SpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::Double, quint32 size = 2, QObject *parent = 0);
	Q_INVOKABLE AMDSAmptekSDD123SpectralDataHolder(AMDSAmptekSDD123SpectralDataHolder *sourceDataHolder, QObject *parent = 0);
	virtual ~AMDSAmptekSDD123SpectralDataHolder();

	/// function to copy the value of source instance to the current instance
	virtual void cloneData(AMDSDataHolder *dataHolder);
	/// Implement the PLUS operation of AMDSLightWeightDataHolder, which will plus the value of the two instances of AMDSLightWeightDataHolder and return the new instance
	virtual AMDSDataHolder* operator +(AMDSDataHolder &dataHolder);
	/// Implement the Division operation of AMDSLightWeightDataHolder: the value of the given handler will be divided by the given divisior
	virtual AMDSDataHolder* operator /(quint32 divisor);
	/// The status data relating to the detector response
	AmptekStatusData statusData() const;

public slots:
	/// Sets the status data for the histogram
	void setStatusData(const AmptekStatusData &statusData);

protected:
	AmptekStatusData statusData_;
};

#endif // AMDSAMPTEKSDD123SPECTRALDATAHOLDER_H
