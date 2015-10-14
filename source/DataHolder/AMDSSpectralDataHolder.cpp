#include "AMDSSpectralDataHolder.h"

#include "source/Connection/AMDSDataStream.h"

/// ===================== Implementation of AMDSLightWeightSpectralDataHolder ==============================
AMDSLightWeightSpectralDataHolder::AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent) :
	AMDSLightWeightGenericFlatArrayDataHolder(dataType, size, parent)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightSpectralDataHolder::AMDSLightWeightSpectralDataHolder(AMDSLightWeightSpectralDataHolder *sourceDataHolder, QObject *parent)
	:AMDSLightWeightGenericFlatArrayDataHolder(sourceDataHolder, parent)
{

}

AMDSLightWeightSpectralDataHolder::~AMDSLightWeightSpectralDataHolder()
{
}

bool AMDSLightWeightSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	return AMDSLightWeightGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType);
}

bool AMDSLightWeightSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	return AMDSLightWeightGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType);
}

/// ===================== Implementation of AMDSFullSpectralDataHolder ==============================
AMDSFullSpectralDataHolder::AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, size, axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightSpectralDataHolder(dataType, size, this);
}

AMDSFullSpectralDataHolder::~AMDSFullSpectralDataHolder()
{
}

bool AMDSFullSpectralDataHolder::writeToDataStream(AMDSDataStream *dataStream, bool encodeDataType) const{
	// This will take care of encoding and writing our lightWeightDataHolder to the stream
	if(!AMDSFullGenericFlatArrayDataHolder::writeToDataStream(dataStream, encodeDataType))
		return false;

	return true;
}

bool AMDSFullSpectralDataHolder::readFromDataStream(AMDSDataStream *dataStream, AMDSDataTypeDefinitions::DataType decodeAsDataType){
	// This will take care of decoding and instantiating our lightWeightDataHolder from the stream (and deleting any old memory we allocated for one before)
	if(!AMDSFullGenericFlatArrayDataHolder::readFromDataStream(dataStream, decodeAsDataType))
		return false;

	return true;
}

/// ===================== Implementation of AMDSDwellSpectralDataHolder ==============================
AMDSDwellSpectralDataHolder::AMDSDwellSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent)
	:AMDSLightWeightSpectralDataHolder(dataType, size, parent)
{

}

AMDSDwellSpectralDataHolder::AMDSDwellSpectralDataHolder(AMDSDwellSpectralDataHolder *sourceDataHolder, QObject *parent)
	:AMDSLightWeightSpectralDataHolder(sourceDataHolder, parent)
{

}

AMDSDwellSpectralDataHolder::~AMDSDwellSpectralDataHolder()
{

}

void AMDSDwellSpectralDataHolder::clear()
{
	AMDSLightWeightSpectralDataHolder::clear();

	AMDSDwellStatusData newStatusData;
	setDwellStatusData(newStatusData);
}

void AMDSDwellSpectralDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSDwellSpectralDataHolder *sourceSpectrualDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(sourceDataHolder);
	if (sourceSpectrualDataHolder) {
		AMDSLightWeightSpectralDataHolder::cloneData(sourceSpectrualDataHolder);
		setDwellStatusData(sourceSpectrualDataHolder->dwellStatusData());
	} else {
		AMErrorMon::alert(this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of source dataHolder is NOT a DwellSpectralDataHolder.").arg(sourceDataHolder->metaObject()->className()));
	}
}

AMDSDataHolder* AMDSDwellSpectralDataHolder::operator +(AMDSDataHolder &dataHolder)
{
	AMDSDwellSpectralDataHolder *addTargetDataHolder = 0;

	AMDSDwellSpectralDataHolder *addInputDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(&dataHolder);
	if (addInputDataHolder) {
		AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator +(dataHolder);

		addTargetDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(targetDataHolder);
		if (addTargetDataHolder) {
			AMDSDwellStatusData addInputStatusData = addInputDataHolder->dwellStatusData();
			AMDSDwellStatusData newStatusData;

			newStatusData.fastCounts_ = addInputStatusData.fastCounts_ + dwellStatusData().fastCounts_;
			newStatusData.slowCounts_ = addInputStatusData.slowCounts_ + dwellStatusData().slowCounts_;
			newStatusData.detectorTemperature_ = addInputStatusData.detectorTemperature_ + dwellStatusData().detectorTemperature_;
			newStatusData.accumulationTime_ = addInputStatusData.accumulationTime_ + dwellStatusData().accumulationTime_;
			newStatusData.liveTime_ = addInputStatusData.liveTime_ + dwellStatusData().liveTime_;
			newStatusData.realTime_ = addInputStatusData.realTime_ + dwellStatusData().realTime_;
			newStatusData.generalPurposeCounter_ = addInputStatusData.generalPurposeCounter_ + dwellStatusData().generalPurposeCounter_;
			if(addInputStatusData.dwellEndTime_ < dwellStatusData().dwellEndTime_)
				newStatusData.dwellEndTime_.setHMS(dwellStatusData().dwellEndTime_.hour(), dwellStatusData().dwellEndTime_.minute(), dwellStatusData().dwellEndTime_.second(), dwellStatusData().dwellEndTime_.msec());
			else if(dwellStatusData().dwellStartTime_ < addInputStatusData.dwellStartTime_)
				newStatusData.dwellStartTime_.setHMS(dwellStatusData().dwellStartTime_.hour(), dwellStatusData().dwellStartTime_.minute(), dwellStatusData().dwellStartTime_.second(), dwellStatusData().dwellStartTime_.msec());

			newStatusData.dwellReplyTime_ = newStatusData.dwellReplyTime_.addMSecs(dwellStatusData().dwellEndTime_.msecsTo(dwellStatusData().dwellReplyTime_));

			addTargetDataHolder->setDwellStatusData(newStatusData);
		} else {
			AMErrorMon::alert(this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of addition dataHolder is NOT a DwellSpectralDataHolder.").arg(dataHolder.metaObject()->className()));
		}
	} else {
		AMErrorMon::alert(this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of addition dataHolder is NOT a DwellSpectralDataHolder.").arg(dataHolder.metaObject()->className()));
	}

	return addTargetDataHolder;
}

AMDSDataHolder* AMDSDwellSpectralDataHolder::operator /(quint32 divisor)
{
	AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator /(divisor);

	AMDSDwellSpectralDataHolder *divisionTargetDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(targetDataHolder);
	if (divisionTargetDataHolder) {
		AMDSDwellStatusData newStatusData(dwellStatusData());

		newStatusData.fastCounts_ = dwellStatusData().fastCounts_ / divisor;
		newStatusData.slowCounts_ = dwellStatusData().slowCounts_ / divisor;
		newStatusData.detectorTemperature_ = dwellStatusData().detectorTemperature_ / divisor;
		newStatusData.accumulationTime_ = dwellStatusData().accumulationTime_ / divisor;
		newStatusData.liveTime_ = dwellStatusData().liveTime_ / divisor;
		newStatusData.realTime_ = dwellStatusData().realTime_ / divisor;
		newStatusData.generalPurposeCounter_ = dwellStatusData().generalPurposeCounter_ / divisor;

		divisionTargetDataHolder->setDwellStatusData(newStatusData);
	} else {
		AMErrorMon::alert(this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of division dataHolder is NOT a DwellSpectralDataHolder.").arg(divisionTargetDataHolder->metaObject()->className()));
	}

	return divisionTargetDataHolder;
}

AMDSDwellStatusData AMDSDwellSpectralDataHolder::dwellStatusData() const{
	return dwellStatusData_;
}

void AMDSDwellSpectralDataHolder::setDwellStatusData(const AMDSDwellStatusData &statusData){
	dwellStatusData_ = statusData;
}
