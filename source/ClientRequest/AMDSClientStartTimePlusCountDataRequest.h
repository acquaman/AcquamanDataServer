#ifndef AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H
#define AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H

#include <QDateTime>

#include "ClientRequest/AMDSClientDataRequest.h"


class AMDSClientStartTimePlusCountDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientStartTimePlusCountDataRequest(QObject *parent = 0);
	explicit AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
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
	/// Sets the values of all the data attributes of client request
	void setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, quint64 count);

protected:
	/// Writes this AMDSClientStartTimePlusCountDataRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream) const;
	/// Reads this AMDSClientStartTimePlusCountDataRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
	/// Number of data points after the start time to collect, must be > 0.
	quint64 count_;
};

#endif // AMDSCLIENTSTARTTIMEPLUSCOUNTDATAREQUEST_H
