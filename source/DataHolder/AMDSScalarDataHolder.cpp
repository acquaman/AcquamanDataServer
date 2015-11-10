#include "AMDSScalarDataHolder.h"


#include <QDebug>
AMDSLightWeightScalarDataHolder::AMDSLightWeightScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, QObject *parent) :
	AMDSLightWeightGenericFlatArrayDataHolder(dataType, 1, parent)
{
	eventData_ = new AMDSLightWeightEventData();
}

AMDSLightWeightScalarDataHolder::AMDSLightWeightScalarDataHolder(AMDSLightWeightScalarDataHolder *sourceDataHolder, QObject *parent )
	:AMDSLightWeightGenericFlatArrayDataHolder(sourceDataHolder, parent)
{
}

AMDSLightWeightScalarDataHolder::~AMDSLightWeightScalarDataHolder()
{
	qDebug() << "==== delete AMDSLightWeightScalarDataHolder";
}

/// ======================= implementation of AMDSFullScalarDataHolder =======================
AMDSFullScalarDataHolder::AMDSFullScalarDataHolder(AMDSDataTypeDefinitions::DataType dataType, AMDSDataHolder::AxesStyle axesStyle, AMDSDataHolder::DataTypeStyle dataTypeStyle, const QList<AMDSAxisInfo> &axes, QObject *parent) :
	AMDSFullGenericFlatArrayDataHolder(dataType, 1, axesStyle, dataTypeStyle, axes, parent)
{
	lightWeightDataHolder_ = new AMDSLightWeightScalarDataHolder(dataType, this);
}

AMDSFullScalarDataHolder::~AMDSFullScalarDataHolder()
{
}
