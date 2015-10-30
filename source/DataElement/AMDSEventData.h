#ifndef AMDSEVENTDATA_H
#define AMDSEVENTDATA_H

#include <QObject>
#include <QDateTime>
#include <QDataStream>

class AMDSEventData : public QObject
{
Q_OBJECT
public:
	static AMDSEventData *decodeAndInstantiateEventData(QDataStream *dataStream);

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

	AMDSEventData(QObject *parent = 0);
	virtual ~AMDSEventData();

	virtual QDateTime eventTime() const = 0;
	virtual inline AMDSEventData::EventType eventType() const = 0;
	virtual inline AMDSEventData::TimeScale timeScale() const = 0;
	virtual inline quint16 timeUncertainty() const = 0;

	virtual inline bool setEventTime(QDateTime eventTime) = 0;
	virtual inline bool setEventType(AMDSEventData::EventType eventType) = 0;
	virtual inline bool setTimeScale(AMDSEventData::TimeScale timeScale) = 0;
	virtual inline bool setTimeUncertainty(quint16 timeUncertainty) = 0;

	/// virtual function to copy the data of the source eventData to the current instance
	virtual void cloneData(AMDSEventData *sourceEventData) = 0;
	/// implementationt the function to copy the data of the source eventData to the target instance
	virtual AMDSEventData& operator =(AMDSEventData &sourceEventData);

	/// Writes this AMDSEventData to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) const = 0;
	/// Reads this AMDSEventData from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream) = 0;
};

class AMDSLightWeightEventData : public AMDSEventData
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSLightWeightEventData(QDateTime eventTime = QDateTime::currentDateTime(), QObject *parent = 0);
	Q_INVOKABLE AMDSLightWeightEventData(AMDSLightWeightEventData &eventData, QObject *parent = 0);
	virtual ~AMDSLightWeightEventData();

	virtual QDateTime eventTime() const { return eventTime_; }
	virtual inline AMDSEventData::EventType eventType() const { return AMDSEventData::UniformEvent; }
	virtual inline AMDSEventData::TimeScale timeScale() const { return AMDSEventData::UniformScale; }
	virtual inline quint16 timeUncertainty() const { return 0; }

	virtual inline bool setEventTime(QDateTime eventTime);
	virtual inline bool setEventType(AMDSEventData::EventType eventType);
	virtual inline bool setTimeScale(AMDSEventData::TimeScale timeScale);
	virtual inline bool setTimeUncertainty(quint16 timeUncertainty);

	/// implement the function to copy the data of the source eventData to the current instance
	virtual void cloneData(AMDSEventData *sourceEventData);

	/// Writes this AMDSEventData to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) const;
	/// Reads this AMDSEventData from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream);

protected:
	QDateTime eventTime_;
};

class AMDSFullEventData : public AMDSEventData
{
Q_OBJECT
public:
	Q_INVOKABLE AMDSFullEventData(QDateTime eventTime = QDateTime::currentDateTime(), AMDSEventData::EventType eventType = AMDSEventData::SingleEvent, AMDSEventData::TimeScale timeScale = AMDSEventData::SecondsScale, quint16 timeUncertainty = 0, QObject *parent = 0);
	Q_INVOKABLE AMDSFullEventData(AMDSFullEventData &eventData, QObject *parent = 0);
	virtual ~AMDSFullEventData();

	virtual QDateTime eventTime() const { return lightWeightEventData_->eventTime(); }
	virtual inline AMDSEventData::EventType eventType() const;
	virtual inline AMDSEventData::TimeScale timeScale() const;
	virtual inline quint16 timeUncertainty() const;

	virtual inline bool setEventTime(QDateTime eventTime) { return lightWeightEventData_->setEventTime(eventTime); }
	virtual inline bool setEventType(AMDSEventData::EventType eventType);
	virtual inline bool setTimeScale(AMDSEventData::TimeScale timeScale);
	virtual inline bool setTimeUncertainty(quint16 timeUncertainty);

	/// implement the function to copy the data of the source eventData to the current instance
	virtual void cloneData(AMDSEventData *sourceEventData);

	/// Writes this AMDSEventData to an QDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(QDataStream *dataStream) const;
	/// Reads this AMDSEventData from the QDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(QDataStream *dataStream);

protected:
	/// getter function to get the lightWeightEventData_ of AMDSFullEventData.
	/// The reason to encapsulate this as protected is that the user of AMDSFullEventData should NOT be aware the existence of the instance of lightWeightEventData_
	inline AMDSLightWeightEventData * lightWeightEventData() { return lightWeightEventData_; }

protected:
	AMDSLightWeightEventData *lightWeightEventData_;

	AMDSEventData::EventType eventType_;
	AMDSEventData::TimeScale timeScale_;
	quint16 timeUncertainty_;
};

////////////////////////////////////////
// AMDSEventData inline implementations
////////////////////////////////////////

bool AMDSLightWeightEventData::setEventTime(QDateTime eventTime){
	eventTime_ = eventTime;
	return true;
}

bool AMDSLightWeightEventData::setEventType(AMDSEventData::EventType eventType){
	Q_UNUSED(eventType)
	return false;
}

bool AMDSLightWeightEventData::setTimeScale(AMDSEventData::TimeScale timeScale){
	Q_UNUSED(timeScale)
	return false;
}

bool AMDSLightWeightEventData::setTimeUncertainty(quint16 timeUncertainty){
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
