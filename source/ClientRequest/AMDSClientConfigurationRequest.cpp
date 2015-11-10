#include "AMDSClientConfigurationRequest.h"

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(QObject *parent)
	: AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Introspection);

//	setBufferName("Invalid");
}

AMDSClientConfigurationRequest::AMDSClientConfigurationRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::ResponseType responseType, QObject *parent)
	: AMDSClientRequest(socketKey, QString(), AMDSClientRequestDefinitions::Configuration, responseType, parent)
{

}

AMDSClientConfigurationRequest::~AMDSClientConfigurationRequest()
{

}
