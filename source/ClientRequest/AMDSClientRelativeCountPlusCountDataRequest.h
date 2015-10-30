#ifndef AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H
#define AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H

#include "ClientRequest/AMDSClientDataRequest.h"

class AMDSClientRelativeCountPlusCountDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientRelativeCountPlusCountDataRequest(QObject *parent = 0);
	explicit AMDSClientRelativeCountPlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool flattenResultData, const quint64 relativeCount, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientRelativeCountPlusCountDataRequest();

	/// Copy constructor
	AMDSClientRelativeCountPlusCountDataRequest(const AMDSClientRelativeCountPlusCountDataRequest &other);
	/// Equal operator
	AMDSClientRelativeCountPlusCountDataRequest& operator =(const AMDSClientRelativeCountPlusCountDataRequest &other);

	/// Returns the relative count for the data request
	inline quint64 relativeCount() const { return relativeCount_; }
	/// Returns the number of data points after the start time to collect
	inline quint64 count() const { return count_; }

	/// set the relative count for the data request
	inline void setRelativeCount(quint64 count) { relativeCount_ = count; }
	/// set the number of data points after the start time to collect
	inline void setCount(quint64 count) { count_ = (count==0 ? 1 : count); }

	/// Sets the values of all the data attributes of client request
	void setAttributesValues(const QString &bufferName, bool includeStatusData, bool flattenResultData, quint64 relativeCount, quint64 count);

protected:
	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream) const;
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

protected:
	/// the relative count to start
	quint64 relativeCount_;
	/// Number of data points after the relative count to collect
	quint64 count_;
};

#endif // AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H
