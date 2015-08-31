#include "source/AMDSBufferGroupInfo.h"

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const QString &name, const QString &description, const QString &units, const bool flattenEnabled, const DataFlattenMethod flattenMethod, const QList<AMDSAxisInfo> &axes)
{
	setName(name);
	setDescription(description);
	setUnits(units);
	setFlattenEnabled(flattenEnabled);
	setFlattenMethod(flattenMethod);
	setAxes(axes);
}

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const AMDSBufferGroupInfo &other) {
	(*this) = other;
}

AMDSBufferGroupInfo& AMDSBufferGroupInfo::operator=(const AMDSBufferGroupInfo& other) {
	setName(other.name());
	setName(other.name());
	setDescription(other.description());
	setUnits(other.units());
	setFlattenEnabled(other.flattenEnabled());
	setFlattenMethod(other.flattenMethod());
	setAxes(other.axes());

	return *this;
}
