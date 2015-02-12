#ifndef AMDSEVENTDATA_H
#define AMDSEVENTDATA_H

#include <QDateTime>
class AMDSEventData
{
public:
	enum EventType{
		UniformEvent = 0,
		SingleEvent = 1,
		UpdateEvent = 2,
		InvalidEvent = 3
	};

	enum TimeScale{
		UniformScale = 0,
		FemtosecondsScale = 1,
		NanosecondsScale = 2,
		PicosecondsScale = 3,
		MicrosecondsScale = 4,
		MillisecondsScale = 5,
		SecondsScale = 6,
		InvalidScale = 7
	};

	AMDSEventData(QDateTime eventTime = QDateTime::currentDateTime());
	virtual ~AMDSEventData();

	inline QDateTime eventTime() const;
	virtual inline AMDSEventData::EventType eventType() const;
	virtual inline AMDSEventData::TimeScale timeScale() const;
	virtual inline quint16 timeUncertainty() const;

	inline bool setEventTime(QDateTime eventTime);
	virtual inline bool setEventType(AMDSEventData::EventType eventType);
	virtual inline bool setTimeScale(AMDSEventData::TimeScale timeScale);
	virtual inline bool setTimeUncertainty(quint16 timeUncertainty);

protected:
	QDateTime eventTime_;
};

class AMDSFullEventData : public AMDSEventData
{
public:
	AMDSFullEventData(QDateTime eventTime = QDateTime::currentDateTime(), AMDSEventData::EventType eventType = AMDSEventData::SingleEvent, AMDSEventData::TimeScale timeScale = AMDSEventData::SecondsScale, quint16 timeUncertainty = 0);
	virtual ~AMDSFullEventData();

	virtual inline AMDSEventData::EventType eventType() const;
	virtual inline AMDSEventData::TimeScale timeScale() const;
	virtual inline quint16 timeUncertainty() const;

	virtual inline bool setEventType(AMDSEventData::EventType eventType);
	virtual inline bool setTimeScale(AMDSEventData::TimeScale timeScale);
	virtual inline bool setTimeUncertainty(quint16 timeUncertainty);

protected:
	AMDSEventData::EventType eventType_;
	AMDSEventData::TimeScale timeScale_;
	quint16 timeUncertainty_;
};

////////////////////////////////////////
// AMDSEventData inline implementations
////////////////////////////////////////

QDateTime AMDSEventData::eventTime() const{
	return eventTime_;
}

AMDSEventData::EventType AMDSEventData::eventType() const{
	return AMDSEventData::UniformEvent;
}

AMDSEventData::TimeScale AMDSEventData::timeScale() const{
	return AMDSEventData::UniformScale;
}

quint16 AMDSEventData::timeUncertainty() const{
	return -1;
}

bool AMDSEventData::setEventTime(QDateTime eventTime){
	eventTime_ = eventTime;
	return true;
}

bool AMDSEventData::setEventType(AMDSEventData::EventType eventType){
	Q_UNUSED(eventType)
	return false;
}

bool AMDSEventData::setTimeScale(AMDSEventData::TimeScale timeScale){
	Q_UNUSED(timeScale)
	return false;
}

bool AMDSEventData::setTimeUncertainty(quint16 timeUncertainty){
	Q_UNUSED(timeUncertainty)
	return false;
}

////////////////////////////////////////
// AMDSFullEventData inline implementations
////////////////////////////////////////

AMDSEventData::EventType AMDSFullEventData::eventType() const{
	return eventType_;
}

AMDSEventData::TimeScale AMDSFullEventData::timeScale() const{
	return timeScale_;
}

quint16 AMDSFullEventData::timeUncertainty() const{
	return timeUncertainty_;
}

bool AMDSFullEventData::setEventType(AMDSEventData::EventType eventType){
	eventType_ = eventType;
	return true;
}

bool AMDSFullEventData::setTimeScale(AMDSEventData::TimeScale timeScale){
	timeScale_ = timeScale;
	return true;
}

bool AMDSFullEventData::setTimeUncertainty(quint16 timeUncertainty){
	timeUncertainty_ = timeUncertainty;
	return true;
}

#endif // AMDSEVENTDATA_H
