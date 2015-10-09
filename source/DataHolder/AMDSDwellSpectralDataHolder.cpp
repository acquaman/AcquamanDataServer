#include "AMDSDwellSpectralDataHolder.h"


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

void AMDSDwellSpectralDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSLightWeightSpectralDataHolder::cloneData(sourceDataHolder);

	AMDSDwellSpectralDataHolder *sourceSpectrualDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(sourceDataHolder);
	if (sourceSpectrualDataHolder) {
		setDwellStatusData(sourceSpectrualDataHolder->dwellStatusData());
	}
}

AMDSDataHolder* AMDSDwellSpectralDataHolder::operator +(AMDSDataHolder &dataHolder)
{
	AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator +(dataHolder);

	AMDSDwellSpectralDataHolder *addInputDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(&dataHolder);
	AMDSDwellSpectralDataHolder *addTargetDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(targetDataHolder);
	if (addInputDataHolder && addTargetDataHolder) {
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
	}

	return divisionTargetDataHolder;
}

AMDSDwellStatusData AMDSDwellSpectralDataHolder::dwellStatusData() const{
	return dwellStatusData_;
}

void AMDSDwellSpectralDataHolder::setDwellStatusData(const AMDSDwellStatusData &statusData){
	dwellStatusData_ = statusData;
}

