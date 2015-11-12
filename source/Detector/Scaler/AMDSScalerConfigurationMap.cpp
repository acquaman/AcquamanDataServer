#include "AMDSScalerConfigurationMap.h"

AMDSScalerConfigurationMap::AMDSScalerConfigurationMap(const QString &scalerName, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, const QList<quint8> &enabledChannelId, QObject *parent )
	:QObject(parent)
{
	scalerName_ = scalerName;
	scalerBasePVName_ = basePVName;
	dataType_ = dataType;
	enabledChannels_ = enabledChannelId;
}

AMDSScalerConfigurationMap::~AMDSScalerConfigurationMap()
{
}
