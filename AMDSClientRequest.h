#ifndef AMDSCLIENTREQUEST_H
#define AMDSCLIENTREQUEST_H

#include "AMDSBufferGroupInfo.h"

class AMDSDataStream;

class AMDSClientRequest : public QObject
{
Q_OBJECT
public:
	enum RequestType{
		Introspection = 0,
		Statistics = 1,
		StartTimePlusCount = 2,
		RelativeCountPlusCount = 3,
		StartTimeToEndTime = 4,
		MiddleTimePlusCountBeforeAndAfter = 5,
		Continuous = 6,
		InvalidRequest = 7
	};

	enum ResponseType{
		Binary = 0,
		JSON = 1,
		Error = 2,
		InvalidResponse = 3
	};

	explicit AMDSClientRequest(QObject *parent = 0);
	explicit AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent = 0);
	virtual ~AMDSClientRequest();

	/// Copy constructor
	AMDSClientRequest(const AMDSClientRequest& other);

	/// A key used to identify the client socket on which the request was made
	inline QString socketKey() const { return socketKey_; }
	/// The last error message encountered attempting to fullfil the data request
	inline QString errorMessage() const { return errorMessage_; }
	/// The type of request. This is set by the constructor, depending on the arguments passed
	inline AMDSClientRequest::RequestType requestType() const { return requestType_; }
	/// The response type the client has specified. If an error is encountered, this will be changed
	/// to Error
	inline AMDSClientRequest::ResponseType responseType() const { return responseType_; }

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientRequest& operator=(const AMDSClientRequest& other);

	/// Sets the socket key identifier
	inline void setSocketKey(const QString &socketKey) { socketKey_ = socketKey; }
	/// Sets an error string describing the type of error encountered. Also sets the responseType to Error
	inline void setErrorMessage(const QString& errorMessage) { errorMessage_ = errorMessage; }
	/// Sets the request type
	inline void setRequestType(RequestType requestType) { requestType_ = requestType; }
	/// Sets the repsonse type
	inline void setResponseType(ResponseType responseType) { responseType_ = responseType; }

	/// Writes this AMDSClientRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	QString socketKey_;
	QString errorMessage_;
	AMDSClientRequest::RequestType requestType_;
	AMDSClientRequest::ResponseType responseType_;
};

class AMDSDataHolder;

class AMDSClientDataRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	explicit AMDSClientDataRequest(QObject *parent = 0);
	explicit AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequest::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QString &bufferName, bool includeStatusData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientDataRequest();

	/// Copy constructor
	AMDSClientDataRequest(const AMDSClientDataRequest& other);

	/// The string identifier for the buffer data is being request from or received from
	inline QString bufferName() const { return bufferName_; }
	/// Whether or not the client has requested that the statusData is included in the response
	inline bool includeStatusData() const { return includeStatusData_; }
	inline AMDSBufferGroupInfo bufferGroupInfo() const { return bufferGroupInfo_; }

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientDataRequest& operator=(const AMDSClientDataRequest& other);

	/// Sets the buffer identifier
	inline void setBufferName(const QString &bufferName) { bufferName_ = bufferName; }
	/// Sets the flag to include status meta data
	inline void setIncludeStatusData(bool includeStatusData) { includeStatusData_ = includeStatusData; }
	inline void setBufferGroupInfo(const AMDSBufferGroupInfo &bufferGroupInfo) { bufferGroupInfo_ = bufferGroupInfo; }
	/// Returns the list of data holders
	inline QList<AMDSDataHolder*> data() const { return data_; }

	/// Adds some data to the list of data holders
	inline void appendData(AMDSDataHolder *dataHolder) { data_.append(dataHolder); }
	/// Clears the list of data holders
	inline void clearData() { data_.clear(); }

	/// Writes this AMDSClienDatatRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientDataRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// The string identifier for the buffer data is being request from or received from
	QString bufferName_;
	/// Flag to determine if status meta data will be sent with the actual data
	bool includeStatusData_;

	AMDSBufferGroupInfo bufferGroupInfo_;

	/// List of data holders
	QList<AMDSDataHolder*> data_;
};

class AMDSClientIntrospectionRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	explicit AMDSClientIntrospectionRequest(QObject *parent = 0);
	explicit AMDSClientIntrospectionRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent = 0);
	virtual ~AMDSClientIntrospectionRequest();

	/// Copy constructor
	AMDSClientIntrospectionRequest(const AMDSClientIntrospectionRequest& other);

	/// The string identifier for the buffer data is being request from or received from
	inline QString bufferName() const { return bufferName_; }
	/// Returns the list of buffer group infos
	inline QList<AMDSBufferGroupInfo> bufferGroupInfos() const { return bufferGroupInfos_; }

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientIntrospectionRequest& operator=(const AMDSClientIntrospectionRequest& other);

	/// Sets the buffer identifier
	inline void setBufferName(const QString &bufferName) { bufferName_ = bufferName; }

	/// Adds a buffer group info to the list of buffer group infos
	inline void appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo) { bufferGroupInfos_.append(bufferGroupInfo); }
	/// Clears the list of buffer group infos
	inline void clearBufferGroupInfos() { bufferGroupInfos_.clear(); }

	/// Writes this AMDSClientIntrospectionRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientIntrospectionRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// The string identifier for the buffer or buffers being introspected
	QString bufferName_;

	/// Holds the introspection information
	QList<AMDSBufferGroupInfo> bufferGroupInfos_;
};

#include "AMDSPacketStats.h"

class AMDSClientStatisticsRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	explicit AMDSClientStatisticsRequest(QObject *parent = 0);
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

	/// Writes this AMDSClientStatisticsRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientStatisticsRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	QList<AMDSPacketStats> packetStats_;
};

#include <QDateTime>

class AMDSClientStartTimePlusCountDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	explicit AMDSClientStartTimePlusCountDataRequest(QObject *parent = 0);
	explicit AMDSClientStartTimePlusCountDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, bool includeStatusData, const QDateTime &startTime, quint64 count, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
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
	inline void setCount(quint64 count) { count_ = count; }

	/// Writes this AMDSClientStartTimePlusCountDataRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientStartTimePlusCountDataRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// Start time for data
	QDateTime startTime_;
	/// Number of data points after the start time to collect
	quint64 count_;
};

class AMDSClientContinuousDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	explicit AMDSClientContinuousDataRequest(QObject *parent = 0);
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

#endif // AMDSCLIENTREQUEST_H
