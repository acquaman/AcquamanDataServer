#include "source/ClientRequest/AMDSClientStatisticsRequest.h"

#include <QDebug>

#include "source/AMDSDataStream.h"

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Statistics);
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(AMDSClientRequest::ResponseType responseType, const QString socketKey, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequestDefinitions::Statistics, responseType, parent)
{

}

AMDSClientStatisticsRequest::~AMDSClientStatisticsRequest()
{
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(const AMDSClientStatisticsRequest &other) :
	AMDSClientRequest(other)
{
	(*this) = other;
}

AMDSClientStatisticsRequest& AMDSClientStatisticsRequest::operator =(const AMDSClientStatisticsRequest &other)
{
	if(this != &other){
		AMDSClientRequest::operator =(other);

		clearPacketStats();
		packetStats_.append(other.packetStats());
	}
	return (*this);
}

int AMDSClientStatisticsRequest::writeToDataStream(AMDSDataStream *dataStream) const
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << (quint32)packetStats_.count();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS_COUNT;

	for(int x = 0, size = packetStats().count(); x < size; x++)
		dataStream->write(packetStats().at(x));

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientStatisticsRequest::readFromDataStream(AMDSDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::readFromDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	quint32 readPacketStatsCount;
	QList<AMDSPacketStats> readPacketStats;

	*dataStream >> readPacketStatsCount;
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS_COUNT;

	for(int x = 0, size = readPacketStatsCount; x < size; x++){
		AMDSPacketStats onePacketStat("Invalid");
		dataStream->read(onePacketStat);
		if(onePacketStat.name() == "Invalid")
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS;
		readPacketStats.append(onePacketStat);
	}

	packetStats_.append(readPacketStats);
	return AMDS_CLIENTREQUEST_SUCCESS;
}

bool AMDSClientStatisticsRequest::validateResponse()
{
	for(int x = 0, size = packetStats().count(); x < size; x++)
		qDebug() << "Packet Stats " << packetStats().at(x).name() << ": " << packetStats().at(x).allStats();

	return true;
}
