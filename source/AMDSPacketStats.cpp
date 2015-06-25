#include "source/AMDSPacketStats.h"

AMDSPacketStats::AMDSPacketStats(const QString &name, quint64 inboundBytes, quint64 outboundBytes, quint64 maxInboundBytes, quint64 maxOutboundBytes, quint64 maxTotalBytes)
{
	name_ = name;
	inboundBytes_ = inboundBytes;
	outboundBytes_ = outboundBytes;
	maxInboundBytes_ = maxInboundBytes;
	maxOutboundBytes_ = maxOutboundBytes;
	maxTotalBytes_ = maxTotalBytes;
}
