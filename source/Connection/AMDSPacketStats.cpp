#include "AMDSPacketStats.h"


AMDSPacketStats::AMDSPacketStats(const QString &name, quint64 inboundBytes, quint64 outboundBytes, quint64 maxInboundBytes, quint64 maxOutboundBytes, quint64 maxTotalBytes)
{
	name_ = name;
	inboundBytes_ = inboundBytes;
	outboundBytes_ = outboundBytes;
	maxInboundBytes_ = maxInboundBytes;
	maxOutboundBytes_ = maxOutboundBytes;
	maxTotalBytes_ = maxTotalBytes;
}

bool AMDSPacketStats::writeToDataStream(QDataStream *dataStream)
{
	*dataStream << name();
	*dataStream <<((quint64)(inboundBytes()));
	*dataStream <<((quint64)(outboundBytes()));
	*dataStream <<((quint64)(maxInboundBytes()));
	*dataStream <<((quint64)(maxOutboundBytes()));
	*dataStream <<((quint64)(maxTotalBytes()));

	return true;
}

bool AMDSPacketStats::readFromDataStream(QDataStream *dataStream)
{
	QString name;
	quint64 inboundBytes;
	quint64 outboundBytes;
	quint64 maxInboundBytes;
	quint64 maxOutboundBytes;
	quint64 maxTotalBytes;

	*dataStream >> name;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(inboundBytes);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(outboundBytes);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(maxInboundBytes);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(maxOutboundBytes);
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >>(maxTotalBytes);
	if(dataStream->status() != QDataStream::Ok)
		return false;

	setName(name);
	setInboundBytes(inboundBytes);
	setOutboundBytes(outboundBytes);
	setMaxInboundBytes(maxInboundBytes);
	setMaxOutboundBytes(maxOutboundBytes);
	setMaxTotalBytes(maxTotalBytes);

	return true;
}
