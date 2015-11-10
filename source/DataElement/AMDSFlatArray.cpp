#include "DataElement/AMDSFlatArray.h"

#include <QDataStream>

AMDSFlatArray::AMDSFlatArray(AMDSDataTypeDefinitions::DataType dataType, quint32 size)
{
	resizeType(dataType, size);
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

bool AMDSFlatArray::setValues(QVector<quint8> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned8 || size() > values.size()) {
		return false;
	}

	vectorQuint8_ = QVector<quint8>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<qint8> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed8 || size() > values.size()) {
		return false;
	}

	vectorQint8_ = QVector<qint8>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<quint16> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned16 || size() > values.size()) {
		return false;
	}

	vectorQuint16_ = QVector<quint16>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<qint16> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed16 || size() > values.size()) {
		return false;
	}

	vectorQint16_ = QVector<qint16>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<quint32> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned32 || size() > values.size()) {
		return false;
	}

	vectorQuint32_ = QVector<quint32>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<qint32> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed32 || size() > values.size()) {
		return false;
	}

	vectorQint32_ = QVector<qint32>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<quint64> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned64 || size() > values.size()) {
		return false;
	}

	vectorQuint64_ = QVector<quint64>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<qint64> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed64 || size() > values.size()) {
		return false;
	}

	vectorQint64_ = QVector<qint64>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<float> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Float || size() > values.size()) {
		return false;
	}

	vectorFloat_ = QVector<float>(values);
	return true;
}

bool AMDSFlatArray::setValues(QVector<double> values)
{
	if (dataType_ != AMDSDataTypeDefinitions::Double || size() > values.size()) {
		return false;
	}

	vectorDouble_ = QVector<double>(values);
	return true;
}

bool AMDSFlatArray::setValue(int index, qint8   value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed8 || index < 0 || index > vectorQint8_.size()) {
		return false;
	}

	vectorQint8_[index] = value;
	return true;
}
bool AMDSFlatArray::setValue(int index, quint8  value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned8 || index < 0 || index > vectorQuint8_.size()) {
		return false;
	}

	vectorQuint8_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, qint16  value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed16 || index < 0 || index > vectorQint16_.size()) {
		return false;
	}

	vectorQint16_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, quint16 value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned16 || index < 0 || index > vectorQuint16_.size()) {
		return false;
	}

	vectorQuint16_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, qint32  value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed32 || index < 0 || index > vectorQint32_.size()) {
		return false;
	}

	vectorQint32_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, quint32 value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned32 || index < 0 || index > vectorQuint32_.size()) {
		return false;
	}

	vectorQuint32_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, qint64  value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Signed64 || index < 0 || index > vectorQint64_.size()) {
		return false;
	}

	vectorQint64_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, quint64 value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Unsigned64 || index < 0 || index > vectorQuint64_.size()) {
		return false;
	}

	vectorQuint64_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, float   value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Float || index < 0 || index > vectorFloat_.size()) {
		return false;
	}

	vectorFloat_[index] = value;
	return true;
}

bool AMDSFlatArray::setValue(int index, double  value)
{
	if (dataType_ != AMDSDataTypeDefinitions::Double || index < 0 || index > vectorDouble_.size()) {
		return false;
	}

	vectorDouble_[index] = value;
	return true;
}

int AMDSFlatArray::size() const{
	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		return vectorQint8_.size();
	case AMDSDataTypeDefinitions::Unsigned8:
		return vectorQuint8_.size();
	case AMDSDataTypeDefinitions::Signed16:
		return vectorQint16_.size();
	case AMDSDataTypeDefinitions::Unsigned16:
		return vectorQuint16_.size();
	case AMDSDataTypeDefinitions::Signed32:
		return vectorQint32_.size();
	case AMDSDataTypeDefinitions::Unsigned32:
		return vectorQuint32_.size();
	case AMDSDataTypeDefinitions::Signed64:
		return vectorQint64_.size();
	case AMDSDataTypeDefinitions::Unsigned64:
		return vectorQuint64_.size();
	case AMDSDataTypeDefinitions::Float:
		return vectorFloat_.size();
	case AMDSDataTypeDefinitions::Double:
		return vectorDouble_.size();
	case AMDSDataTypeDefinitions::InvalidType:
		return 0;
	default:
		return 0;
	}
}

void AMDSFlatArray::clear(){
	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		vectorQint8_ = QVector<qint8>();
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		vectorQuint8_ = QVector<quint8>();
		break;
	case AMDSDataTypeDefinitions::Signed16:
		vectorQint16_ = QVector<qint16>();
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		vectorQuint16_ = QVector<quint16>();
		break;
	case AMDSDataTypeDefinitions::Signed32:
		vectorQint32_ = QVector<qint32>();
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		vectorQuint32_ = QVector<quint32>();
		break;
	case AMDSDataTypeDefinitions::Signed64:
		vectorQint64_ = QVector<qint64>();
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		vectorQuint64_ = QVector<quint64>();
		break;
	case AMDSDataTypeDefinitions::Float:
		vectorFloat_ = QVector<float>();
		break;
	case AMDSDataTypeDefinitions::Double:
		vectorDouble_ = QVector<double>();
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}
	dataType_ = AMDSDataTypeDefinitions::InvalidType;
}

void AMDSFlatArray::clearAndReset(AMDSDataTypeDefinitions::DataType dataType, quint32 size){
	clear();
	resizeType(dataType, size);
}

bool AMDSFlatArray::readFromDataStream(QDataStream *dataStream)
{
	// read the data type
	quint8 dataTypeAsInt;
	*dataStream >>(dataTypeAsInt);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	dataType_ = (AMDSDataTypeDefinitions::DataType)dataTypeAsInt;

	// read the data size
	quint32 size;
	*dataStream >>(size);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	clearAndReset(dataType_, size);

	// read the data
	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		*dataStream >> vectorQint8();
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		*dataStream >> vectorQuint8();
		break;
	case AMDSDataTypeDefinitions::Signed16:
		*dataStream >> vectorQint16();
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		*dataStream >> vectorQuint16();
		break;
	case AMDSDataTypeDefinitions::Signed32:
		*dataStream >> vectorQint32();
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		*dataStream >> vectorQuint32();
		break;
	case AMDSDataTypeDefinitions::Signed64:
		*dataStream >> vectorQint64();
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		*dataStream >> vectorQuint64();
		break;
	case AMDSDataTypeDefinitions::Float:
		*dataStream >> vectorFloat();
		break;
	case AMDSDataTypeDefinitions::Double:
		*dataStream >> vectorDouble();
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}

	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSFlatArray::writeToDataStream(QDataStream *dataStream)
{
	*dataStream << ((quint8)dataType_);
	*dataStream << (size());

	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		*dataStream << constVectorQint8();
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		*dataStream << constVectorQuint8();
		break;
	case AMDSDataTypeDefinitions::Signed16:
		*dataStream << constVectorQint16();
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		*dataStream << constVectorQuint16();
		break;
	case AMDSDataTypeDefinitions::Signed32:
		*dataStream << constVectorQint32();
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		*dataStream << constVectorQuint32();
		break;
	case AMDSDataTypeDefinitions::Signed64:
		*dataStream << constVectorQint64();
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		*dataStream << constVectorQuint64();
		break;
	case AMDSDataTypeDefinitions::Float:
		*dataStream << constVectorFloat();
		break;
	case AMDSDataTypeDefinitions::Double:
		*dataStream << constVectorDouble();
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}

	return true;
}

bool AMDSFlatArray::copyDataToTargetArray(AMDSFlatArray *targetArray) const{
	if(targetArray == this)
		return false;

	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		if(vectorQint8_.size() != targetArray->vectorQint8().size())
			return false;
		memcpy(targetArray->vectorQint8().data(), vectorQint8_.constData(), vectorQint8_.size()*sizeof(qint8));
		return true;
	case AMDSDataTypeDefinitions::Unsigned8:
		if(vectorQuint8_.size() != targetArray->vectorQuint8().size())
			return false;
		memcpy(targetArray->vectorQuint8().data(), vectorQuint8_.constData(), vectorQuint8_.size()*sizeof(quint8));
		return true;
	case AMDSDataTypeDefinitions::Signed16:
		if(vectorQint16_.size() != targetArray->vectorQint16().size())
			return false;
		memcpy(targetArray->vectorQint16().data(), vectorQint16_.constData(), vectorQint16_.size()*sizeof(qint16));
		return true;
	case AMDSDataTypeDefinitions::Unsigned16:
		if(vectorQuint16_.size() != targetArray->vectorQuint16().size())
			return false;
		memcpy(targetArray->vectorQuint16().data(), vectorQuint16_.constData(), vectorQuint16_.size()*sizeof(quint16));
		return true;
	case AMDSDataTypeDefinitions::Signed32:
		if(vectorQint32_.size() != targetArray->vectorQint32().size())
			return false;
		memcpy(targetArray->vectorQint32().data(), vectorQint32_.constData(), vectorQint32_.size()*sizeof(qint32));
		return true;
	case AMDSDataTypeDefinitions::Unsigned32:
		if(vectorQuint32_.size() != targetArray->vectorQuint32().size())
			return false;
		memcpy(targetArray->vectorQuint32().data(), vectorQuint32_.constData(), vectorQuint32_.size()*sizeof(quint32));
		return true;
	case AMDSDataTypeDefinitions::Signed64:
		if(vectorQint64_.size() != targetArray->vectorQint64().size())
			return false;
		memcpy(targetArray->vectorQint64().data(), vectorQint64_.constData(), vectorQint64_.size()*sizeof(qint64));
		return true;
	case AMDSDataTypeDefinitions::Unsigned64:
		if(vectorQuint64_.size() != targetArray->vectorQuint64().size())
			return false;
		memcpy(targetArray->vectorQuint64().data(), vectorQuint64_.constData(), vectorQuint64_.size()*sizeof(quint64));
		return true;
	case AMDSDataTypeDefinitions::Float:
		if(vectorFloat_.size() != targetArray->vectorFloat().size())
			return false;
		memcpy(targetArray->vectorFloat().data(), vectorFloat_.constData(), vectorFloat_.size()*sizeof(float));
		return true;
	case AMDSDataTypeDefinitions::Double:
		if(vectorDouble_.size() != targetArray->vectorDouble().size())
			return false;
		memcpy(targetArray->vectorDouble().data(), vectorDouble_.constData(), vectorDouble_.size()*sizeof(double));
		return true;
	case AMDSDataTypeDefinitions::InvalidType:
		return -1;
	default:
		return -1;
	}
}

bool AMDSFlatArray::resetTargetArrayAndReplaceData(AMDSFlatArray *targetArray) const{
	targetArray->clearAndReset(dataType_, size());
	return copyDataToTargetArray(targetArray);
}

void AMDSFlatArray::resizeType(AMDSDataTypeDefinitions::DataType dataType, quint32 size){
	dataType_ = dataType;

	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		vectorQint8_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		vectorQuint8_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Signed16:
		vectorQint16_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		vectorQuint16_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Signed32:
		vectorQint32_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		vectorQuint32_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Signed64:
		vectorQint64_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		vectorQuint64_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Float:
		vectorFloat_.resize(size);
		break;
	case AMDSDataTypeDefinitions::Double:
		vectorDouble_.resize(size);
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		break;
	default:
		break;
	}
}

QString AMDSFlatArray::toString() const{
	QString retVal;
	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		retVal.append("Signed 8: ");
		for(int x = 0, size = vectorQint8_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQint8_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		retVal.append("Unsigned 8: ");
		for(int x = 0, size = vectorQuint8_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQuint8_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Signed16:
		retVal.append("Signed 16: ");
		for(int x = 0, size = vectorQint16_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQint16_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		retVal.append("Unsigned 16: ");
		for(int x = 0, size = vectorQuint16_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQuint16_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Signed32:
		retVal.append("Signed 32: ");
		for(int x = 0, size = vectorQint32_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQint32_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		retVal.append("Unsigned 32: ");
		for(int x = 0, size = vectorQuint32_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQuint32_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Signed64:
		retVal.append("Signed 64: ");
		for(int x = 0, size = vectorQint64_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQint64_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		retVal.append("Unsigned 64: ");
		for(int x = 0, size = vectorQuint64_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorQuint64_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Float:
		retVal.append("Float: ");
		for(int x = 0, size = vectorFloat_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorFloat_.at(x)));
		break;
	case AMDSDataTypeDefinitions::Double:
		retVal.append("Double: ");
		for(int x = 0, size = vectorDouble_.size(); x < size; x++)
			retVal.append(QString("%1 ").arg(vectorDouble_.at(x)));
		break;
	case AMDSDataTypeDefinitions::InvalidType:
		retVal = "Invalid Type";
		break;
	default:
		retVal = "Default Case, past enumeration";
		break;
	}
	return retVal;
}
