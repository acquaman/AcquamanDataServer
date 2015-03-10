#include "AMDSDataHolderSupport.h"

#include "AMDSDataHolder.h"

AMDSDataHolderObjectInfo::AMDSDataHolderObjectInfo()
{
	dataHolderMetaObject_ = 0;
	dataHolderClassName_ = QString();
}

AMDSDataHolderObjectInfo::AMDSDataHolderObjectInfo(AMDSDataHolder *prototypeDataHolder)
{
	initWithMetaObject(prototypeDataHolder->metaObject());
}

AMDSDataHolderObjectInfo::AMDSDataHolderObjectInfo(const QMetaObject *dataHolderMetaObject)
{
	if(inheritsDataHolder(dataHolderMetaObject))
		initWithMetaObject(dataHolderMetaObject);
	else{
		dataHolderMetaObject_ = 0;
		dataHolderClassName_ = QString();
	}
}

AMDSDataHolderObjectInfo::~AMDSDataHolderObjectInfo()
{
}

bool AMDSDataHolderObjectInfo::inheritsDataHolderClass(const QMetaObject *queryMetaObject, const QMetaObject *typeMetaObject)
{
	const QMetaObject *dataHolderSuperClass = queryMetaObject;
	const QMetaObject *generalDataHolderMetaObject = &(AMDSDataHolder::staticMetaObject);
	bool inheritsDataHolderClass = false;
	do {
		inheritsDataHolderClass = (dataHolderSuperClass->className() == generalDataHolderMetaObject->className());
	} while( (dataHolderSuperClass = dataHolderSuperClass->superClass()) && !inheritsDataHolderClass);

	if(!inheritsDataHolderClass)
		return false;

	const QMetaObject *specificSuperClass = queryMetaObject;
	bool inheritsSpecificClass = false;
	do {
		inheritsSpecificClass = (specificSuperClass->className() == typeMetaObject->className());
	} while( (specificSuperClass = specificSuperClass->superClass()) && !inheritsSpecificClass);

	return inheritsSpecificClass;
}

void AMDSDataHolderObjectInfo::initWithMetaObject(const QMetaObject *dataHolderMetaObject)
{
	dataHolderMetaObject_ = dataHolderMetaObject;
	dataHolderClassName_ = dataHolderMetaObject->className();
}

bool AMDSDataHolderObjectInfo::inheritsDataHolder(const QMetaObject *metaObject) const
{
	const QMetaObject *superClass = metaObject;
	bool inheritsDataHolderClass;
	do {
		inheritsDataHolderClass = (superClass->className() == QString("AMDSDataHolder"));
	} while( (superClass = superClass->superClass()) && !inheritsDataHolderClass);
	return inheritsDataHolderClass;
}

namespace AMDSDataHolderSupport{
	QHash<QString, AMDSDataHolderObjectInfo> registeredClasses_;

	const QHash<QString, AMDSDataHolderObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	AMDSDataHolder* instantiateDataHolderFromClassName(const QString &className){
		if(registeredClasses_.contains(className)){
			AMDSDataHolder *dataHolder = qobject_cast<AMDSDataHolder*>(registeredClasses_.value(className).dataHolderMetaObject()->newInstance());
			if(dataHolder)
				return dataHolder;
		}
		return 0;
	}
}
