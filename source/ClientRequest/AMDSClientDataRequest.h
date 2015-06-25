#ifndef AMDSCLIENTDATAREQUEST_H
#define AMDSCLIENTDATAREQUEST_H

#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/AMDSDataTypeDefinitions.h"

class AMDSDataHolder;

class AMDSClientDataRequest : public AMDSClientRequest
{
Q_OBJECT
public:
	explicit AMDSClientDataRequest(QObject *parent = 0);
	explicit AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QString &bufferName, bool includeStatusData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
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

	inline AMDSDataTypeDefinitions::DataType uniformDataType() const { return uniformDataType_; }
	/// Returns the list of data holders
	inline QList<AMDSDataHolder*> data() const { return data_; }


	inline void setUniformDataType(AMDSDataTypeDefinitions::DataType uniformDataType) { uniformDataType_ = uniformDataType; }
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

	AMDSDataTypeDefinitions::DataType uniformDataType_;
	/// List of data holders
	QList<AMDSDataHolder*> data_;
};

#endif // AMDSCLIENTDATAREQUEST_H
