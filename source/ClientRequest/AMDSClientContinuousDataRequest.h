#ifndef AMDSCLIENTCONTINUOUSDATAREQUEST_H
#define AMDSCLIENTCONTINUOUSDATAREQUEST_H

#include "source/ClientRequest/AMDSClientDataRequest.h"

#include <QDateTime>

class AMDSClientContinuousDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientContinuousDataRequest(QObject *parent = 0);
	explicit AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientContinuousDataRequest();
	/// Copy constructor
	AMDSClientContinuousDataRequest(const AMDSClientContinuousDataRequest &other);

	/// Returns the start time for the data request
	inline QDateTime startTime() const { return startTime_; }

	AMDSClientContinuousDataRequest& operator =(const AMDSClientContinuousDataRequest &other);

	/// Sets the start time for the data request
	inline void setStartTime(const QDateTime &startTime) { startTime_ = startTime; }

	/// Writes this AMDSClientContinuousRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientContinuousRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
};

#endif // AMDSCLIENTCONTINUOUSDATAREQUEST_H
