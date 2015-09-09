#include "source/DataElement/AMDSAxisInfo.h"

AMDSAxisInfo::AMDSAxisInfo(const QString &name, quint32 size, const QString &description, const QString &units, bool isUniform)
{
	name_ = name;
	size_ = size;
	description_ = description;
	units_ = units;
	isUniform_ = isUniform;

	start_ = 0;
	increment_ = 1;
}

AMDSAxisInfo::~AMDSAxisInfo()
{
}

QString AMDSAxisInfo::toString() const
{
	return QString("%1 %2 %3 %4 %5 %6 %7").arg(name()).arg(description()).arg(units()).arg(size()).arg(isUniform()).arg(start()).arg(increment());
}
