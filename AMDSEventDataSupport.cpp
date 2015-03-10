#include "AMDSEventDataSupport.h"

#include "AMDSEventData.h"

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
	if(inheritsEventData(eventDataMetaObject))
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

bool AMDSEventDataObjectInfo::inheritsEventData(const QMetaObject *metaObject) const
{
	const QMetaObject *superClass = metaObject;
	bool inheritsEventDataClass;
	do {
		inheritsEventDataClass = (superClass->className() == QString("AMDSEventData"));
	} while( (superClass = superClass->superClass()) && !inheritsEventDataClass);
	return inheritsEventDataClass;
}

namespace AMDSEventDataSupport{
	QHash<QString, AMDSEventDataObjectInfo> registeredClasses_;

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
}
