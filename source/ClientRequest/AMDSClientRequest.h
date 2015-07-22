#ifndef AMDSCLIENTREQUEST_H
#define AMDSCLIENTREQUEST_H

#include "source/ClientRequest/AMDSClientRequestDefinitions.h"
#include "source/AMDSBufferGroupInfo.h"

class AMDSDataStream;

class AMDSClientRequest : public QObject
{
Q_OBJECT
public:
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

	/// A key used to identify the client socket on which the request was made
	inline QString socketKey() const { return socketKey_; }
	/// The last error message encountered attempting to fullfil the data request
	inline QString errorMessage() const { return errorMessage_; }
	/// The type of request. This is set by the constructor, depending on the arguments passed
	inline AMDSClientRequestDefinitions::RequestType requestType() const { return requestType_; }
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
	inline void setRequestType(AMDSClientRequestDefinitions::RequestType requestType) { requestType_ = requestType; }
	/// Sets the repsonse type
	inline void setResponseType(ResponseType responseType) { responseType_ = responseType; }

	/// Writes this AMDSClientRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

private:
	/// To set the values of all the properties
	void setProperties(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType);

protected:
	QString socketKey_;
	QString errorMessage_;
	AMDSClientRequestDefinitions::RequestType requestType_;
	AMDSClientRequest::ResponseType responseType_;
};

#endif // AMDSCLIENTREQUEST_H
