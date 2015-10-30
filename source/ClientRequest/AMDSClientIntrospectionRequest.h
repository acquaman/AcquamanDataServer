#ifndef AMDSCLIENTINTROSPECTIONREQUEST_H
#define AMDSCLIENTINTROSPECTIONREQUEST_H

#include <QStringList>

#include "ClientRequest/AMDSClientRequest.h"

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
	/// Returns whether it is ready to read data
	inline bool readReady() { return bufferGroupInfos_.count() > 0; }
	/// Returns whether we are looking for all buffers now
	inline bool checkAllBuffer() { return bufferName_ == "All"; }
	/// Returns the list of names of all the buffers
	QStringList getAllBufferNames();

	/// Overload of the assignment operator. Performs a deep copy. DOES NOT MAINTAIN QOBJECT PARENTAGE.
	AMDSClientIntrospectionRequest& operator=(const AMDSClientIntrospectionRequest& other);

	/// Sets the buffer identifier
	inline void setBufferName(const QString &bufferName) { bufferName_ = bufferName; }

	/// Adds a buffer group info to the list of buffer group infos
	inline void appendBufferGroupInfo(AMDSBufferGroupInfo bufferGroupInfo) { bufferGroupInfos_.append(bufferGroupInfo); }
	/// Clears the list of buffer group infos
	inline void clearBufferGroupInfos() { bufferGroupInfos_.clear(); }

protected:
	/// Writes this AMDSClientIntrospectionRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream);
	/// Reads this AMDSClientIntrospectionRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

	/// implement the function to return a data string of the message
	virtual QString toString() const;

protected:
	/// The string identifier for the buffer or buffers being introspected
	QString bufferName_;

	/// Holds the introspection information
	QList<AMDSBufferGroupInfo> bufferGroupInfos_;
};

#endif // AMDSCLIENTINTROSPECTIONREQUEST_H
