#include "AMDSScalarDataHolder.h"

AMDSScalarDataHolder::AMDSScalarDataHolder() :
	AMDSDataHolder()
{
	eventData_ = new AMDSEventData();
}

AMDSScalarDataHolder::~AMDSScalarDataHolder()
{
}

AMDSFullScalarDataHolder::AMDSFullScalarDataHolder(AxesStyle axesStyle, const QList<AMDSAxisInfo> &axes) :
	AMDSFullDataHolder(axesStyle, axes), AMDSScalarDataHolder()
{
}

AMDSFullScalarDataHolder::~AMDSFullScalarDataHolder()
{
}
