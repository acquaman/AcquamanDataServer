#include "AMDSAxisInfo.h"

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

