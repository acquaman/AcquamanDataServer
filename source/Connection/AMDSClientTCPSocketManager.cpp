#include "AMDSClientTCPSocketManager.h"

AMDSClientTCPSocketManager *AMDSClientTCPSocketManager::clientTCPSocketManager = 0;
AMDSClientTCPSocketManager::AMDSClientTCPSocketManager(QObject *parent) :
    QObject(parent)
{
}

void AMDSClientTCPSocketManager::onTCPSocketConnect(quint8 requestTypeId, QString bufferName)
{

}

