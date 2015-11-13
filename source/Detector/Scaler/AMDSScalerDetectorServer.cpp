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

		QMap<int, QVariant> commands = configureRequest->configurationCommands();
		for (int index = 0; index < commands.size(); index++) {
			int commandId = commands.keys().at(index);
			QVariant commandValue = commands.values(commandId);

			// perform the configuration
			if (commandId = AMDSCommandManager::RequestStartDwell)
				startDwellAfterConfiguration = true;
			else
				performConfiguration(commandId, commandValue);
		}

		if (startDwellAfterConfiguration) {
			emit serverGoingToStartDwelling(detectorName());
		}
	}
}

void AMDSScalerDetectorServer::performConfiguration(int commandId, QVariant value)
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
