#ifndef AMDSCLIENTREQUESTSUPPORT_H
#define AMDSCLIENTREQUESTSUPPORT_H

#include <QHash>

#include "source/ClientRequest/AMDSClientRequestDefinitions.h"

class AMDSClientRequest;

class AMDSClientRequestObjectInfo{
public:
	/// The default constructor creates an invalid object
	AMDSClientRequestObjectInfo();

	/// fill the className
	AMDSClientRequestObjectInfo(AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest *prototypeClientRequest);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDSClientRequestObjectInfo(AMDSClientRequestDefinitions::RequestType requestType, const QMetaObject *clientRequestMetaObject);

	virtual ~AMDSClientRequestObjectInfo();

	/// Indicates this AMDSClientRequestObjectInfo represents a valid object.
	inline bool isValid() const {
		return ( (requestType_ < AMDSClientRequestDefinitions::InvalidRequest) && (clientRequestMetaObject_ != 0) );
	}

	inline AMDSClientRequestDefinitions::RequestType requestType() const { return requestType_; }
	inline QString clientRequestClassName() const { return clientRequestClassName_; }
	inline const QMetaObject *clientRequestMetaObject() const { return clientRequestMetaObject_; }

protected:
	AMDSClientRequestDefinitions::RequestType requestType_; ///< the enumerated request type
	QString clientRequestClassName_; ///< the class name (C++ type name) of the client request object
	const QMetaObject *clientRequestMetaObject_; ///< QMetaObject pointer with the complete meta-object for the client request

private:
	/// used to implement both constructors
	void initWithMetaObject(AMDSClientRequestDefinitions::RequestType requestType, const QMetaObject *clientRequestMetaObject);
};

namespace AMDSClientRequestSupport {
	extern QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientRequestObjectInfo> registeredClasses_;

	/// helper function to register all the client request classes
	void registerClientRequestClass();

	/// returns a const pointer to the hash of registered AMClientRequestObjectInfo classes
	const QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientRequestObjectInfo>* registeredClasses();

	/// helper function to instantiate client request from the give type
	AMDSClientRequest* instantiateClientRequestFromType(AMDSClientRequestDefinitions::RequestType clientRequestType);

	bool inheritsClientRequest(const QMetaObject *queryMetaObject);

	template<class Ta>
	bool registerClass(AMDSClientRequestDefinitions::RequestType requestType){
		// make sure this is a valid requestType
		if( requestType >= AMDSClientRequestDefinitions::InvalidRequest)
			return false;

		// create the meta object for the client request
		const QMetaObject *clientRequestMetaObject = &(Ta::staticMetaObject);
		if(!inheritsClientRequest(clientRequestMetaObject))
			return false;

		AMDSClientRequestObjectInfo newInfo(requestType, clientRequestMetaObject);

		registeredClasses_.insert(requestType, newInfo);
		return true;
	}
}

#endif // AMDSCLIENTREQUESTSUPPORT_H
