#include "AMDSBufferGroupInfo.h"

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

QString AMDSBufferGroupInfo::toString()
{
	QString bufferGroupInfoDef = QString("%1 %2 %3 %4 %5 %6 %7").arg(name()).arg(description()).arg(units()).arg(rank()).arg(size().toString()).arg(flattenEnabled()?"true":"false").arg(flattenMethod());

	for(int x = 0, size = axes().count(); x < size; x++){
		AMDSAxisInfo axisInfo = axes().at(x);
		QString axisDef = QString("\tAxis (%1): %2").arg(x).arg(axisInfo.toString());
		bufferGroupInfoDef = QString("%1\n%2").arg(bufferGroupInfoDef).arg(axisDef);
	}

	return bufferGroupInfoDef;
}
