#include "source/DataElement/AMDSFlatArray.h"

AMDSFlatArray::AMDSFlatArray(AMDSDataTypeDefinitions::DataType dataType, quint32 size)
{
	dataType_ = dataType;
	resizeType(dataType_, size);
}

AMDSFlatArray::~AMDSFlatArray()
{
	clear();
}

AMDSFlatArray AMDSFlatArray::operator +(AMDSFlatArray &dataFlatArray)
{
	AMDSFlatArray newDataFlatArray(dataType(), size());
	copyDataToTargetArray(&newDataFlatArray);

	switch(dataType_) {
	case AMDSDataTypeDefinitions::Signed8: {
		QVector<qint8> *qint8SourceDataVector = &dataFlatArray.vectorQint8();
		QVector<qint8> *qint8TargetDataVector = &newDataFlatArray.vectorQint8();
		for (int i = 0, size = qint8TargetDataVector->size(); i < size; i++) {
			qint8TargetDataVector->replace(i, qint8TargetDataVector->value(i) + qint8SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned8: {
		QVector<quint8> *quint8SourceDataVector = &dataFlatArray.vectorQuint8();
		QVector<quint8> *quint8TargetDataVector = &newDataFlatArray.vectorQuint8();
		for (int i = 0, size = quint8TargetDataVector->size(); i < size; i++) {
			quint8TargetDataVector->replace(i, quint8TargetDataVector->value(i) + quint8SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed16: {
		QVector<qint16> *qint16SourceDataVector = &dataFlatArray.vectorQint16();
		QVector<qint16> *qint16TargetDataVector = &newDataFlatArray.vectorQint16();
		for (int i = 0, size = qint16TargetDataVector->size(); i < size; i++) {
			qint16TargetDataVector->replace(i, qint16TargetDataVector->value(i) + qint16SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned16: {
		QVector<quint16> *quint16SourceDataVector = &dataFlatArray.vectorQuint16();
		QVector<quint16> *quint16TargetDataVector = &newDataFlatArray.vectorQuint16();
		for (int i = 0, size = quint16TargetDataVector->size(); i < size; i++) {
			quint16TargetDataVector->replace(i, quint16TargetDataVector->value(i) + quint16SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed32: {
		QVector<qint32> *qint32SourceDataVector = &dataFlatArray.vectorQint32();
		QVector<qint32> *qint32TargetDataVector = &newDataFlatArray.vectorQint32();
		for (int i = 0, size = qint32TargetDataVector->size(); i < size; i++) {
			qint32TargetDataVector->replace(i, qint32TargetDataVector->value(i) + qint32SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned32: {
		QVector<quint32> *quint32SourceDataVector = &dataFlatArray.vectorQuint32();
		QVector<quint32> *quint32TargetDataVector = &newDataFlatArray.vectorQuint32();
		for (int i = 0, size = quint32TargetDataVector->size(); i < size; i++) {
			quint32TargetDataVector->replace(i, quint32TargetDataVector->value(i) + quint32SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed64: {
		QVector<qint64> *qint64SourceDataVector = &dataFlatArray.vectorQint64();
		QVector<qint64> *qint64TargetDataVector = &newDataFlatArray.vectorQint64();
		for (int i = 0, size = qint64TargetDataVector->size(); i < size; i++) {
			qint64TargetDataVector->replace(i, qint64TargetDataVector->value(i) + qint64SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned64: {
		QVector<quint64> *quint64SourceDataVector = &dataFlatArray.vectorQuint64();
		QVector<quint64> *quint64TargetDataVector = &newDataFlatArray.vectorQuint64();
		for (int i = 0, size = quint64TargetDataVector->size(); i < size; i++) {
			quint64TargetDataVector->replace(i, quint64TargetDataVector->value(i) + quint64SourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Float: {
		QVector<float> *floatSourceDataVector = &dataFlatArray.vectorFloat();
		QVector<float> *floatTargetDataVector = &newDataFlatArray.vectorFloat();
		for (int i = 0, size = floatTargetDataVector->size(); i < size; i++) {
			floatTargetDataVector->replace(i, floatTargetDataVector->value(i) + floatSourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::Double: {
		QVector<double> *doubleSourceDataVector = &dataFlatArray.vectorDouble();
		QVector<double> *doubleTargetDataVector = &newDataFlatArray.vectorDouble();
		for (int i = 0, size = doubleTargetDataVector->size(); i < size; i++) {
			doubleTargetDataVector->replace(i, doubleTargetDataVector->value(i) + doubleSourceDataVector->value(i));
		}
		break;
	}
	case AMDSDataTypeDefinitions::InvalidType:
	default:
		break;
	}

	return newDataFlatArray;
}

AMDSFlatArray AMDSFlatArray::operator /(const quint32 divisor)
{
	AMDSFlatArray newDataFlatArray(dataType(), size());
	copyDataToTargetArray(&newDataFlatArray);

	switch(dataType_) {
	case AMDSDataTypeDefinitions::Signed8: {
		QVector<qint8> *qint8TargetDataVector = &newDataFlatArray.vectorQint8();
		for (int i = 0; i < qint8TargetDataVector->size(); i++) {
			qint8 value = (qint8)(qint8TargetDataVector->value(i) / divisor);
			qint8TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned8: {
		QVector<quint8> *quint8TargetDataVector = &newDataFlatArray.vectorQuint8();
		for (int i = 0; i < quint8TargetDataVector->size(); i++) {
			quint8 value = (quint8)(quint8TargetDataVector->value(i) / divisor);
			quint8TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed16: {
		QVector<qint16> *qint16TargetDataVector = &newDataFlatArray.vectorQint16();
		for (int i = 0; i < qint16TargetDataVector->size(); i++) {
			qint16 value = (qint16)(qint16TargetDataVector->value(i) / divisor);
			qint16TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned16: {
		QVector<quint16> *quint16TargetDataVector = &newDataFlatArray.vectorQuint16();
		for (int i = 0; i < quint16TargetDataVector->size(); i++) {
			quint16 value = (quint16)(quint16TargetDataVector->value(i) / divisor);
			quint16TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed32: {
		QVector<qint32> *qint32TargetDataVector = &newDataFlatArray.vectorQint32();
		for (int i = 0; i < qint32TargetDataVector->size(); i++) {
			qint32 value = (qint32)(qint32TargetDataVector->value(i) / divisor);
			qint32TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned32: {
		QVector<quint32> *quint32TargetDataVector = &newDataFlatArray.vectorQuint32();
		for (int i = 0; i < quint32TargetDataVector->size(); i++) {
			quint32 value = (quint32)(quint32TargetDataVector->value(i) / divisor);
			quint32TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Signed64: {
		QVector<qint64> *qint64TargetDataVector = &newDataFlatArray.vectorQint64();
		for (int i = 0; i < qint64TargetDataVector->size(); i++) {
			qint64 value = (qint64)(qint64TargetDataVector->value(i) / divisor);
			qint64TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Unsigned64: {
		QVector<quint64> *quint64TargetDataVector = &newDataFlatArray.vectorQuint64();
		for (int i = 0; i < quint64TargetDataVector->size(); i++) {
			quint64 value = (quint64)(quint64TargetDataVector->value(i) / divisor);
			quint64TargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Float: {
		QVector<float> *floatTargetDataVector = &newDataFlatArray.vectorFloat();
		for (int i = 0; i < floatTargetDataVector->size(); i++) {
			float value = (float)(floatTargetDataVector->value(i) / divisor);
			floatTargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::Double: {
		QVector<double> *doubleTargetDataVector = &newDataFlatArray.vectorDouble();
		for (int i = 0; i < doubleTargetDataVector->size(); i++) {
			double value = (double)(doubleTargetDataVector->value(i) / divisor);
			doubleTargetDataVector->replace(i, value );
		}
		break;
	}
	case AMDSDataTypeDefinitions::InvalidType:
	default:
		break;
	}

	return newDataFlatArray;
}
