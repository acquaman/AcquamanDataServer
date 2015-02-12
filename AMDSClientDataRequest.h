#ifndef AMDSCLIENTDATAREQUEST_H
#define AMDSCLIENTDATAREQUEST_H

#include <QString>
#include <QTimer>
#include <QTime>

#include "AMDSBufferGroupInfo.h"

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
		Introspection = 5
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
				   int count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   int amptekIndex,
				   QObject *parent = 0);
	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(int relativeCount,
				   int count,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   int amptekIndex,
				   QObject *parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& startTime,
				   const QDateTime& endTime,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   int amptekIndex,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& middleTime,
				   int countBefore,
				   int countAfter,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   int amptekIndex,
				   QObject* parent = 0);

	/// Constructs a data request using the provided arguments
	explicit AMDSClientDataRequest(const QDateTime& lastFetch,
				   bool includeStatusData,
				   ResponseType responseType,
				   const QString& socketKey,
				   int amptekIndex,
				   QObject* parent = 0);

	explicit AMDSClientDataRequest(ResponseType responseType, const QString &socketKey, QObject *parent = 0);

	/// Copy constructor
	AMDSClientDataRequest(const AMDSClientDataRequest& other);


	/// A key used to identify the client socket on which the request was made
	QString socketKey() const;
	/// The type of request. This is set by the constructor, depending on the arguments passed
	RequestType requestType() const;
	/// The response type the client has specified. If an error is encountered, this will be changed
	/// to Error
	ResponseType responseType() const;
	/// Whether or not the client has requested that the statusData is included in the response
	bool includeStatusData() const;

	/// These values are used differently, depending on the requestType. The number relates to the
	/// order of the parameter in function arguments. I.e. for StartTimePlusCount time1 is used for
	/// the startTime, and count1 is used for the count - all others are ignored
	QDateTime time1() const;
	QDateTime time2() const;
	int count1() const;
	int count2() const;
	/// The index of the amptek detector the request is being made to
	int indexOfAmptek() const;
	/// A reference to the data store which is to be filled, then delivered to the client who made the request
//	AmptekSDD123DwellHistogramGroup* histogramData();


	QList<AMDSBufferGroupInfo> bufferGroupInfos() const;

	/// Sets an error string describing the type of error encountered. Also sets the responseType to Error
	void setError(const QString& errorMessage);
	/// The last error message encountered attempting to fullfil the data request.
	QString lastError() const;

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientDataRequest& operator=(const AMDSClientDataRequest& other);
	void startContinuousRequestTimer(int msecs);


	void setRequestType(RequestType requestType);
	void setResponseType(ResponseType responseType);
	void setTime1(const QDateTime& time);

	//void setBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo);
	void appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo);
	void clearBufferGroupInfos();

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
	int count1_;
	int count2_;
	int indexOfAmptek_;
//	AmptekSDD123DwellHistogramGroup histogramData_;
	QTimer continuousDataRequestTimer_;

	QList<AMDSBufferGroupInfo> bufferGroupInfos_;
};

#endif // AMDSCLIENTDATAREQUEST_H
