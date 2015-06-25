#include "source/AMDSBufferGroupInfo.h"

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const QString &name, const QString &description, const QString &units, const QList<AMDSAxisInfo> &axes)
{
	name_ = name;
	description_ = description;
	units_ = units;
	axes_ = axes;
}

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const AMDSBufferGroupInfo &other) {
	name_ = other.name();
	description_ = other.description();
	units_ = other.units();
	axes_ = other.axes();
}

AMDSBufferGroupInfo& AMDSBufferGroupInfo::operator=(const AMDSBufferGroupInfo& other) {
	name_ = other.name();
	description_ = other.description();
	units_ = other.units();
	axes_ = other.axes();

	return *this;
}
