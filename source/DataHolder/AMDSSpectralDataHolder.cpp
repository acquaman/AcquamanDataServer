#include "AMDSSpectralDataHolder.h"

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

/// ===================== Implementation of AMDSFullSpectralDataHolder ==============================
AMDSFullSpectralDataHolder::AMDSFullSpectralDataHolder(AMDSDataTypeDefinitions::DataType dataType, quint32 size, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, size, axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightSpectralDataHolder(dataType, size, this);
}

AMDSFullSpectralDataHolder::~AMDSFullSpectralDataHolder()
{
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

AMDSDataHolder* AMDSDwellSpectralDataHolder::operator +(AMDSDataHolder &dataHolder)

{
	AMDSDwellSpectralDataHolder *addTargetDataHolder = 0;

	AMDSDwellSpectralDataHolder *addInputDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(&dataHolder);
	if (addInputDataHolder) {
		AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator +(dataHolder);

		addTargetDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(targetDataHolder);
		if (addTargetDataHolder) {
			AMDSDwellStatusData newStatusData = dwellStatusData() + addInputDataHolder->dwellStatusData();
			addTargetDataHolder->setDwellStatusData(newStatusData);
		} else {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of addition dataHolder is NOT a DwellSpectralDataHolder.").arg(dataHolder.metaObject()->className()));
		}
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of addition dataHolder is NOT a DwellSpectralDataHolder.").arg(dataHolder.metaObject()->className()));
	}

	return addTargetDataHolder;
}

AMDSDataHolder* AMDSDwellSpectralDataHolder::operator /(quint32 divisor)
{
	AMDSDataHolder* targetDataHolder = AMDSLightWeightSpectralDataHolder::operator /(divisor);

	AMDSDwellSpectralDataHolder *divisionTargetDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(targetDataHolder);
	if (divisionTargetDataHolder) {
		AMDSDwellStatusData newStatusData = dwellStatusData() / divisor;
		divisionTargetDataHolder->setDwellStatusData(newStatusData);
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of division dataHolder is NOT a DwellSpectralDataHolder.").arg(divisionTargetDataHolder->metaObject()->className()));
	}

	return divisionTargetDataHolder;
}

void AMDSDwellSpectralDataHolder::clear()
{
	AMDSLightWeightSpectralDataHolder::clear();

	AMDSDwellStatusData newStatusData;
	setDwellStatusData(newStatusData);
}

QString AMDSDwellSpectralDataHolder::toString()
{
	return QString("%1 \n	%2").arg(AMDSLightWeightSpectralDataHolder::toString()).arg(dwellStatusData_.toString());
}

void AMDSDwellSpectralDataHolder::cloneData(AMDSDataHolder *sourceDataHolder)
{
	AMDSDwellSpectralDataHolder *sourceSpectrualDataHolder = qobject_cast<AMDSDwellSpectralDataHolder *>(sourceDataHolder);
	if (sourceSpectrualDataHolder) {
		AMDSLightWeightSpectralDataHolder::cloneData(sourceSpectrualDataHolder);
		setDwellStatusData(sourceSpectrualDataHolder->dwellStatusData());
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_ALERT_INVALID_DATA_TYPE, QString("The type (%1) of source dataHolder is NOT a DwellSpectralDataHolder.").arg(sourceDataHolder->metaObject()->className()));
	}
}


void AMDSDwellSpectralDataHolder::setDwellStatusData(const AMDSDwellStatusData &statusData){
	dwellStatusData_ = statusData;
/*
	QDateTime eventTime = eventData_->eventTime();
	eventTime.setTime(statusData.dwellStartTime());

	eventData_->setEventTime(eventTime)*/;
}

bool AMDSDwellSpectralDataHolder::writeToDataStream(QDataStream *dataStream)
{
	if (!AMDSLightWeightSpectralDataHolder::writeToDataStream(dataStream))
		return false;

	dwellStatusData_.writeToDataStream(dataStream);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSDwellSpectralDataHolder::readFromDataStream(QDataStream *dataStream)
{
	if (!AMDSLightWeightSpectralDataHolder::readFromDataStream(dataStream))
		return false;

	dwellStatusData_.readFromDataStream(dataStream);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}
