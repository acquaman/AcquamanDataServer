#ifndef AMDSCLIENTINTROSPECTIONREQUEST_H
#define AMDSCLIENTINTROSPECTIONREQUEST_H

#include "source/ClientRequest/AMDSClientRequest.h"

class AMDSClientIntrospectionRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientIntrospectionRequest(QObject *parent = 0);
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
	/// validate the message response
	virtual bool validateResponse();

protected:
	/// The string identifier for the buffer or buffers being introspected
	QString bufferName_;

	/// Holds the introspection information
	QList<AMDSBufferGroupInfo> bufferGroupInfos_;
};

#endif // AMDSCLIENTINTROSPECTIONREQUEST_H
