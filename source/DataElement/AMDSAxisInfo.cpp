#include "source/DataElement/AMDSAxisInfo.h"

AMDSAxisInfo::AMDSAxisInfo(const QString &name, quint32 size, const QString &description, const QString &units, bool isUniform)
{
	setName(name);
	setSize(size);
	setDescription(description);
	setUnits(units);
	setIsUniform(isUniform);
	setStart(0);
	setIncrement(1);
}

AMDSAxisInfo::AMDSAxisInfo(const AMDSAxisInfo &axisInfo )
{
	setName(axisInfo.name());
	setSize(axisInfo.size());
	setDescription(axisInfo.description());
	setUnits(axisInfo.units());
	setIsUniform(axisInfo.isUniform());
	setStart(axisInfo.start());
	setIncrement(axisInfo.increment());
}

AMDSAxisInfo::~AMDSAxisInfo()
{
}

QString AMDSAxisInfo::toString()
{
	return QString("%1 %2 %3 %4 %5 %6 %7").arg(name()).arg(description()).arg(units()).arg(size()).arg(isUniform()).arg(start()).arg(increment());
}
