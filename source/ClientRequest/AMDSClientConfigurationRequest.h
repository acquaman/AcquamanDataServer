#ifndef AMDSCLIENTCONFIGURATIONREQUEST_H
#define AMDSCLIENTCONFIGURATIONREQUEST_H

#include "ClientRequest/AMDSClientRequest.h"

class AMDSClientConfigurationRequest : public AMDSClientRequest
{
	Q_OBJECT
public:
	explicit AMDSClientConfigurationRequest(QObject *parent = 0);
	explicit AMDSClientConfigurationRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::ResponseType responseType, QObject *parent = 0);
	virtual ~AMDSClientConfigurationRequest();


};

#endif // AMDSCLIENTCONFIGURATIONREQUEST_H
