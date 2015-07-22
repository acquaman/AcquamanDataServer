#ifndef AMDSEVENTDATASUPPORT_H
#define AMDSEVENTDATASUPPORT_H

#include <QHash>

class AMDSEventData;

class AMDSEventDataObjectInfo
{
public:
	/// The default constructor creates an invalid object
	AMDSEventDataObjectInfo();

	/// fill the className
	AMDSEventDataObjectInfo(AMDSEventData *prototypeEventData);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDSEventDataObjectInfo(const QMetaObject *eventDataMetaObject);

	virtual ~AMDSEventDataObjectInfo();

	/// Indicates this AMDSEventDataObjectInfo represents a valid object.
	inline bool isValid() const {
		return ( eventDataMetaObject_ != 0 );
	}

	inline QString eventDataClassName() const { return eventDataClassName_; }
	inline const QMetaObject *eventDataMetaObject() const { return eventDataMetaObject_; }

protected:
	QString eventDataClassName_; ///< the class name (C++ type name) of the event data object
	const QMetaObject *eventDataMetaObject_; ///< QMetaObject pointer with the complete meta-object for the event data

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *eventDataMetaObject);
};

namespace AMDSEventDataSupport {
	extern QHash<QString, AMDSEventDataObjectInfo> registeredClasses_;

	/// register all the event data object
	void registerEventDataObjectClass();

	/// returns a const pointer to the hash of registered AMClientRequestObjectInfo classes
	const QHash<QString, AMDSEventDataObjectInfo>* registeredClasses();

	AMDSEventData* instantiateEventDataFromClassName(const QString &className);

	bool inheritsEventData(const QMetaObject *queryMetaObject);

	template<class Ta>
	bool registerClass(){
		// create the meta object for the client request
		const QMetaObject *eventDataMetaObject = &(Ta::staticMetaObject);
		if(!inheritsEventData(eventDataMetaObject))
			return false;

		AMDSEventDataObjectInfo newInfo(eventDataMetaObject);

		registeredClasses_.insert(newInfo.eventDataClassName(), newInfo);
		return true;
	}
}

#endif // AMDSEVENTDATASUPPORT_H
