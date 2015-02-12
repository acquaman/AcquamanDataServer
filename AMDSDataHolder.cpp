#include "AMDSDataHolder.h"

AMDSDataHolder::AMDSDataHolder()
{
}

AMDSDataHolder::~AMDSDataHolder()
{
}

AMDSFullDataHolder::AMDSFullDataHolder(AMDSDataHolder::AxesStyle axesStyle, const QList<AMDSAxisInfo> &axes) :
	AMDSDataHolder()
{
	axesStyle_ = axesStyle;
	axes_ = axes;
}

AMDSFullDataHolder::~AMDSFullDataHolder()
{
}
