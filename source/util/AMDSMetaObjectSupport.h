#ifndef AMDSMETAOBJECTSUPPORT_H
#define AMDSMETAOBJECTSUPPORT_H

#include <QMetaObject>

namespace AMDSMetaObjectSupport {
	bool inheritsClass(const QMetaObject *queryMetaObject, const QMetaObject *specificTypeMetaObject, const QMetaObject *generalTypeMetaObject = 0);
}

#endif // AMDSMETAOBJECTSUPPORT_H
