#include "AMDSBufferGroup.h"

AMDSBufferGroupInfo::AMDSBufferGroupInfo(const QString &name, const QString &description, const QString &units, const QList<AMDSAxisInfo> &axes)
{
	name_ = name;
	description_ = description;
	units_ = units;
	axes_ = axes;
}

AMDSnDIndex AMDSBufferGroupInfo::size() const {
	quint8 rank = axes_.count();
	AMDSnDIndex s(rank, AMDSnDIndex::DoNotInit);
	for(quint8 i=0; i<rank; ++i)
		s[i] = axes_.at(i).size();
	return s;
}


quint64 AMDSBufferGroupInfo::spanSize() const {
	quint64 aSize = 1;
	for(quint8 i=axes_.count()-1; i>=0; --i)
		aSize *= axes_.at(i).size();
	return aSize;
}

AMDSBufferGroup::AMDSBufferGroup(QObject *parent) :
	QObject(parent)
{
}
