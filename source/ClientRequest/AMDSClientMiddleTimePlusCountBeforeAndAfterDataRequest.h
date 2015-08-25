#ifndef AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H
#define AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H

#include "AMDSClientDataRequest.h"

#include <QDateTime>

class AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest : public AMDSClientDataRequest
{
	Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(QObject *parent = 0);
	explicit AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, quint64 countBefore, quint64 countAfter, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest();

	/// Copy constructor
	AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other);
	/// Equal operator
	AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest& operator =(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other);

	/// Returns the middle time for the data request
	inline QDateTime middleTime() const { return middleTime_; }
	/// Returns the countBefore for the data request
	inline quint64 countBefore() const { return countBefore_; }
	/// Returns the countAfter for the data request
	inline quint64 countAfter() const { return countAfter_; }

	/// Sets the middle time for the data request
	inline void setMiddleTime(const QDateTime &startTime) { middleTime_ = startTime; }
	/// Sets the count before for the data request
	inline void setCountBefore(quint64 count) { countBefore_ = count; }
	/// Sets the count after for the data request
	inline void setCountAfter(quint64 count) { countAfter_ = count; }

	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an AMDSDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the AMDSDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// Middle time for data
	QDateTime middleTime_;
	/// count before
	quint64 countBefore_;
	/// count after
	quint64 countAfter_;
};

#endif // AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H
