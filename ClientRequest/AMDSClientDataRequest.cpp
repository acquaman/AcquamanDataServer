#include "AMDSClientDataRequest.h"

#include "AMDSDataStream.h"
#include "AMDSScalarDataHolder.h"

AMDSClientDataRequest::AMDSClientDataRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	bufferName_ = "Invalid";
	includeStatusData_ = false;
	bufferGroupInfo_.setName("Invalid");
}

AMDSClientDataRequest::AMDSClientDataRequest(const QString &socketKey, const QString &errorMessage, AMDSClientRequestDefinitions::RequestType requestType, AMDSClientRequest::ResponseType responseType, const QString &bufferName, bool includeStatusData, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientRequest(socketKey, errorMessage, requestType, responseType, parent)
{
	bufferName_ = bufferName;
	includeStatusData_ = includeStatusData;
	bufferGroupInfo_ = bufferGroupInfo;
}

AMDSClientDataRequest::~AMDSClientDataRequest()
{
}

AMDSClientDataRequest::AMDSClientDataRequest(const AMDSClientDataRequest &other) :
	AMDSClientRequest(0)
{
	(*this) = other;
}

AMDSClientDataRequest& AMDSClientDataRequest::operator =(const AMDSClientDataRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);
		bufferName_ = other.bufferName();
		includeStatusData_ = other.includeStatusData();
		bufferGroupInfo_ = other.bufferGroupInfo_;

		clearData();
		for(int x = 0, size = other.data().count(); x < size; x++)
			appendData(other.data().at(x));
	}
	return (*this);
}

bool AMDSClientDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientDataRequest::writeToDataStream";
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientDataRequest::writeToDataStream";

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream << includeStatusData_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	bool includeData;
	if(bufferGroupInfo_.name() == "Invalid"){
		includeData = false;
		*dataStream << includeData;
		if(dataStream->status() != QDataStream::Ok)
			return false;
	}
	else{
		includeData = true;
		*dataStream << includeData;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		dataStream->write(bufferGroupInfo_);
		quint16 dataCount = data_.count();
		*dataStream << dataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		// do some data writing here

		for(int x = 0, xSize = data_.count(); x < xSize; x++){
			QVector<double> oneClientDataRequestVector = QVector<double>(bufferGroupInfo_.spanSize());
			data_.at(x)->data(oneClientDataRequestVector.data());
			for(int y = 0, ySize = bufferGroupInfo_.spanSize(); y < ySize; y++)
				*dataStream << oneClientDataRequestVector[y];
		}
	}

	return true;
}

bool AMDSClientDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientDataRequest::readFromDataStream";
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientDataRequest::readFromDataStream";

	QString readBufferName;
	bool readIncludeStatusData;
	bool readIncludeData;
	AMDSBufferGroupInfo readBufferGroupInfo;
	quint16 readDataCount;
	QList<AMDSDataHolder*> readDataHolder;
	QVector<double> totalVector;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readIncludeStatusData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readIncludeData;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	if(readIncludeData){
		dataStream->read(readBufferGroupInfo);
		*dataStream >> readDataCount;
		if(dataStream->status() != QDataStream::Ok)
			return false;
		// do some data reading here

		quint64 totalSize = readDataCount*readBufferGroupInfo.spanSize();
		totalVector = QVector<double>(totalSize);
		for(quint64 x = 0; x < totalSize; x++)
			*dataStream >> totalVector[x];
	}

	setBufferName(readBufferName);
	setIncludeStatusData(readIncludeStatusData);
	if(readIncludeData){
		setBufferGroupInfo(readBufferGroupInfo);
		// do some data setting here

		int totalCounter = 0;
		clearData();
		for(quint16 x = 0; x < readDataCount; x++){
			AMDSScalarDataHolder *oneScalarDataHolder = new AMDSScalarDataHolder();
			oneScalarDataHolder->setSingleValue(totalVector.at(totalCounter++));

			appendData(oneScalarDataHolder);
			double oneStoredValue;
			oneScalarDataHolder->data(&oneStoredValue);
			qDebug() << "One value was " << oneStoredValue;
		}
	}

	return true;
}
