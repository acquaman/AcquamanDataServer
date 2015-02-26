#include "AMDSClientStatisticsRequest.h"

#include <QDebug>

#include "AMDSDataStream.h"

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	requestType_ = AMDSClientRequestDefinitions::Statistics;
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(AMDSClientRequest::ResponseType responseType, const QString socketKey, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequestDefinitions::Statistics, responseType, parent)
{

}

AMDSClientStatisticsRequest::~AMDSClientStatisticsRequest()
{
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(const AMDSClientStatisticsRequest &other) :
	AMDSClientRequest(0)
{
	(*this) = other;
}

AMDSClientStatisticsRequest& AMDSClientStatisticsRequest::operator =(const AMDSClientStatisticsRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		clearPacketStats();
		for(int x = 0, size = other.packetStats().count(); x < size; x++)
			appendPacketStats(other.packetStats().at(x));
	}
	return (*this);
}

bool AMDSClientStatisticsRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	qDebug() << "Calling AMDSClientStatisticsRequest::writeToDataStream";
	if(!AMDSClientRequest::writeToDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStatisticsRequest::writeToDataStream";

	*dataStream << (quint32)packetStats_.count();
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = packetStats().count(); x < size; x++)
		dataStream->write(packetStats().at(x));

	return true;
}

bool AMDSClientStatisticsRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	qDebug() << "Calling AMDSClientStatisticsRequest::readFromDataStream";
	if(!AMDSClientRequest::readFromDataStream(dataStream))
		return false;
	qDebug() << "Continuing on with AMDSClientStatisticsRequest::readFromDataStream";

	quint32 readPacketStatsCount;
	QList<AMDSPacketStats> readPacketStats;

	*dataStream >> readPacketStatsCount;
	if(dataStream->status() != QDataStream::Ok)
		return false;

	for(int x = 0, size = readPacketStatsCount; x < size; x++){
		AMDSPacketStats onePacketStat("Invalid");
		dataStream->read(onePacketStat);
		if(onePacketStat.name() == "Invalid")
			return false;
		readPacketStats.append(onePacketStat);
	}

	for(int x = 0, size = readPacketStats.count(); x < size; x++)
		appendPacketStats(readPacketStats.at(x));

	return true;
}
