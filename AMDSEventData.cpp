#include "AMDSEventData.h"

#include "AMDSDataStream.h"

AMDSEventData::AMDSEventData(QObject *parent) :
	QObject(parent)
{
}

AMDSEventData::~AMDSEventData()
{
}

AMDSLightWeightEventData::AMDSLightWeightEventData(QDateTime eventTime, QObject *parent) :
	AMDSEventData(parent)
{
	eventTime_ = eventTime;
}

AMDSLightWeightEventData::~AMDSLightWeightEventData()
{
}

bool AMDSLightWeightEventData::writeToDataStream(AMDSDataStream *dataStream) const{
	*dataStream << eventTime_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSLightWeightEventData::readFromDataStream(AMDSDataStream *dataStream){
	QDateTime readEventTime;
	*dataStream >> readEventTime;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setEventTime(readEventTime);

	return true;
}

AMDSFullEventData::AMDSFullEventData(QDateTime eventTime, AMDSEventData::EventType eventType, AMDSEventData::TimeScale timeScale, quint16 timeUncertainty, QObject *parent) :
	AMDSEventData(parent)
{
	eventType_ = eventType;
	timeScale_ = timeScale;
	timeUncertainty_ = timeUncertainty;
	lightWeightEventData_ = 0;
	Q_UNUSED(eventTime)
}

AMDSFullEventData::~AMDSFullEventData()
{
}

bool AMDSFullEventData::writeToDataStream(AMDSDataStream *dataStream) const{
	if(!lightWeightEventData_->writeToDataStream(dataStream))
		return false;

	*dataStream << (quint8)eventType_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << (quint8)timeScale_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << timeUncertainty_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSFullEventData::readFromDataStream(AMDSDataStream *dataStream){
	if(!lightWeightEventData_->readFromDataStream(dataStream))
		return false;

	quint8 readEventTypeAsInt;
	quint8 readTimeScaleAsInt;
	quint16 readTimeUncertainty;

	*dataStream >> readEventTypeAsInt;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readTimeScaleAsInt;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readTimeUncertainty;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	AMDSEventData::EventType readEventType = (AMDSEventData::EventType)readEventTypeAsInt;
	AMDSEventData::TimeScale readTimeScale = (AMDSEventData::TimeScale)readTimeScaleAsInt;

	if(!setEventType(readEventType))
		return false;
	if(!setTimeScale(readTimeScale))
		return false;
	if(!setTimeUncertainty(readTimeUncertainty))
		return false;

	return true;
}
