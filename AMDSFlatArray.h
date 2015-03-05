#ifndef AMDSFLATARRAY_H
#define AMDSFLATARRAY_H

#include <QVector>

#include <QDebug>

#include "AMDSDataTypeDefinitions.h"

class AMDSFlatArray
{
public:
	AMDSFlatArray(AMDSDataTypeDefinitions::DataType dataType = AMDSDataTypeDefinitions::InvalidType, quint32 size = 0);
	virtual ~AMDSFlatArray();

	inline AMDSDataTypeDefinitions::DataType dataType() const { return dataType_; }
	inline quint32 size() const;

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

	inline void clear();
	inline void clearAndReset(AMDSDataTypeDefinitions::DataType dataType, quint32 size);

	inline bool copyData(AMDSFlatArray *other) const;
	inline bool replaceData(AMDSFlatArray *other) const;

protected:
	inline void resizeType(AMDSDataTypeDefinitions::DataType dataType, quint32 size);

protected:
	AMDSDataTypeDefinitions::DataType dataType_;

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

quint32 AMDSFlatArray::size() const{
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
		vectorQint8_.clear();
		vectorQint8_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Unsigned8:
		vectorQuint8_.clear();
		vectorQuint8_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Signed16:
		vectorQint16_.clear();
		vectorQint16_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Unsigned16:
		vectorQuint16_.clear();
		vectorQuint16_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Signed32:
		vectorQint32_.clear();
		vectorQint32_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Unsigned32:
		vectorQuint32_.clear();
		vectorQuint32_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Signed64:
		vectorQint64_.clear();
		vectorQint64_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Unsigned64:
		vectorQuint64_.clear();
		vectorQuint64_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Float:
		vectorFloat_.clear();
		vectorFloat_.resize(0);
		break;
	case AMDSDataTypeDefinitions::Double:
		vectorDouble_.clear();
		vectorDouble_.resize(0);
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
	dataType_ = dataType;
	resizeType(dataType_, size);
}

bool AMDSFlatArray::copyData(AMDSFlatArray *other) const{
	if(other == this)
		return false;
	switch(dataType_){
	case AMDSDataTypeDefinitions::Signed8:
		if(vectorQint8_.size() != other->vectorQint8().size())
			return false;
		memcpy(other->vectorQint8().data(), vectorQint8_.constData(), vectorQint8_.size()*sizeof(qint8));
		return true;
	case AMDSDataTypeDefinitions::Unsigned8:
		if(vectorQuint8_.size() != other->vectorQuint8().size())
			return false;
		memcpy(other->vectorQuint8().data(), vectorQuint8_.constData(), vectorQuint8_.size()*sizeof(quint8));
		return true;
	case AMDSDataTypeDefinitions::Signed16:
		if(vectorQint16_.size() != other->vectorQint16().size())
			return false;
		memcpy(other->vectorQint16().data(), vectorQint16_.constData(), vectorQint16_.size()*sizeof(qint16));
		return true;
	case AMDSDataTypeDefinitions::Unsigned16:
		if(vectorQuint16_.size() != other->vectorQuint16().size())
			return false;
		memcpy(other->vectorQuint16().data(), vectorQuint16_.constData(), vectorQuint16_.size()*sizeof(quint16));
		return true;
	case AMDSDataTypeDefinitions::Signed32:
		if(vectorQint32_.size() != other->vectorQint32().size())
			return false;
		memcpy(other->vectorQint32().data(), vectorQint32_.constData(), vectorQint32_.size()*sizeof(qint32));
		return true;
	case AMDSDataTypeDefinitions::Unsigned32:
		if(vectorQuint32_.size() != other->vectorQuint32().size())
			return false;
		memcpy(other->vectorQuint32().data(), vectorQuint32_.constData(), vectorQuint32_.size()*sizeof(quint32));
		return true;
	case AMDSDataTypeDefinitions::Signed64:
		if(vectorQint64_.size() != other->vectorQint64().size())
			return false;
		memcpy(other->vectorQint64().data(), vectorQint64_.constData(), vectorQint64_.size()*sizeof(qint64));
		return true;
	case AMDSDataTypeDefinitions::Unsigned64:
		if(vectorQuint64_.size() != other->vectorQuint64().size())
			return false;
		memcpy(other->vectorQuint64().data(), vectorQuint64_.constData(), vectorQuint64_.size()*sizeof(quint64));
		return true;
	case AMDSDataTypeDefinitions::Float:
		if(vectorFloat_.size() != other->vectorFloat().size())
			return false;
		memcpy(other->vectorFloat().data(), vectorFloat_.constData(), vectorFloat_.size()*sizeof(float));
		return true;
	case AMDSDataTypeDefinitions::Double:
		if(vectorDouble_.size() != other->vectorDouble().size())
			return false;
		memcpy(other->vectorDouble().data(), vectorDouble_.constData(), vectorDouble_.size()*sizeof(double));
		return true;
	case AMDSDataTypeDefinitions::InvalidType:
		return -1;
	default:
		return -1;
	}
}

bool AMDSFlatArray::replaceData(AMDSFlatArray *other) const{
	other->clearAndReset(dataType_, size());
	return copyData(other);
}

void AMDSFlatArray::resizeType(AMDSDataTypeDefinitions::DataType dataType, quint32 size){
	switch(dataType){
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

#endif // AMDSFLATARRAY_H
