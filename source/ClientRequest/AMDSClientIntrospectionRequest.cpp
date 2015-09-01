#include "AMDSClientIntrospectionRequest.h"

#include "source/Connection/AMDSDataStream.h"
#include "source/util/AMDSErrorMonitor.h"

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Introspection);

	setBufferName("Invalid");
}

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(ResponseType responseType, const QString &socketKey, const QString &bufferName, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequestDefinitions::Introspection, responseType, parent)
{
	setBufferName(bufferName);
}

AMDSClientIntrospectionRequest::~AMDSClientIntrospectionRequest()
{
}

AMDSClientIntrospectionRequest::AMDSClientIntrospectionRequest(const AMDSClientIntrospectionRequest &other) :
	AMDSClientRequest(other)
{
	(*this) = other;
}

AMDSClientIntrospectionRequest& AMDSClientIntrospectionRequest::operator =(const AMDSClientIntrospectionRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		setBufferName(other.bufferName());

		clearBufferGroupInfos();
		for(int x = 0, size = other.bufferGroupInfos().count(); x < size; x++)
			appendBufferGroupInfo(other.bufferGroupInfos().at(x));
	}
	return (*this);
}

QStringList AMDSClientIntrospectionRequest::getAllBufferNames()
{
	QStringList bufferNames;
	for(int x = 0, xSize = bufferGroupInfos_.count(); x < xSize; x++) {
		AMDSBufferGroupInfo bufferGroupInfo = bufferGroupInfos_.at(x);

		bufferNames.append(bufferGroupInfo.name());
	}

	return bufferNames;
}

int AMDSClientIntrospectionRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;

	quint16 bufferGroupInfosCount = bufferGroupInfos_.count();
	*dataStream << bufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO_COUNT;

	for(int x = 0, size = bufferGroupInfos_.count(); x < size; x++)
		dataStream->write(bufferGroupInfos_.at(x));

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientIntrospectionRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	QString readBufferName;
	quint16 readBufferGroupInfosCount;
	QList<AMDSBufferGroupInfo> readBufferGroupInfos;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_NAME;
	*dataStream >> readBufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO_COUNT;

	for(int x = 0, size = readBufferGroupInfosCount; x < size; x++){
		AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
		dataStream->read(oneBufferGroupInfo);
		if(oneBufferGroupInfo.name() == "Invalid")
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_BUFFER_GROUP_INFO;
		readBufferGroupInfos.append(oneBufferGroupInfo);
	}

	setBufferName(readBufferName);
	bufferGroupInfos_.append(readBufferGroupInfos);

	return AMDS_CLIENTREQUEST_SUCCESS;
}

bool AMDSClientIntrospectionRequest::validateResponse()
{
	for(int y = 0, ySize = bufferGroupInfos_.count(); y < ySize; y++){
		AMDSBufferGroupInfo bufferGroupInfo = bufferGroupInfos_.at(y);
		AMDSErrorMon::information(this, AMDS_CLIENTREQUEST_INFO_REQUEST_DATA, QString("%1 %2 %3 %4 %5 %6 %7").arg(bufferGroupInfo.name()).arg(bufferGroupInfo.description()).arg(bufferGroupInfo.units()).arg(bufferGroupInfo.rank()).arg(bufferGroupInfo.size().toString()).arg(bufferGroupInfo.flattenEnabled()?"true":"false").arg(bufferGroupInfo.flattenMethod()));
		for(int x = 0, size = bufferGroupInfo.axes().count(); x < size; x++){
			AMDSAxisInfo axisInfo = bufferGroupInfo.axes().at(x);
			AMDSErrorMon::information(this, AMDS_CLIENTREQUEST_INFO_REQUEST_DATA, QString("\tAxis info at %1 %2 %3 %4 %5 %6 %7 %8").arg(x).arg(axisInfo.name()).arg(axisInfo.description()).arg(axisInfo.units()).arg(axisInfo.size()).arg(axisInfo.isUniform()).arg(axisInfo.start()).arg(axisInfo.increment()));
		}
	}

	return true;
}
