#include "AMDSPVControllerServer.h"

#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "Detector/PVController/AMDSPVCommandManager.h"

AMDSPVControllerServer::AMDSPVControllerServer(const QString &pvName, QObject *parent)
	: AMDSDetectorServer(pvName, parent)
{
}

AMDSPVControllerServer::~AMDSPVControllerServer()
{

}

void AMDSPVControllerServer::onConfigurationRequestReceived(AMDSClientRequest *clientRequest)
{
	AMDSDetectorServer::onConfigurationRequestReceived(clientRequest);

	AMDSClientConfigurationRequest * configureRequest = qobject_cast<AMDSClientConfigurationRequest*>(clientRequest);
	if (configureRequest) {
		foreach (int commandId, configureRequest->configurationCommands().keys()) {
			foreach(QString commandValue, configureRequest->configurationCommands().values(commandId)) {
				// perform the configuration
				performConfiguration(commandId, configureRequest->bufferName(), commandValue);
			}
		}
	}

	emit clientRequestProcessed(clientRequest);
}

void AMDSPVControllerServer::performConfiguration(int commandId, const QString &bufferName, const QString &value)
{
	Q_UNUSED(value)

	switch (commandId) {
	case AMDSPVCommandManager::RequestEnablePV:
		emit enablePVController(bufferName);
		break;
	case AMDSPVCommandManager::RequestDisablePV:
		emit disablePVController(bufferName);
		break;
	}
}
