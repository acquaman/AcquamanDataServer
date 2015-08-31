#ifndef AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H
#define AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H

#include "AMDSClientDataRequest.h"

#include <QDateTime>

class AMDSClientStartTimeToEndTimeDataRequest : public AMDSClientDataRequest
{
    Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientStartTimeToEndTimeDataRequest(QObject *parent = 0);
	explicit AMDSClientStartTimeToEndTimeDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, const QDateTime &endTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientStartTimeToEndTimeDataRequest();

	/// Copy constructor
	AMDSClientStartTimeToEndTimeDataRequest(const AMDSClientStartTimeToEndTimeDataRequest &other);
	/// Equal operator
	AMDSClientStartTimeToEndTimeDataRequest& operator =(const AMDSClientStartTimeToEndTimeDataRequest &other);

	/// Returns the start time for the data request
	inline QDateTime startTime() const { return startTime_; }
	/// Returns the endtime for the data request
	inline QDateTime endTime() const { return endTime_; }

	/// Sets the start time for the data request
	inline void setStartTime(const QDateTime &startTime) { startTime_ = startTime; }
	/// Sets the end time for the data request
	inline void setEndTime(const QDateTime &endTime) { endTime_ = endTime; }

	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an AMDSDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the AMDSDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
	/// End time for data
	QDateTime endTime_;
};

#endif // AMDSCLIENTSTARTTIMETOENDTIMEDATAREQUEST_H
