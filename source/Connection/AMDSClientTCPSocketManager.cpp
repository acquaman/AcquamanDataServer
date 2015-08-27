#include "AMDSClientTCPSocketManager.h"

#include "source/util/AMDSErrorMonitor.h"

AMDSClientTCPSocketManager *AMDSClientTCPSocketManager::clientTCPSocketManager_ = 0;

AMDSClientTCPSocketManager::AMDSClientTCPSocketManager(QObject *parent) :
    QObject(parent)
{
}

AMDSClientTCPSocketManager::~AMDSClientTCPSocketManager()
{
	clientTCPSocketsBySocketKey_.clear();
	clientTCPSocketsByIdentifier_.clear();
}


void AMDSClientTCPSocketManager::appendSocket(AMDSClientTCPSocket *clientTCPSocket)
{
	clientTCPSocketsBySocketKey_.insert(clientTCPSocket->socketKey(), clientTCPSocket);
	AMDSErrorMon::information(this, 0, QString("active connections (after append): %1").arg(clientTCPSocketsBySocketKey_.size()));
}
void AMDSClientTCPSocketManager::removeSocket(QString socketKey)
{
	clientTCPSocketsBySocketKey_.remove(socketKey);
	AMDSErrorMon::information(this, 0, QString("active connections (after remove): %1").arg(clientTCPSocketsBySocketKey_.size()));
}

void AMDSClientTCPSocketManager::appendSocket(QString identifier, AMDSClientTCPSocket *clientTCPSocket)
{
	if (!clientTCPSocketsByIdentifier_.contains(identifier)) {
		clientTCPSocketsByIdentifier_.insert(identifier, clientTCPSocket);
	}
	AMDSErrorMon::information(this, 0, QString("active connections by hostName: %1").arg(clientTCPSocketsByIdentifier_.size()));
}
void AMDSClientTCPSocketManager::removeSocketByIdentifier(QString identifier)
{
	clientTCPSocketsByIdentifier_.remove(identifier);
	AMDSErrorMon::information(this, 0, QString("active connections by identifier: %1").arg(clientTCPSocketsByIdentifier_.size()));
}

