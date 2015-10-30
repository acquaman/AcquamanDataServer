#ifndef AMDSPACKETSTATS_H
#define AMDSPACKETSTATS_H

#include <QObject>
#include <QDataStream>


class AMDSPacketStats
{
public:
	AMDSPacketStats(const QString &name = QString(), quint64 inboundBytes = 0, quint64 outboundBytes = 0, quint64 maxInboundBytes = 0, quint64 maxOutboundBytes = 0, quint64 maxTotalBytes = 0);

	/// Writes this AMDSPacketStats to an QDataStream, returns true if no errors are encountered
	bool writeToDataStream(QDataStream *dataStream) const;
	/// Reads this AMDSPacketStats from the QDataStream, returns true if no errors are encountered
	bool readFromDataStream(QDataStream *dataStream);

	inline QString name() const { return name_; }

	inline quint64 inboundBytes() const { return inboundBytes_; }
	inline quint64 outboundBytes() const { return outboundBytes_; }
	inline quint64 maxInboundBytes() const { return maxInboundBytes_; }
	inline quint64 maxOutboundBytes() const { return maxOutboundBytes_; }
	inline quint64 maxTotalBytes() const { return maxTotalBytes_; }

	inline void setName(const QString &name) { name_ = name; }

	inline void setInboundBytes(quint64 inboundBytes) { inboundBytes_ = inboundBytes; }
	inline void setOutboundBytes(quint64 outboundBytes) { outboundBytes_ = outboundBytes; }
	inline void setMaxInboundBytes(quint64 maxInboundBytes) { maxInboundBytes_ = maxInboundBytes; }
	inline void setMaxOutboundBytes(quint64 maxOutboundBytes) { maxOutboundBytes_ = maxOutboundBytes; }
	inline void setMaxTotalBytes(quint64 maxTotalBytes) { maxTotalBytes_ = maxTotalBytes; }

	inline QString toString() const { return QString("%1: %2 %3 %4 %5 %6").arg(name_).arg(inboundBytes_).arg(outboundBytes_).arg(maxInboundBytes_).arg(maxOutboundBytes_).arg(maxTotalBytes_); }

protected:
	QString name_;
	quint64 inboundBytes_;
	quint64 outboundBytes_;
	quint64 maxInboundBytes_;
	quint64 maxOutboundBytes_;
	quint64 maxTotalBytes_;
};

#endif // AMDSPACKETSTATS_H
