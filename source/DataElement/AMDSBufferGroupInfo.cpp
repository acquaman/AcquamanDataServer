#include "AMDSBufferGroupInfo.h"

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const QString &name, const QString &description, const QString &units, AMDSDataTypeDefinitions::DataType dataType, const DataFlattenMethod flattenMethod, const QList<AMDSAxisInfo> &axes)
{
	setName(name);
	setDescription(description);
	setUnits(units);
	setFlattenMethod(flattenMethod);
	setAxes(axes);
	setDataType(dataType);
}

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const AMDSBufferGroupInfo &other) {
	(*this) = other;
}

AMDSBufferGroupInfo& AMDSBufferGroupInfo::operator=(const AMDSBufferGroupInfo& other) {
	setName(other.name());
	setName(other.name());
	setDescription(other.description());
	setUnits(other.units());
	setFlattenMethod(other.flattenMethod());
	setAxes(other.axes());
	setDataType(other.dataType());

	return *this;
}

bool AMDSBufferGroupInfo::writeToDataStream(QDataStream *dataStream) const
{
	*dataStream << name();
	*dataStream << description();
	*dataStream << units();
	*dataStream << ((quint8)flattenMethod());
	*dataStream <<((quint8)(axes().count()));
	for(int x = 0, size = axes().count(); x < size; x++) {
		axes().at(x).writeToDataStream(dataStream);
	}

	return true;
}

bool AMDSBufferGroupInfo::readFromDataStream(QDataStream *dataStream)
{
	QString name;
	QString description;
	QString units;
	quint8 axesCount;
	quint8 flattenMethod;
	QList<AMDSAxisInfo> axes;

	*dataStream >> name;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> description;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> units;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> flattenMethod;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(axesCount);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = axesCount; x < size; x++){
		AMDSAxisInfo oneAxisInfo("Invalid", 0);
		oneAxisInfo.readFromDataStream(dataStream);

		if(oneAxisInfo.name() == "Invalid")
			return false;
		axes.append(oneAxisInfo);
	}

	setName(name);
	setDescription(description);
	setUnits(units);
	setFlattenMethod((DataFlattenMethod)flattenMethod);
	setAxes(axes);

	return true;
}

QString AMDSBufferGroupInfo::toString() const
{
	QString flattenMethodStr;
	switch(flattenMethod()) {
	case Average:
		flattenMethodStr = "Average";
		break;
	case Summary:
		flattenMethodStr = "Summary";
		break;
	case NoFlatten:
		flattenMethodStr = "NoFlatten";
		break;
	default:
		flattenMethodStr = "Unknown";
		break;
	}

	QString bufferGroupInfoDef = QString("%1 %2 %3 %4 %5 %6").arg(name()).arg(description()).arg(units()).arg(rank()).arg(size().toString()).arg(flattenMethodStr);

	for(int x = 0, size = axes().count(); x < size; x++){
		AMDSAxisInfo axisInfo = axes().at(x);
		QString axisDef = QString("\tAxis (%1): %2").arg(x).arg(axisInfo.toString());
		bufferGroupInfoDef = QString("%1\n%2").arg(bufferGroupInfoDef).arg(axisDef);
	}

	return bufferGroupInfoDef;
}
