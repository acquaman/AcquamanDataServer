#ifndef AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H
#define AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H

#include "source/ClientRequest/AMDSClientDataRequest.h"

class AMDSClientRelativeCountPlusCountDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientRelativeCountPlusCountDataRequest(QObject *parent = 0);
	explicit AMDSClientRelativeCountPlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, bool enableFlattening, const quint64 relativeCount, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
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

	/// Writes this AMDSClientRelativeCountPlusCountDataRequest to an AMDSDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientRelativeCountPlusCountDataRequest from the AMDSDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// the relative count to start
	quint64 relativeCount_;
	/// Number of data points after the relative count to collect
	quint64 count_;
};

#endif // AMDSCLIENTRELATIVECOUNTPLUSCOUNTDATAREQUEST_H
