#include "AMDSClientRequestSupport.h"

#include "ClientRequest/AMDSClientRequest.h"

AMDSClientRequestObjectInfo::AMDSClientRequestObjectInfo()
{
	requestType_ = AMDSClientRequestDefinitions::InvalidRequest;
	clientRequestMetaObject_ = 0;
	clientRequestClassName_ = QString();
}

AMDSClientRequestObjectInfo::AMDSClientRequestObjectInfo(AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest *prototypeClientRequest)
{
	initWithMetaObject(requestType, prototypeClientRequest->metaObject());
}

AMDSClientRequestObjectInfo::AMDSClientRequestObjectInfo(AMDSClientRequestDefinitions::RequestType requestType, const QMetaObject *clientRequestMetaObject)
{
	if(inheritsClientRequest(clientRequestMetaObject))
		initWithMetaObject(requestType, clientRequestMetaObject);
	else{
		requestType_ = AMDSClientRequestDefinitions::InvalidRequest;
		clientRequestMetaObject_ = 0;
		clientRequestClassName_ = QString();
	}
}

AMDSClientRequestObjectInfo::~AMDSClientRequestObjectInfo()
{
}

void AMDSClientRequestObjectInfo::initWithMetaObject(AMDSClientRequestDefinitions::RequestType requestType, const QMetaObject *clientRequestMetaObject)
{
	requestType_ = requestType;
	clientRequestMetaObject_ = clientRequestMetaObject;
	clientRequestClassName_ = clientRequestMetaObject->className();
}

bool AMDSClientRequestObjectInfo::inheritsClientRequest(const QMetaObject *metaObject) const
{
	const QMetaObject *superClass = metaObject;
	bool inheritsClientRequestClass;
	do {
		inheritsClientRequestClass = (superClass->className() == QString("AMDSClientRequest"));
	} while( (superClass = superClass->superClass()) && !inheritsClientRequestClass);
	return inheritsClientRequestClass;
}

namespace AMDSClientRequestSupport{
	QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientRequestObjectInfo> registeredClasses_;

	const QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientRequestObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	AMDSClientRequest* instantiateClientRequestFromType(AMDSClientRequestDefinitions::RequestType clientRequestType){
		if(clientRequestType >= AMDSClientRequestDefinitions::InvalidRequest)
			return 0;

		if(registeredClasses_.contains(clientRequestType)){
			AMDSClientRequest *clientRequest = qobject_cast<AMDSClientRequest*>(registeredClasses_.value(clientRequestType).clientRequestMetaObject()->newInstance());
			if(clientRequest)
				return clientRequest;
		}
		return 0;
	}
}
