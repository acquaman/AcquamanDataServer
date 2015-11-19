#include "AMDSDataHolderSupport.h"

#include "DataHolder/AMDSDataHolder.h"
#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataHolder/AMDSSpectralDataHolder.h"

#include "util/AMDSMetaObjectSupport.h"

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
	if(AMDSDataHolderSupport::inheritsDataHolder(dataHolderMetaObject))
		initWithMetaObject(dataHolderMetaObject);
	else{
		dataHolderMetaObject_ = 0;
		dataHolderClassName_ = QString();
	}
}

AMDSDataHolderObjectInfo::~AMDSDataHolderObjectInfo()
{
}

void AMDSDataHolderObjectInfo::initWithMetaObject(const QMetaObject *dataHolderMetaObject)
{
	dataHolderMetaObject_ = dataHolderMetaObject;
	dataHolderClassName_ = dataHolderMetaObject->className();
}

namespace AMDSDataHolderSupport{
	QHash<QString, AMDSDataHolderObjectInfo> registeredClasses_;

	void registerDataHolderClass()
	{
		if(!AMDSDataHolderSupport::registeredClasses()->contains(AMDSLightWeightScalarDataHolder::staticMetaObject.className()))
			AMDSDataHolderSupport::registerClass<AMDSLightWeightScalarDataHolder>();
		if(!AMDSDataHolderSupport::registeredClasses()->contains(AMDSLightWeightSpectralDataHolder::staticMetaObject.className()))
			AMDSDataHolderSupport::registerClass<AMDSLightWeightSpectralDataHolder>();
		if(!AMDSDataHolderSupport::registeredClasses()->contains(AMDSDwellSpectralDataHolder::staticMetaObject.className()))
			AMDSDataHolderSupport::registerClass<AMDSDwellSpectralDataHolder>();
	}

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

	AMDSDataHolder* instantiateDataHolderFromInstance(const AMDSDataHolder *dataHolder){
		if (dataHolder)
			return instantiateDataHolderFromClassName(dataHolder->metaObject()->className());
		else
			return 0;
	}

	bool inheritsDataHolder(const QMetaObject *queryMetaObject){
		const QMetaObject *dataHolderMetaObject = &(AMDSDataHolder::staticMetaObject);
		return AMDSMetaObjectSupport::inheritsClass(queryMetaObject, dataHolderMetaObject);
	}
}
