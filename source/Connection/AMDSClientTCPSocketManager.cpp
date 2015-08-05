#include "AMDSClientTCPSocketManager.h"

#include "source/util/AMDSErrorMonitor.h"

AMDSClientTCPSocketManager *AMDSClientTCPSocketManager::clientTCPSocketManager_ = 0;

AMDSClientTCPSocketManager::AMDSClientTCPSocketManager(QObject *parent) :
    QObject(parent)
{
}

void AMDSClientTCPSocketManager::appendSocket(AMDSClientTCPSocket *clientTCPSocket)
{
	clientTCPSocketsBySocketKey_.insert(clientTCPSocket->socketKey(), clientTCPSocket);
	AMDSErrorMon::information(this, 0, QString("active connections: %1").arg(clientTCPSocketsBySocketKey_.size()));
}
void AMDSClientTCPSocketManager::removeSocket(QString socketKey)
{
	clientTCPSocketsBySocketKey_.remove(socketKey);
	AMDSErrorMon::information(this, 0, QString("active connections: %1").arg(clientTCPSocketsBySocketKey_.size()));
}

