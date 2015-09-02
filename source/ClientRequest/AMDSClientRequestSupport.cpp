#include "AMDSClientRequestSupport.h"

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientStatisticsRequest.h"
#include "source/ClientRequest/AMDSClientStartTimePlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientRelativeCountPlusCountDataRequest.h"
#include "source/ClientRequest/AMDSClientStartTimeToEndTimeDataRequest.h"
#include "source/ClientRequest/AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"

#include "source/util/AMDSMetaObjectSupport.h"

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
	if(AMDSClientRequestSupport::inheritsClientRequest(clientRequestMetaObject))
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

namespace AMDSClientRequestSupport{
	QHash<AMDSClientRequestDefinitions::RequestType, AMDSClientRequestObjectInfo> registeredClasses_;

	void registerClientRequestClass() {
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Introspection))
			AMDSClientRequestSupport::registerClass<AMDSClientIntrospectionRequest>(AMDSClientRequestDefinitions::Introspection);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Statistics))
			AMDSClientRequestSupport::registerClass<AMDSClientStatisticsRequest>(AMDSClientRequestDefinitions::Statistics);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::StartTimePlusCount))
			AMDSClientRequestSupport::registerClass<AMDSClientStartTimePlusCountDataRequest>(AMDSClientRequestDefinitions::StartTimePlusCount);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::RelativeCountPlusCount))
			AMDSClientRequestSupport::registerClass<AMDSClientRelativeCountPlusCountDataRequest>(AMDSClientRequestDefinitions::RelativeCountPlusCount);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::StartTimeToEndTime))
			AMDSClientRequestSupport::registerClass<AMDSClientStartTimeToEndTimeDataRequest>(AMDSClientRequestDefinitions::StartTimeToEndTime);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter))
			AMDSClientRequestSupport::registerClass<AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest>(AMDSClientRequestDefinitions::MiddleTimePlusCountBeforeAndAfter);
		if(!AMDSClientRequestSupport::registeredClasses()->contains(AMDSClientRequestDefinitions::Continuous))
			AMDSClientRequestSupport::registerClass<AMDSClientContinuousDataRequest>(AMDSClientRequestDefinitions::Continuous);
	}

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

	bool inheritsClientRequest(const QMetaObject *queryMetaObject){
		const QMetaObject *clientRequestMetaObject = &(AMDSClientRequest::staticMetaObject);
		return AMDSMetaObjectSupport::inheritsClass(queryMetaObject, clientRequestMetaObject);
	}
}
