#include "AMDSCentralServer.h"

#include "AMDSThreadedTcpDataServer.h"
#include "AMDSThreadedBufferGroup.h"
#include "AMDSBufferGroup.h"

AMDSCentralServer::AMDSCentralServer(QObject *parent) :
	QObject(parent)
{
	dataServer_ = new AMDSThreadedTcpDataServer(this);

	connect(dataServer_, SIGNAL(requestBufferGroupInfo()), this, SLOT(onDataServerBufferGroupInfoRequested()));
}

void AMDSCentralServer::onDataServerBufferGroupInfoRequested(){

}
