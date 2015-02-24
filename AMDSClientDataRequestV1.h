#ifndef AMDSClientDataRequestV1_H
#define AMDSClientDataRequestV1_H

#include <QString>
#include <QTimer>
#include <QTime>

#include "AMDSBufferGroupInfo.h"
#include "AMDSPacketStats.h"

class AMDSDataHolder;

/**
  * Class representing a single request for data. Includes information regarding the nature of the request, an
  * identifier for the client socket through which the request was made, as well as a store to put the data in.
  */
class AMDSClientDataRequestV1 : public QObject
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

	/// Default constructor. Should never really be called. Is needed only because passing a AMDSClientDataRequestV1 across
	/// thread boundaries requires qRegisterMetaType, which in turn requires a default constructor
	explicit AMDSClientDataRequestV1(QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequestV1(const QDateTime& startTime,
				   quint64 count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject *parent = 0);
	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequestV1(int relativeCount,
				   quint64 count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject *parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequestV1(const QDateTime& startTime,
				   const QDateTime& endTime,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequestV1(const QDateTime& middleTime,
				   quint64 countBefore,
				   quint64 countAfter,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequestV1(const QDateTime& lastFetch,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   const QString &bufferName,
				   QObject* parent = 0);

	explicit AMDSClientDataRequestV1(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent = 0);

	explicit AMDSClientDataRequestV1(ResponseType responseType, const QString &socketKey, QObject *parent = 0);

	/// Copy constructor
	AMDSClientDataRequestV1(const AMDSClientDataRequestV1& other);


	/// A key used to identify the client socket on which the request was made
	inline QString socketKey() const { return socketKey_; }
	/// The type of request. This is set by the constructor, depending on the arguments passed
	inline AMDSClientDataRequestV1::RequestType requestType() const { return requestType_; }
	/// The response type the client has specified. If an error is encountered, this will be changed
	/// to Error
	inline AMDSClientDataRequestV1::ResponseType responseType() const { return responseType_; }
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
	inline QList<AMDSDataHolder*> data() const { return data_; }

	/// The last error message encountered attempting to fullfil the data request.
	inline QString lastError() const { return errorMessage_; }



	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientDataRequestV1& operator=(const AMDSClientDataRequestV1& other);
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

	inline void appendData(AMDSDataHolder *dataHolder) { data_.append(dataHolder); }
	inline void clearData() { data_.clear(); }

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
	QList<AMDSDataHolder*> data_;
};

void AMDSClientDataRequestV1::setErrorMessage(const QString &errorMessage)
{
	responseType_ = AMDSClientDataRequestV1::Error;
	errorMessage_ = errorMessage;
}

#endif // AMDSClientDataRequestV1_H
