#ifndef AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H
#define AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H

#include "source/ClientRequest/AMDSClientDataRequest.h"

#include <QDateTime>

class AMDSClientStartTimePlusCountDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientStartTimePlusCountDataRequest(QObject *parent = 0);
	explicit AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool enableFlattening, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientStartTimePlusCountDataRequest();

	/// Copy constructor
	AMDSClientStartTimePlusCountDataRequest(const AMDSClientStartTimePlusCountDataRequest &other);

	/// Returns the start time for the data request
	inline QDateTime startTime() const { return startTime_; }
	/// Returns the number of data points after the start time to collect
	inline quint64 count() const { return count_; }

	AMDSClientStartTimePlusCountDataRequest& operator =(const AMDSClientStartTimePlusCountDataRequest &other);

	/// Sets the start time for the data request
	inline void setStartTime(const QDateTime &startTime) { startTime_ = startTime; }
	/// Sets the number of data points after the start time to collect
	inline void setCount(quint64 count) { count_ = qMax((quint64)1, count) ; }

	/// Writes this AMDSClientStartTimePlusCountDataRequest to an AMDSDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientStartTimePlusCountDataRequest from the AMDSDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
	/// Number of data points after the start time to collect, must be > 0.
	quint64 count_;
};

#endif // AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H
