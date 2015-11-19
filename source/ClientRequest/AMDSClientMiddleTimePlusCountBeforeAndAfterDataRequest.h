#ifndef AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H
#define AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H

#include <QDateTime>

#include "ClientRequest/AMDSClientDataRequest.h"

class AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest : public AMDSClientDataRequest
{
	Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(QObject *parent = 0);
	explicit AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(ResponseType responseType, const QDateTime &localDateTime, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &startTime, quint64 countBefore, quint64 countAfter, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest();

	/// Copy constructor
	AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other);
	/// Equal operator
	AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest& operator =(const AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest &other);

	/// Returns the middle time for the data request
	inline QDateTime middleTime() const { return middleTime_.addMSecs(timeDelta()); }
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

	/// Sets the values of all the data attributes of client request
	void setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, const QDateTime &MiddleTime, quint64 countBefore, quint64 countAfter);

protected:
	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream);
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

protected:
	/// Middle time for data
	QDateTime middleTime_;
	/// count before
	quint64 countBefore_;
	/// count after
	quint64 countAfter_;
};

#endif // AMDSCLIENTMIDDLETIMEPLUSCOUNTBEFOREANDAFTERDATAREQUEST_H
