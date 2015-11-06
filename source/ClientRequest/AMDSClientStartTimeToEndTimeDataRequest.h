#ifndef AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H
#define AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H

#include <QDateTime>

#include "ClientRequest/AMDSClientDataRequest.h"

class AMDSClientStartTimeToEndTimeDataRequest : public AMDSClientDataRequest
{
    Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientStartTimeToEndTimeDataRequest(QObject *parent = 0);
	explicit AMDSClientStartTimeToEndTimeDataRequest(ResponseType responseType, const QDateTime &localDateTime, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, const QDateTime &endTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientStartTimeToEndTimeDataRequest();

	/// Copy constructor
	AMDSClientStartTimeToEndTimeDataRequest(const AMDSClientStartTimeToEndTimeDataRequest &other);
	/// Equal operator
	AMDSClientStartTimeToEndTimeDataRequest& operator =(const AMDSClientStartTimeToEndTimeDataRequest &other);

	/// Returns the start time for the data request
	inline QDateTime startTime() const { return startTime_.addMSecs(timeDelta()); }
	/// Returns the endtime for the data request
	inline QDateTime endTime() const { return endTime_.addMSecs(timeDelta()); }

	/// Sets the start time for the data request
	inline void setStartTime(const QDateTime &startTime) { startTime_ = startTime; }
	/// Sets the end time for the data request
	inline void setEndTime(const QDateTime &endTime) { endTime_ = endTime; }

	/// Sets the values of all the data attributes of client request
	void setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, const QDateTime &endTime);

protected:
	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream);
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
	/// End time for data
	QDateTime endTime_;
};

#endif // AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H
