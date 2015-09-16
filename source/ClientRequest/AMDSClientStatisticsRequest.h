#ifndef AMDSCLIENTSTATISTICSREQUEST_H
#define AMDSCLIENTSTATISTICSREQUEST_H

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/Connection/AMDSPacketStats.h"

class AMDSClientStatisticsRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientStatisticsRequest(QObject *parent = 0);
	explicit AMDSClientStatisticsRequest(AMDSClientRequest::ResponseType responseType, const QString socketKey, QObject *parent = 0);
	virtual ~AMDSClientStatisticsRequest();

	/// Copy constructor
	AMDSClientStatisticsRequest(const AMDSClientStatisticsRequest &other);

	/// Returns the list of packet stats
	inline QList<AMDSPacketStats> packetStats() const { return packetStats_; }

	AMDSClientStatisticsRequest& operator=(const AMDSClientStatisticsRequest &other);

	/// Adds packet stats to the list of packet stats
	inline void appendPacketStats(AMDSPacketStats packetStat) { packetStats_.append(packetStat); }
	/// Clears the list of packet stats
	inline void clearPacketStats() { packetStats_.clear(); }

	/// Writes this AMDSClientStatisticsRequest to an AMDSDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientStatisticsRequest from the AMDSDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(AMDSDataStream *dataStream);

	/// implement the function to return a data string of the message
	virtual QString toString() const;

protected:
	QList<AMDSPacketStats> packetStats_;
};

#endif // AMDSCLIENTSTATISTICSREQUEST_H
