#include "AMDSAmptekSDD123SpectralDataHolder.h"


AMDSAmptekSDD123SpectralDataHolder::AMDSAmptekSDD123SpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, QObject *parent)
	:AMDSLightWeightSpectralDataHolder(dataType, size, parent)
{

}

AMDSAmptekSDD123SpectralDataHolder::AMDSAmptekSDD123SpectralDataHolder(AMDSAmptekSDD123SpectralDataHolder *sourceDataHolder, QObject *parent)
	:AMDSLightWeightSpectralDataHolder(sourceDataHolder, parent)
{

}

AMDSAmptekSDD123SpectralDataHolder::~AMDSAmptekSDD123SpectralDataHolder()
{

}

void AMDSAmptekSDD123SpectralDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSAmptekSDD123SpectralDataHolder *sourceSpectrualDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(sourceDataHolder);
	if (sourceSpectrualDataHolder) {
		setStatusData(sourceSpectrualDataHolder->statusData());
	}
}

AMDSDataHolder* AMDSAmptekSDD123SpectralDataHolder::operator +(AMDSDataHolder &dataHolder)
{
//	AMDSAmptekSDD123SpectralDataHolder *addInputDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(&dataHolder);
//	if (!addInputDataHolder) {
//		AMErrorMon::alert(this, AMDS_SERVER_ERR_UNMATCH_DATA_HOLDER_FOR_PLUS, QString("Invalid class type (%1) to do the PLUS operation").arg(dataHolder.metaObject()->className()));
//		return 0;
//	}

//	AMDSFlatArray data = dataArray() + addInputDataHolder->dataArray();

//	AMDSDataHolder* targetDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromClassName(metaObject()->className());
//	targetDataHolder->setData(&data);
//	return targetDataHolder;

	AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator +(dataHolder);

	AMDSAmptekSDD123SpectralDataHolder *addInputDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(&dataHolder);
	AMDSAmptekSDD123SpectralDataHolder *addTargetDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(targetDataHolder);
	if (addInputDataHolder && addTargetDataHolder) {
		AmptekStatusData addInputStatusData = addInputDataHolder->statusData();
		AmptekStatusData newStatusData;

		newStatusData.fastCounts_ = addInputStatusData.fastCounts_ + statusData().fastCounts_;
		newStatusData.slowCounts_ = addInputStatusData.slowCounts_ + statusData().slowCounts_;
		newStatusData.detectorTemperature_ = addInputStatusData.detectorTemperature_ + statusData().detectorTemperature_;
		newStatusData.accumulationTime_ = addInputStatusData.accumulationTime_ + statusData().accumulationTime_;
		newStatusData.liveTime_ = addInputStatusData.liveTime_ + statusData().liveTime_;
		newStatusData.realTime_ = addInputStatusData.realTime_ + statusData().realTime_;
		newStatusData.generalPurposeCounter_ = addInputStatusData.generalPurposeCounter_ + statusData().generalPurposeCounter_;
		if(addInputStatusData.dwellEndTime_ < statusData().dwellEndTime_)
			newStatusData.dwellEndTime_.setHMS(statusData().dwellEndTime_.hour(), statusData().dwellEndTime_.minute(), statusData().dwellEndTime_.second(), statusData().dwellEndTime_.msec());
		else if(statusData().dwellStartTime_ < addInputStatusData.dwellStartTime_)
			newStatusData.dwellStartTime_.setHMS(statusData().dwellStartTime_.hour(), statusData().dwellStartTime_.minute(), statusData().dwellStartTime_.second(), statusData().dwellStartTime_.msec());

		newStatusData.dwellReplyTime_ = newStatusData.dwellReplyTime_.addMSecs(statusData().dwellEndTime_.msecsTo(statusData().dwellReplyTime_));

		addTargetDataHolder->setStatusData(newStatusData);
	}

	return addTargetDataHolder;
}

AMDSDataHolder* AMDSAmptekSDD123SpectralDataHolder::operator /(quint32 divisor)
{
	AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator /(divisor);

	AMDSAmptekSDD123SpectralDataHolder *divisionTargetDataHolder = qobject_cast<AMDSAmptekSDD123SpectralDataHolder *>(targetDataHolder);

	if (divisionTargetDataHolder) {
		AmptekStatusData newStatusData(statusData());

		newStatusData.fastCounts_ = statusData().fastCounts_ / divisor;
		newStatusData.slowCounts_ = statusData().slowCounts_ / divisor;
		newStatusData.detectorTemperature_ = statusData().detectorTemperature_ / divisor;
		newStatusData.accumulationTime_ = statusData().accumulationTime_ / divisor;
		newStatusData.liveTime_ = statusData().liveTime_ / divisor;
		newStatusData.realTime_ = statusData().realTime_ / divisor;
		newStatusData.generalPurposeCounter_ = statusData().generalPurposeCounter_ / divisor;

		divisionTargetDataHolder->setStatusData(newStatusData);
	}

	return divisionTargetDataHolder;
}

AmptekStatusData AMDSAmptekSDD123SpectralDataHolder::statusData() const{
	return statusData_;
}

void AMDSAmptekSDD123SpectralDataHolder::setStatusData(const AmptekStatusData &statusData){
	statusData_ = statusData;
}

