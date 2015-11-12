#include "AMDSClientStatisticsRequest.h"


AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(QObject *parent) :
	AMDSClientRequest(parent)
{
	setRequestType(AMDSClientRequestDefinitions::Statistics);
}

AMDSClientStatisticsRequest::AMDSClientStatisticsRequest(AMDSClientRequest::ResponseType responseType, const QDateTime &localDateTime, const QString socketKey, QObject *parent) :
	AMDSClientRequest(socketKey, QString(), AMDSClientRequestDefinitions::Statistics, responseType, localDateTime, parent)
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

int AMDSClientStatisticsRequest::writeToDataStream(QDataStream *dataStream)
{
	int errorCode = AMDSClientRequest::writeToDataStream(dataStream);
	if( errorCode != AMDS_CLIENTREQUEST_SUCCESS)
		return errorCode;

	*dataStream << (quint32)packetStats_.count();
	if(dataStream->status() != QDataStream::Ok)
		return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS_COUNT;

	for(int x = 0, size = packetStats().count(); x < size; x++) {
		AMDSPacketStats newPacketStats = packetStats().at(x);
		newPacketStats.writeToDataStream(dataStream);
	}

	return AMDS_CLIENTREQUEST_SUCCESS;
}

int AMDSClientStatisticsRequest::readFromDataStream(QDataStream *dataStream)
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
		onePacketStat.readFromDataStream(dataStream);
		if(onePacketStat.name() == "Invalid")
			return AMDS_CLIENTREQUEST_FAIL_TO_HANDLE_PACKET_STATS;
		readPacketStats.append(onePacketStat);
	}

	packetStats_.append(readPacketStats);
	return AMDS_CLIENTREQUEST_SUCCESS;
}

QString AMDSClientStatisticsRequest::toString() const
{
	QString messageData = QString("Data of Statistics message (%1):").arg(socketKey());
	for(int x = 0, size = packetStats().count(); x < size; x++) {
		messageData = QString("%1\n\tPacket Stats %2").arg(messageData).arg(packetStats().at(x).toString());
	}

	return messageData;
}
