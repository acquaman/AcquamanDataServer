#ifndef AMDSCLIENTREQUEST_H
#define AMDSCLIENTREQUEST_H

#include <QDataStream>

#include "ClientRequest/AMDSClientRequestDefinitions.h"
#include "DataElement/AMDSBufferGroupInfo.h"

class AMDSClientRequest : public QObject
{
Q_OBJECT
public:
	static AMDSClientRequest* decodeAndInstantiateClientRequest(QDataStream *dataStream);
	static bool encodeAndwriteClientRequest(QDataStream *dataStream, AMDSClientRequest *clientRequest);

	enum ResponseType{
		Binary = 0,
		JSON = 1,
		Error = 2,
		InvalidResponse = 3
	};

	explicit AMDSClientRequest(QObject *parent = 0);
	explicit AMDSClientRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, QObject *parent = 0);
	virtual ~AMDSClientRequest();

	/// Copy constructor
	AMDSClientRequest(const AMDSClientRequest& other);

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientRequest& operator=(const AMDSClientRequest& other);

	/// returns whether this is an instrospection message
	inline bool isInstrospectionMessage() { return requestType() == AMDSClientRequestDefinitions::Introspection; }
	/// returns whether this is a statistics message
	inline bool isStatisticsMessage() { return requestType() == AMDSClientRequestDefinitions::Statistics; }
	/// returns whether this is a data client request
	bool isDataClientRequest();
	/// returns whether this is a configuration client request
	bool isConfigurationClientRequest();
	/// returns whether this is a continuous message
	inline bool isContinuousMessage() { return requestType() == AMDSClientRequestDefinitions::Continuous;}

	/// A key used to identify the client socket on which the request was made
	inline QString socketKey() const { return socketKey_; }
	/// The last error message encountered attempting to fullfil the data request
	inline QString errorMessage() const { return errorMessage_; }
	/// The type of request. This is set by the constructor, depending on the arguments passed
	inline AMDSClientRequestDefinitions::RequestType requestType() const { return requestType_; }
	/// The response type the client has specified. If an error is encountered, this will be changed
	/// to Error
	inline AMDSClientRequest::ResponseType responseType() const { return responseType_; }

	/// Sets the socket key identifier
	virtual void setSocketKey(const QString &socketKey) { socketKey_ = socketKey; }
	/// Sets an error string describing the type of error encountered. Also sets the responseType to Error
	inline void setErrorMessage(const QString& errorMessage) { errorMessage_ = errorMessage; }
	/// Sets the request type
	inline void setRequestType(AMDSClientRequestDefinitions::RequestType requestType) { requestType_ = requestType; }
	/// Sets the repsonse type
	inline void setResponseType(ResponseType responseType) { responseType_ = responseType; }

	/// print out the message data
	void printData();
	/// validate the message response
	virtual bool validateResponse() {return true;}
	/// pure virtual to return a data string of the message
	virtual QString toString() const = 0;

protected:
	/// Writes this AMDSClientRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream);
	/// Reads this AMDSClientRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

private:
	/// To set the values of all the attributes
	void setBaseAttributesValues(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType);

protected:
	/// the socket key to identify a connection
	QString socketKey_;
	/// the error message if an error is detected
	QString errorMessage_;
	/// the request type of the message
	AMDSClientRequestDefinitions::RequestType requestType_;
	/// the response type of the message
	AMDSClientRequest::ResponseType responseType_;
};

#endif // AMDSCLIENTREQUEST_H
