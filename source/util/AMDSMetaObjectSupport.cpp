#include "AMDSMetaObjectSupport.h"

#include <QString>

namespace AMDSMetaObjectSupport {
	bool inheritsClass(const QMetaObject *queryMetaObject, const QMetaObject *specificTypeMetaObject, const QMetaObject *generalTypeMetaObject){
		if(generalTypeMetaObject){
			const QMetaObject *generalSuperClass = queryMetaObject;
			bool inheritsGeneralClass = false;
			do {
				inheritsGeneralClass = (QString(generalSuperClass->className()) == QString(generalTypeMetaObject->className()));
			} while( (generalSuperClass = generalSuperClass->superClass()) && !inheritsGeneralClass);
			if(!inheritsGeneralClass)
				return false;
		}

		const QMetaObject *specificSuperClass = queryMetaObject;
		bool inheritsSpecificClass = false;
		do {
			inheritsSpecificClass = (QString(specificSuperClass->className()) == QString(specificTypeMetaObject->className()));
		} while( (specificSuperClass = specificSuperClass->superClass()) && !inheritsSpecificClass);

		return inheritsSpecificClass;
	}
}
