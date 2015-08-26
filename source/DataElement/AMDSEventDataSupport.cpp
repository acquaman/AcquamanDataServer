#include "source/DataElement/AMDSEventDataSupport.h"

#include "source/DataElement/AMDSEventData.h"
#include "source/AMDSMetaObjectSupport.h"

AMDSEventDataObjectInfo::AMDSEventDataObjectInfo()
{
	eventDataMetaObject_ = 0;
	eventDataClassName_ = QString();
}

AMDSEventDataObjectInfo::AMDSEventDataObjectInfo(AMDSEventData *prototypeEventData)
{
	initWithMetaObject(prototypeEventData->metaObject());
}

AMDSEventDataObjectInfo::AMDSEventDataObjectInfo(const QMetaObject *eventDataMetaObject)
{
	if(AMDSEventDataSupport::inheritsEventData(eventDataMetaObject))
		initWithMetaObject(eventDataMetaObject);
	else{
		eventDataMetaObject_ = 0;
		eventDataClassName_ = QString();
	}
}

AMDSEventDataObjectInfo::~AMDSEventDataObjectInfo()
{
}

void AMDSEventDataObjectInfo::initWithMetaObject(const QMetaObject *eventDataMetaObject)
{
	eventDataMetaObject_ = eventDataMetaObject;
	eventDataClassName_ = eventDataMetaObject->className();
}

namespace AMDSEventDataSupport{
	QHash<QString, AMDSEventDataObjectInfo> registeredClasses_;

	void registerEventDataObjectClass()
	{
		if(!AMDSEventDataSupport::registeredClasses()->contains(AMDSLightWeightEventData::staticMetaObject.className()))
			AMDSEventDataSupport::registerClass<AMDSLightWeightEventData>();
	}

	const QHash<QString, AMDSEventDataObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	AMDSEventData* instantiateEventDataFromClassName(const QString &className){
		if(registeredClasses_.contains(className)){
			AMDSEventData *eventData = qobject_cast<AMDSEventData*>(registeredClasses_.value(className).eventDataMetaObject()->newInstance());
			if(eventData)
				return eventData;
		}
		return 0;
	}

	bool inheritsEventData(const QMetaObject *queryMetaObject){
		const QMetaObject *eventDataMetaObject = &(AMDSEventData::staticMetaObject);
		return AMDSMetaObjectSupport::inheritsClass(queryMetaObject, eventDataMetaObject);
	}
}
