#include "AMDSAxisInfo.h"

AMDSAxisInfo::AMDSAxisInfo(const QString &name, quint32 size, const QString &description, const QString &units)
{
	name_ = name;
	size_ = size;
	description_ = description;
	units_ = units;
}

AMDSAxisInfo::~AMDSAxisInfo()
{
}

