#ifndef AMDSCLIENTDATAREQUEST_H
#define AMDSCLIENTDATAREQUEST_H

#include <QString>
#include <QTimer>
#include <QTime>

#include "AMDSBufferGroupInfo.h"


class AMDSPacketStats
{
public:
	AMDSPacketStats(const QString &name = QString(), quint64 inboundBytes = 0, quint64 outboundBytes = 0, quint64 maxInboundBytes = 0, quint64 maxOutboundBytes = 0, quint64 maxTotalBytes = 0);

	inline QString name() const { return name_; }

	inline quint64 inboundBytes() const { return inboundBytes_; }
	inline quint64 outboundBytes() const { return outboundBytes_; }
	inline quint64 maxInboundBytes() const { return maxInboundBytes_; }
	inline quint64 maxOutboundBytes() const { return maxOutboundBytes_; }
	inline quint64 maxTotalBytes() const { return maxTotalBytes_; }

	inline void setName(const QString &name) { name_ = name; }

	inline void setInboundBytes(quint64 inboundBytes) { inboundBytes_ = inboundBytes; }
	inline void setOutboundBytes(quint64 outboundBytes) { outboundBytes_ = outboundBytes; }
	inline void setMaxInboundBytes(quint64 maxInboundBytes) { maxInboundBytes_ = maxInboundBytes; }
	inline void setMaxOutboundBytes(quint64 maxOutboundBytes) { maxOutboundBytes_ = maxOutboundBytes; }
	inline void setMaxTotalBytes(quint64 maxTotalBytes) { maxTotalBytes_ = maxTotalBytes; }

	inline QString allStats() const { return QString("%1 %2 %3 %4 %5").arg(inboundBytes_).arg(outboundBytes_).arg(maxInboundBytes_).arg(maxOutboundBytes_).arg(maxTotalBytes_); }

protected:
	QString name_;
	quint64 inboundBytes_;
	quint64 outboundBytes_;
	quint64 maxInboundBytes_;
	quint64 maxOutboundBytes_;
	quint64 maxTotalBytes_;
};

/**
  * Class representing a single request for data. Includes information regarding the nature of the request, an
  * identifier for the client socket through which the request was made, as well as a store to put the data in.
  */
class AMDSClientDataRequest : public QObject
{
	Q_OBJECT
public:

	/// The type of request made. This is automatically set by the constructor depending on the passed arguments
	enum RequestType{
		Continuous = 0,
		StartTimePlusCount = 1,
		RelativeCountPlusCount = 2,
		StartTimeToEndTime = 3,
		MiddleTimePlusCountBeforeAndAfter = 4,
		Introspection = 5,
		Statistics = 6,
		Invalid = 7
	};

	/// The type of response the client has requested
	enum ResponseType{
		Binary = 0,
		Error = 1,
		Json = 2
	};

	/// Default constructor. Should never really be called. Is needed only because passing a AMDSClientDataRequest across
	/// thread boundaries requires qRegisterMetaType, which in turn requires a default constructor
	explicit AMDSClientDataRequest(QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& startTime,
				   quint64 count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject *parent = 0);
	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(int relativeCount,
				   quint64 count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject *parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& startTime,
				   const QDateTime& endTime,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& middleTime,
				   quint64 countBefore,
				   quint64 countAfter,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& lastFetch,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	explicit AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent = 0);

	explicit AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, QObject *parent = 0);

	/// Copy constructor
	AMDSClientDataRequest(const AMDSClientDataRequest& other);


	/// A key used to identify the client socket on which the request was made
	inline QString socketKey() const { return socketKey_; }
	/// The type of request. This is set by the constructor, depending on the arguments passed
	inline AMDSClientDataRequest::RequestType requestType() const { return requestType_; }
	/// The response type the client has specified. If an error is encountered, this will be changed
	/// to Error
	inline AMDSClientDataRequest::ResponseType responseType() const { return responseType_; }
	/// Whether or not the client has requested that the statusData is included in the response
	inline bool includeStatusData() const { return includeStatusData_; }

	/// These values are used differently, depending on the requestType. The number relates to the
	/// order of the parameter in function arguments. I.e. for StartTimePlusCount time1 is used for
	/// the startTime, and count1 is used for the count - all others are ignored
	inline QDateTime time1() const { return time1_; }
	inline QDateTime time2() const { return time2_; }
	inline quint64 count1() const { return count1_; }
	inline quint64 count2() const { return count2_; }
	/// A reference to the data store which is to be filled, then delivered to the client who made the request
//	AmptekSDD123DwellHistogramGroup* histogramData();
	inline QString bufferName() const { return bufferName_; }


	inline QList<AMDSBufferGroupInfo> bufferGroupInfos() const { return bufferGroupInfos_; }
	inline QList<AMDSPacketStats> packetStats() const { return packetStats_; }

	/// The last error message encountered attempting to fullfil the data request.
	inline QString lastError() const { return errorMessage_; }



	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientDataRequest& operator=(const AMDSClientDataRequest& other);
	void startContinuousRequestTimer(int msecs);


	inline void setSocketKey(const QString &socketKey) { socketKey_ = socketKey; }
	/// Sets an error string describing the type of error encountered. Also sets the responseType to Error
	inline void setErrorMessage(const QString& errorMessage);
	inline void setRequestType(RequestType requestType) { requestType_ = requestType; }
	inline void setResponseType(ResponseType responseType) { responseType_ = responseType; }
	inline void setIncludeStatusData(bool includeStatusData) { includeStatusData_ = includeStatusData; }
	inline void setTime1(const QDateTime &time1) { time1_ = time1; }
	inline void setTime2(const QDateTime &time2) { time2_ = time2; }
	inline void setCount1(quint64 count1) { count1_ = count1; }
	inline void setCount2(quint64 count2) { count2_ = count2; }
	inline void setBufferName(const QString &bufferName) { bufferName_ = bufferName; }

	inline void appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo) { bufferGroupInfos_.append(bufferGroupInfo); }
	inline void clearBufferGroupInfos() { bufferGroupInfos_.clear(); }

	inline void appendPacketStats(AMDSPacketStats packetStat) { packetStats_.append(packetStat); }
	inline void clearPacketStats() { packetStats_.clear(); }

signals:
	void sendNewContinuousDataRequest();

public slots:

protected:
	QString socketKey_;
	QString errorMessage_;
	RequestType requestType_;
	ResponseType responseType_;
	bool includeStatusData_;
	QDateTime time1_;
	QDateTime time2_;
	quint64 count1_;
	quint64 count2_;
	QString bufferName_;
//	AmptekSDD123DwellHistogramGroup histogramData_;
	QTimer continuousDataRequestTimer_;

	QList<AMDSBufferGroupInfo> bufferGroupInfos_;
	QList<AMDSPacketStats> packetStats_;
};

void AMDSClientDataRequest::setErrorMessage(const QString &errorMessage)
{
	responseType_ = AMDSClientDataRequest::Error;
	errorMessage_ = errorMessage;
}

#endif // AMDSCLIENTDATAREQUEST_H
