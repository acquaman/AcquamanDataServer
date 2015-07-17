#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"

//#include <QtGui>

#include "source/AMDSDataStream.h"

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
	AMDSClientRequest(0)
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

bool AMDSClientIntrospectionRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;

	*dataStream << bufferName_;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	quint16 bufferGroupInfosCount = bufferGroupInfos_.count();
	*dataStream << bufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = bufferGroupInfos_.count(); x < size; x++)
		dataStream->write(bufferGroupInfos_.at(x));

	return true;
}

bool AMDSClientIntrospectionRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;

	QString readBufferName;
	quint16 readBufferGroupInfosCount;
	QList<AMDSBufferGroupInfo> readBufferGroupInfos;

	*dataStream >> readBufferName;
	if(dataStream->status() != QDataStream::Ok)
		return false;
	*dataStream >> readBufferGroupInfosCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = readBufferGroupInfosCount; x < size; x++){
		AMDSBufferGroupInfo oneBufferGroupInfo("Invalid");
		dataStream->read(oneBufferGroupInfo);
		if(oneBufferGroupInfo.name() == "Invalid")
			return false;
		readBufferGroupInfos.append(oneBufferGroupInfo);
	}

	setBufferName(readBufferName);
	bufferGroupInfos_.append(readBufferGroupInfos);

	return true;
}

void AMDSClientIntrospectionRequest::printData()
{
	/// TODO: to be added ...
//	QList<AMDSBufferGroupInfo> requestBufferGroupInfos = bufferGroupInfos();

//	if(requestBufferGroupInfos.count() > 0){
//		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(requestType->model());
//		for(int x = 1; x < 7; x++){
//			QStandardItem* item = model->item(x);
//			item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
//			// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
//			item->setData(QVariant(), Qt::TextColorRole);
//		}
//	}

//	if(bufferName() == "All"){
//		bufferNameComboBox_->clear();
//		bufferNameComboBox_->addItem("All");
//	}
//	for(int y = 0, ySize = requestBufferGroupInfos.count(); y < ySize; y++){
//		if(bufferName() == "All")
//			bufferNameComboBox_->addItem(requestBufferGroupInfos.at(y).name());
//		qDebug() << requestBufferGroupInfos.at(y).name() << requestBufferGroupInfos.at(y).description() << requestBufferGroupInfos.at(y).units() << requestBufferGroupInfos.at(y).rank() << requestBufferGroupInfos.at(y).size().toString();
//		for(int x = 0, size = requestBufferGroupInfos.at(y).axes().count(); x < size; x++){
//			qDebug() << "\tAxis info at " << x << requestBufferGroupInfos.at(y).axes().at(x).name() << requestBufferGroupInfos.at(y).axes().at(x).description() << requestBufferGroupInfos.at(y).axes().at(x).units() << requestBufferGroupInfos.at(y).axes().at(x).size() << requestBufferGroupInfos.at(y).axes().at(x).isUniform() << requestBufferGroupInfos.at(y).axes().at(x).start() << requestBufferGroupInfos.at(y).axes().at(x).increment();
//		}
//	}

}
