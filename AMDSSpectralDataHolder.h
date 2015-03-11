#ifndef AMDSSPECTRALDATAHOLDER_H
#define AMDSSPECTRALDATAHOLDER_H

#include "AMDSDataHolder.h"

//class AMDSLightWeightSpectralDataHolder
//{
//Q_OBJECT
//public:
//	Q_INVOKABLE AMDSLightWeightSpectralDataHolder(QObject *parent = 0);
//	virtual ~AMDSLightWeightSpectralDataHolder();

//	virtual inline quint8 rank() const { return 1; }
//	virtual inline quint32 size(int axisId) const;
//	virtual inline quint64 spanSize() const { return 1; }

//	virtual inline bool data(AMDSFlatArray *outputValues) const;
//	virtual inline bool setAxes(const QList<AMDSAxisInfo> &axes);

//	inline void setSingleValue(double singleValue) { singleValue_ = singleValue; }
//	inline void setData(AMDSFlatArray *inputValues);

//	/// Writes this AMDSDataHolder to an AMDSDataStream, returns true if no errors are encountered
//	virtual bool writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const;
//	/// Reads this AMDSDataHolder from the AMDSDataStream, returns true if no errors are encountered
//	virtual bool readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType);

//protected:
//	double singleValue_;
//};

#endif // AMDSSPECTRALDATAHOLDER_H
