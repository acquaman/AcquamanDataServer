#include "AMDSClientDataRequest.h"

#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataHolder/AMDSDataHolderSupport.h"
#include "util/AMErrorMonitor.h"

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	bufferName_ = "Invalid";
	includeStatusData_ = false;
	bufferGroupInfo_.setName("Invalid");

	setFlattenResultData(false);
}

AMDSClientDataRequest::AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QDateTime &localDateTime, const QString &bufferName, bool includeStatusData, bool flattenResultData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientRequest(socketKey, errorMessage, requestType, responseType, localDateTime, parent)
{
	bufferName_ = bufferName;
	includeStatusData_ = includeStatusData;
	bufferGroupInfo_ = bufferGroupInfo;

	setFlattenResultData(flattenResultData);
}

AMDSClientDataRequest::~AMDSClientDataRequest()
{
	clearData();
}

AMDSClientDataRequest::AMDSClientDataRequest(const AMDSClientDataRequest &other) :
	AMDSClientRequest(other)
{
	(*this) = other;
}

AMDSClientDataRequest& AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		setBufferName(other.bufferName());
		setIncludeStatusData(other.includeStatusData());

		bufferGroupInfo_ = other.bufferGroupInfo_;

		setFlattenResultData(other.flattenResultData());
		uniformDataType_ = other.uniformDataType();
		clearData();
		for(int x = 0, size = other.data().count(); x < size; x++)
			copyAndAppendData(other.data().at(x));
	}
	return (*this);
}

void AMDSClientDataRequest::copyAndAppendData(AMDSDataHolder *dataHolder)
{
	AMDSDataHolder *cloneDataHolder = AMDSDataHolderSupport::instantiateDataHolderFromInstance(dataHolder);
	if (cloneDataHolder) {
		(*cloneDataHolder) = (*dataHolder);

		data_.append(cloneDataHolder);
	} else {
		AMErrorMon::error(this, AMDS_ALERT_DATA_HOLDER_TYPE_NOT_SUPPORT, QString("This type (%1) of dataHolder might NOT be registered. Please contact Acquaman Developers.").arg(dataHolder->metaObject()->className()));
	}
}

void AMDSClientDataRequest::clearData()
{
	foreach(AMDSDataHolder *dataHolder, data_) {
		dataHolder->deleteLater();
	}
	data_.clear();
}

bool AMDSClientDataRequest::validateResponse()
{
	bool noError = true;
	if(responseType() == AMDSClientRequest::Error) {
		AMErrorMon::alert(this, AMDS_CLIENTREQUEST_ERR_FAILED_TO_RETRIEVE_DATA, QString("(msg %1 --- %2) Failed to retrieve data. Error: %3").arg(socketKey()).arg(bufferName()).arg(errorMessage()));
		noError = false;
	}

	return noError;
}

QString AMDSClientDataRequest::toString() const
{
	QString messageData = QString("Data of Data message (%1 -- %2):").arg(socketKey()).arg(bufferName());

	if (data().count() == 0)
		messageData = QString("%1\n\tNo data for this message yet").arg(messageData);
	else {
		messageData = QString("The data type looks like %1").arg(data().at(0)->metaObject()->className());
		for(int x = 0, size = data().count(); x < size; x++){
			messageData = QString("%1\n\tData at %2 - %3").arg(messageData).arg(x).arg(data().at(x)->printData());
		}
	}

	return messageData;
}

int AMDSClientDataRequest::writeToDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if(errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;
	*dataStream << includeStatusData_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS;
	*dataStream << flattenResultData_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_FLATTEN_RESULT_DATA;

	bool includeData = bufferGroupInfo_.includeData();
	*dataStream << includeData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA;

	// write data to the data stream
	if(includeData){
		bufferGroupInfo_.writeToDataStream(dataStream);

		quint8 uniformDataType = (quint8)uniformDataType_;
		*dataStream << uniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE;

		quint16 dataCount = data_.count();
		*dataStream << dataCount;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT;

		for(int x = 0, size = data_.count(); x < size; x++){
			if (!AMDSDataHolder::encodeAndwriteDataHolder(dataStream, data_.at(x)))
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA;
		}
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientDataRequest::readFromDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::readFromDataStream(dataStream);
	if(errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QString readBufferName;
	bool readIncludeStatusData;
	bool readFlattenResultData;
	bool readIncludeData;
	AMDSBufferGroupInfo readBufferGroupInfo;
	quint8 readUniformDataType;
	quint16 readDataCount;
	QList<AMDSDataHolder*> readDataHolder;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	*dataStream >> readIncludeStatusData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_STATUS;

	*dataStream >> readFlattenResultData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_FLATTEN_RESULT_DATA;

	*dataStream >> readIncludeData;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_INCLUDE_DATA;

	if(readIncludeData){
		if (! readBufferGroupInfo.readFromDataStream(dataStream))
			return false;

		*dataStream >> readUniformDataType;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_UNIFORM_DATA_TYPE;

		*dataStream >> readDataCount;
		if(dataStream->status() != QDataStream::Ok)
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_COUNT;

		AMDSDataHolder *oneDataHolder = 0;
		for(int x = 0; x < readDataCount; x++){
			oneDataHolder = AMDSDataHolder::decodeAndInstantiateDataHolder(dataStream);
			if (oneDataHolder == 0)
				return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_DATA_HOLDER_DATA;

			readDataHolder.append(oneDataHolder);
		}
	}

	// set the values of the variables
	setBufferName(readBufferName);
	setIncludeStatusData(readIncludeStatusData);
	setFlattenResultData(readFlattenResultData);
	if(readIncludeData){
		setBufferGroupInfo(readBufferGroupInfo);

		// do some data setting here
		setUniformDataType((AMDSDataTypeDefinitions::DataType)readUniformDataType);

		clearData();
		data_.append(readDataHolder);
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}
