#ifndef AMDSFLATARRAY_H
#define AMDSFLATARRAY_H

#include <QVector>
#include <QString>

#include "DataElement/AMDSDataTypeDefinitions.h"

class AMDSFlatArray
{
public:
	AMDSFlatArray(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::InvalidType, quint32 size = 0);
	virtual ~AMDSFlatArray();

	/// define the PLUS operation of AMDSFlatArray, which will plus the value of the two instances of AMDSFlatArray and return a new instance
	virtual AMDSFlatArray operator +(AMDSFlatArray &dataFlatArray);
	/// define the Division operation of AMDSFlatArray: the value of the given handler will be divided by the given divisior
	virtual AMDSFlatArray operator /(quint32 divisor);

	/// set the value of the data at a specific index
	bool setValue(int index, qint8   value) ;
	bool setValue(int index, quint8  value) ;
	bool setValue(int index, qint16  value) ;
	bool setValue(int index, quint16 value) ;
	bool setValue(int index, qint32  value) ;
	bool setValue(int index, quint32 value) ;
	bool setValue(int index, qint64  value) ;
	bool setValue(int index, quint64 value) ;
	bool setValue(int index, float   value) ;
	bool setValue(int index, double  value) ;

	/// returns the data Array in const
	inline const QVector<qint8>& constVectorQint8() const { return vectorQint8_; }
	inline const QVector<quint8>& constVectorQuint8() const { return vectorQuint8_; }
	inline const QVector<qint16>& constVectorQint16() const { return vectorQint16_; }
	inline const QVector<quint16>& constVectorQuint16() const { return vectorQuint16_; }
	inline const QVector<qint32>& constVectorQint32() const { return vectorQint32_; }
	inline const QVector<quint32>& constVectorQuint32() const { return vectorQuint32_; }
	inline const QVector<qint64>& constVectorQint64() const { return vectorQint64_; }
	inline const QVector<quint64>& constVectorQuint64() const { return vectorQuint64_; }
	inline const QVector<float>& constVectorFloat() const { return vectorFloat_; }
	inline const QVector<double>& constVectorDouble() const { return vectorDouble_; }

	/// returns the size of the DataArray
	quint32 size() const;
	/// clear the dataArray and reset the size to 0
	void clear();
	/// clear the dataArray and reset the array to the given type/size
	void clearAndReset(AMDSDataTypeDefinitions::DataType dataType, quint32 size);

	/// read data from dataStream and write to the dataArray
	bool readFromDataStream(QDataStream *dataStream);
	/// write data of the dataArray to dataStream
	bool writeToDataStream(QDataStream *dataStream);

	/// copy the data array of the current dataset to the target array
	bool copyDataToTargetArray(AMDSFlatArray *targetArray) const;
	/// clear the target array and copy the data array of the current dataset to the target array
	bool resetTargetArrayAndReplaceData(AMDSFlatArray *targetArray) const;
	/// print all the data in the FlatArray to a QString
	QString printData() const;

	/// returns the current type of data
	inline AMDSDataTypeDefinitions::DataType dataType() const { return dataType_; }

protected:
	/// helper function to resize the dataArray with given type and size
	void resizeType(AMDSDataTypeDefinitions::DataType dataType, quint32 size);

	/// returns the data Array
	inline QVector<qint8>& vectorQint8() { return vectorQint8_; }
	inline QVector<quint8>& vectorQuint8() { return vectorQuint8_; }
	inline QVector<qint16>& vectorQint16() { return vectorQint16_; }
	inline QVector<quint16>& vectorQuint16() { return vectorQuint16_; }
	inline QVector<qint32>& vectorQint32() { return vectorQint32_; }
	inline QVector<quint32>& vectorQuint32() { return vectorQuint32_; }
	inline QVector<qint64>& vectorQint64() { return vectorQint64_; }
	inline QVector<quint64>& vectorQuint64() { return vectorQuint64_; }
	inline QVector<float>& vectorFloat() { return vectorFloat_; }
	inline QVector<double>& vectorDouble() { return vectorDouble_; }

protected:
	/// the dataType of the data Array
	AMDSDataTypeDefinitions::DataType dataType_;

	/// the vectors which holds the data
	QVector<qint8> vectorQint8_;
	QVector<quint8> vectorQuint8_;
	QVector<qint16> vectorQint16_;
	QVector<quint16> vectorQuint16_;
	QVector<qint32> vectorQint32_;
	QVector<quint32> vectorQuint32_;
	QVector<qint64> vectorQint64_;
	QVector<quint64> vectorQuint64_;
	QVector<float> vectorFloat_;
	QVector<double> vectorDouble_;
};


#endif // AMDSFLATARRAY_H
