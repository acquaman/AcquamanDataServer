#ifndef AMDSDATAHOLDERSUPPORT_H
#define AMDSDATAHOLDERSUPPORT_H

#include <QHash>

class AMDSDataHolder;

class AMDSDataHolderObjectInfo
{
public:
	/// The default constructor creates an invalid object
	AMDSDataHolderObjectInfo();

	/// fill the className
	AMDSDataHolderObjectInfo(AMDSDataHolder *prototypeDataHolder);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDSDataHolderObjectInfo(const QMetaObject *dataHolderMetaObject);

	virtual ~AMDSDataHolderObjectInfo();

	/// Indicates this AMDSDataHolderObjectInfo represents a valid object.
	inline bool isValid() const {
		return ( dataHolderMetaObject_ != 0 );
	}

	inline QString dataHolderClassName() const { return dataHolderClassName_; }
	inline const QMetaObject *dataHolderMetaObject() const { return dataHolderMetaObject_; }

	static bool inheritsDataHolderClass(const QMetaObject *queryMetaObject, const QMetaObject *typeMetaObject);

protected:
	QString dataHolderClassName_; ///< the class name (C++ type name) of the data holder object
	const QMetaObject *dataHolderMetaObject_; ///< QMetaObject pointer with the complete meta-object for the data holder

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *dataHolderMetaObject);

	/// checks to make sure a QMetaObject inherits AMDSDataHolder
	bool inheritsDataHolder(const QMetaObject *metaObject) const;
};

namespace AMDSDataHolderSupport {
	extern QHash<QString, AMDSDataHolderObjectInfo> registeredClasses_;

	/// returns a const pointer to the hash of registered AMDSDataHolderObjectInfo classes
	const QHash<QString, AMDSDataHolderObjectInfo>* registeredClasses();

	AMDSDataHolder* instantiateDataHolderFromClassName(const QString &className);

	template<class Ta>
	bool registerClass(){
		// create the meta object for the client request
		const QMetaObject *dataHolderMetaObject = &(Ta::staticMetaObject);

		// is this a subclass of AMDSDataHolder? (Or an AMDSDataHolder itself?)
		const QMetaObject *dataHolderSuperClass = dataHolderMetaObject;
		bool inheritsDataHolderClass;
		do {
			inheritsDataHolderClass = (dataHolderSuperClass->className() == QString("AMDSDataHolder"));
		} while( (dataHolderSuperClass = dataHolderSuperClass->superClass()) && !inheritsDataHolderClass);

		if(!inheritsDataHolderClass)
			return false;

		AMDSDataHolderObjectInfo newInfo(dataHolderMetaObject);

		registeredClasses_.insert(newInfo.dataHolderClassName(), newInfo);
		return true;
	}
}

#endif // AMDSDATAHOLDERSUPPORT_H
