#include "AMDSEventData.h"

AMDSEventData::AMDSEventData(QDateTime eventTime)
{
	eventTime_ = eventTime;
}

AMDSEventData::~AMDSEventData()
{
}

AMDSFullEventData::AMDSFullEventData(QDateTime eventTime, AMDSEventData::EventType eventType, AMDSEventData::TimeScale timeScale, quint16 timeUncertainty) :
	AMDSEventData(eventTime)
{
	eventType_ = eventType;
	timeScale_ = timeScale;
	timeUncertainty_ = timeUncertainty;
}

AMDSFullEventData::~AMDSFullEventData()
{
}
