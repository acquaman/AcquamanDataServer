#include "AMDSScalerConfigurationMap.h"

AMDSScalerConfigurationMap::AMDSScalerConfigurationMap(const QString &scalerName, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, const QList<quint8> &configuredChannelIds, QObject *parent )
	:QObject(parent)
{
	scalerName_ = scalerName;
	scalerBasePVName_ = basePVName;
	dataType_ = dataType;
	configuredChannels_ = configuredChannelIds;

	enabledChannels_.clear();
	disabledChannels_.clear();

	enabledChannels_.append(configuredChannels_);
	// all the other channels are disabled
	for (int channelId=0, size=32; channelId < size; channelId++) {
		if (!configuredChannels_.contains(channelId)) {
			disabledChannels_.append(channelId);
		}
	}
}

AMDSScalerConfigurationMap::~AMDSScalerConfigurationMap()
{
}

bool AMDSScalerConfigurationMap::enableChannel(int channelId)
{
	if (disabledChannels_.contains(channelId)) {
		enabledChannels_.append(channelId);
		return true;
	}

	return false;
}

bool AMDSScalerConfigurationMap::disableChannel(int channelId)
{
	if (enabledChannels_.contains(channelId)) {
		disabledChannels_.append(channelId);
		return true;
	}

	return false;
}
