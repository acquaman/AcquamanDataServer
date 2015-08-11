#include "AMDSClientContinuousWithBatchStreamsDataRequest.h"

#include "source/AMDSDataStream.h"

AMDSClientContinuousWithBatchStreamsDataRequest::AMDSClientContinuousWithBatchStreamsDataRequest(QObject *parent) :
	AMDSClientContinuousDataRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::ContinuousWithBatchStreams);

	setBufferNames(QStringList());
}

AMDSClientContinuousWithBatchStreamsDataRequest::AMDSClientContinuousWithBatchStreamsDataRequest(ResponseType responseType, const QString &socketKey, const QStringList &bufferNamesList, bool includeStatusData, const quint64 msgUpdateInterval, const QString &msgHandShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent) :
	AMDSClientContinuousDataRequest(responseType, socketKey, bufferNamesList, includeStatusData, msgUpdateInterval, msgHandShakeSocketKey, bufferGroupInfo, parent)
{
	setRequestType(AMDSClientRequestDefinitions::ContinuousWithBatchStreams);

	setBufferNames(bufferNamesList);
}

AMDSClientContinuousWithBatchStreamsDataRequest::~AMDSClientContinuousWithBatchStreamsDataRequest()
{
}

AMDSClientContinuousWithBatchStreamsDataRequest::AMDSClientContinuousWithBatchStreamsDataRequest(const AMDSClientContinuousWithBatchStreamsDataRequest &other) :
	AMDSClientContinuousDataRequest(0)
{
	(*this) = other;
}

AMDSClientContinuousWithBatchStreamsDataRequest& AMDSClientContinuousWithBatchStreamsDataRequest::operator =(const AMDSClientContinuousWithBatchStreamsDataRequest &other)
{
	if(this != &other){
		AMDSClientContinuousDataRequest::operator =(other);

		setBufferNames(other.bufferNames());
	}

	return (*this);
}

void AMDSClientContinuousWithBatchStreamsDataRequest::setBufferNames(const QStringList &bufferNames)
{
	bufferNamesList.clear();
	bufferNamesList.append(bufferNames);
}

bool AMDSClientContinuousWithBatchStreamsDataRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientContinuousDataRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << bufferNames();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	return true;
}

bool AMDSClientContinuousWithBatchStreamsDataRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientContinuousDataRequest::readFromDataStream(dataStream))
		return false;

	QStringList readBufferNames;
	*dataStream >> readBufferNames;

	setBufferNames(readBufferNames);

	return true;
}

