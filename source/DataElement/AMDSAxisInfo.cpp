#include "DataElement/AMDSAxisInfo.h"

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

bool AMDSAxisInfo::readFromDataStream(QDataStream *dataStream)
{
	QString name;
	QString description;
	QString units;
	quint32 size;
	bool isUniform;
	qint16 start;
	quint16 increment;

	*dataStream >> name;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> description;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> units;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(size);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(isUniform);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(start);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(increment);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setName(name);
	setDescription(description);
	setUnits(units);
	setSize(size);
	setIsUniform(isUniform);
	setStart(start);
	setIncrement(increment);

	return true;
}

bool AMDSAxisInfo::writeToDataStream(QDataStream *dataStream)
{
	*dataStream << name();
	*dataStream << description();
	*dataStream << units();
	*dataStream << size();
	*dataStream << isUniform();
	*dataStream << start();
	*dataStream << increment();

	return true;
}

QString AMDSAxisInfo::toString() const
{
	return QString("%1 %2 %3 %4 %5 %6 %7").arg(name()).arg(description()).arg(units()).arg(size()).arg(isUniform()).arg(start()).arg(increment());
}
