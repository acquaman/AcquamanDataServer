#include "AMDSDataStream.h"

AMDSDataStream::AMDSDataStream() :
	QDataStream()
{
}

AMDSDataStream::AMDSDataStream(QIODevice *device) :
	QDataStream(device)
{
}

AMDSDataStream::AMDSDataStream(QByteArray *array, QIODevice::OpenMode openMode) :
	QDataStream(array, openMode)
{
}

AMDSDataStream::AMDSDataStream(const QByteArray &array) :
	QDataStream(array)
{
}

AMDSDataStream::~AMDSDataStream()
{
}

void AMDSDataStream::read(AMDSAxisInfo &axisInfo){
	QString name;
	QString description;
	QString units;
	quint32 size;
	bool isUniform;
	qint16 start;
	quint16 increment;

	qDebug() << "Reading an axisInfo";
	*this >> name;
	if(status() != QDataStream::Ok)
		return;
	qDebug() << "Named: " << name;
	*this >> description;
	if(status() != QDataStream::Ok)
		return;
	*this >> units;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(size);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(isUniform);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(start);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(increment);
	if(status() != QDataStream::Ok)
		return;

	axisInfo.setName(name);
	axisInfo.setDescription(description);
	axisInfo.setUnits(units);
	axisInfo.setSize(size);
	axisInfo.setIsUniform(isUniform);
	axisInfo.setStart(start);
	axisInfo.setIncrement(increment);
}

void AMDSDataStream::read(AMDSBufferGroupInfo &bufferGroupInfo){
	QString name;
	QString description;
	QString units;
	quint8 axesCount;
	QList<AMDSAxisInfo> axes;

	qDebug() << "Reading a bufferGroupInfo";
	*this >> name;
	if(status() != QDataStream::Ok)
		return;
	qDebug() << "Named: " << name;
	*this >> description;
	if(status() != QDataStream::Ok)
		return;
	*this >> units;
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(axesCount);
	if(status() != QDataStream::Ok)
		return;

	for(int x = 0, size = axesCount; x < size; x++){
		AMDSAxisInfo oneAxisInfo("Invalid", 0);
		read(oneAxisInfo);
		if(oneAxisInfo.name() == "Invalid")
			return;
		axes.append(oneAxisInfo);
	}

	bufferGroupInfo.setName(name);
	bufferGroupInfo.setDescription(description);
	bufferGroupInfo.setUnits(units);
	bufferGroupInfo.setAxes(axes);
}

void AMDSDataStream::read(AMDSClientDataRequest &clientDataRequest){
	quint8 requestType;
	quint8 responseType;

	QDataStream::operator >>(requestType);
	if(status() != QDataStream::Ok)
		return;
	QDataStream::operator >>(responseType);
	if(status() != QDataStream::Ok)
		return;

	switch(requestType){
	case AMDSClientDataRequest::Continuous:
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		break;
	case AMDSClientDataRequest::Introspection:{
		quint16 bufferGroupCount;
		QList<AMDSBufferGroupInfo> bufferGroupInfos;
		QDataStream::operator >>(bufferGroupCount);
		if(status() != QDataStream::Ok)
			return;
		for(int x = 0, size = bufferGroupCount; x < size; x++){
			AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
			read(oneBufferGroupInfo);
			if(oneBufferGroupInfo.name() == "Invalid")
				return;
			bufferGroupInfos.append(oneBufferGroupInfo);
		}

		clientDataRequest.setRequestType((AMDSClientDataRequest::RequestType)requestType);
		clientDataRequest.setResponseType((AMDSClientDataRequest::ResponseType)responseType);

		for(int x = 0, size = bufferGroupInfos.count(); x < size; x++)
			clientDataRequest.appendBufferGroupInfo(bufferGroupInfos.at(x));

		break;}
	default:
		break;
	}
}

void AMDSDataStream::write(const AMDSAxisInfo &axisInfo){
	qDebug() << "Writing an axisInfo named " << axisInfo.name();
	*this << axisInfo.name();
	*this << axisInfo.description();
	*this << axisInfo.units();
	QDataStream::operator <<(axisInfo.size());
	QDataStream::operator <<(axisInfo.isUniform());
	QDataStream::operator <<(axisInfo.start());
	QDataStream::operator <<(axisInfo.increment());
}

void AMDSDataStream::write(const AMDSBufferGroupInfo &bufferGroupInfo){
	qDebug() << "Writing a bufferGroupInfo named " << bufferGroupInfo.name();
	*this << bufferGroupInfo.name();
	*this << bufferGroupInfo.description();
	*this << bufferGroupInfo.units();
	QDataStream::operator <<((quint8)(bufferGroupInfo.axes().count()));
	qDebug() << "This bufferGroupInfo has " << (quint8)(bufferGroupInfo.axes().count()) << " axes";
	for(int x = 0, size = bufferGroupInfo.axes().count(); x < size; x++){
		qDebug() << "Attempting axis " << x << " as " << bufferGroupInfo.axes().at(x).name();
		write(bufferGroupInfo.axes().at(x));
	}
}

void AMDSDataStream::write(const AMDSClientDataRequest &clientDataRequest){
	QDataStream::operator <<((quint8)(clientDataRequest.requestType()));
	QDataStream::operator <<((quint8)(clientDataRequest.responseType()));

	switch(clientDataRequest.requestType()){
	case AMDSClientDataRequest::Continuous:
		break;
	case AMDSClientDataRequest::StartTimePlusCount:
		break;
	case AMDSClientDataRequest::RelativeCountPlusCount:
		break;
	case AMDSClientDataRequest::StartTimeToEndTime:
		break;
	case AMDSClientDataRequest::MiddleTimePlusCountBeforeAndAfter:
		break;
	case AMDSClientDataRequest::Introspection:
		QDataStream::operator <<((quint16)(clientDataRequest.bufferGroupInfos().count()));
		for(int x = 0, size = clientDataRequest.bufferGroupInfos().count(); x < size; x++)
			write(clientDataRequest.bufferGroupInfos().at(x));
		break;
	default:
		break;
	}
}
