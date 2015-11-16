#include "AMDSScalerDetectorServer.h"

#include "ClientRequest/AMDSClientConfigurationRequest.h"
#include "Detector/Scaler/AMDSScalerCommandManager.h"

AMDSScalerDetectorServer::AMDSScalerDetectorServer(const QString &scalerName, QObject *parent)
	: AMDSDetectorServer(scalerName, parent)
{
}

AMDSScalerDetectorServer::~AMDSScalerDetectorServer()
{

}

void AMDSScalerDetectorServer::onConfigurationRequestReceived(AMDSClientRequest *clientRequest)
{
	AMDSDetectorServer::onConfigurationRequestReceived(clientRequest);

	AMDSClientConfigurationRequest * configureRequest = qobject_cast<AMDSClientConfigurationRequest*>(clientRequest);
	if (configureRequest) {
		bool startDwellAfterConfiguration = false;

		foreach (QString command, configureRequest->configurationCommands().keys()) {
			QString commandValue = configureRequest->configurationCommands().value(command);

			// perform the configuration
			int commandId = AMDSScalerCommandManager::scalerCommandManager()->commandId(command);
			if (commandId == AMDSCommandManager::RequestStartDwell)
				startDwellAfterConfiguration = true;
			else
				performConfiguration(commandId, commandValue);
		}

		if (startDwellAfterConfiguration) {
			emit serverGoingToStartDwelling(detectorName());
		}
	}
}

void AMDSScalerDetectorServer::performConfiguration(int commandId, QString value)
{
	switch (commandId) {
	case AMDSScalerCommandManager::RequestEnableChannel:
		emit enableScalerChannel(value.toInt());
		break;
	case AMDSScalerCommandManager::RequestDisableChannel:
		emit disableScalerChannel(value.toInt());
		break;
	}

}
