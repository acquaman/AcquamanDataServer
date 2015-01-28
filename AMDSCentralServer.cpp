#include "AMDSCentralServer.h"

#include <QTimer>

#include "AMDSThreadedTcpDataServer.h"
#include "AMDSThreadedBufferGroup.h"
#include "AMDSBufferGroup.h"
#include "AMDSClientDataRequest.h"

AMDSCentralServer::AMDSCentralServer(QObject *parent) :
	QObject(parent)
{
	dataServer_ = new AMDSThreadedTcpDataServer(this);

	QList<AMDSAxisInfo> mcpBufferGroupAxes;
	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", mcpBufferGroupAxes);
	AMDSBufferGroup *mcpBufferGroup = new AMDSBufferGroup(mcpBufferGroupInfo);
	AMDSThreadedBufferGroup *mcpThreadedBufferGroup = new AMDSThreadedBufferGroup(mcpBufferGroup);
	bufferGroups_.insert(mcpThreadedBufferGroup->bufferGroupInfo().name(), mcpThreadedBufferGroup);

	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", amptek1BufferGroupAxes);
	AMDSBufferGroup *amptek1BufferGroup = new AMDSBufferGroup(amptek1BufferGroupInfo);
	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroup);
	bufferGroups_.insert(amptek1ThreadedBufferGroup->bufferGroupInfo().name(), amptek1ThreadedBufferGroup);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV");
	AMDSBufferGroup *energyBufferGroup = new AMDSBufferGroup(energyBufferGroupInfo);
	AMDSThreadedBufferGroup *energyThreadedBufferGroup = new AMDSThreadedBufferGroup(energyBufferGroup);
	bufferGroups_.insert(energyThreadedBufferGroup->bufferGroupInfo().name(), energyThreadedBufferGroup);

//	connect(dataServer_, SIGNAL(requestBufferGroupInfo()), this, SLOT(onDataServerBufferGroupInfoRequested()));
	connect(dataServer_->server(), SIGNAL(requestData(AMDSClientDataRequest*)), this, SLOT(onDataServerDataRequested(AMDSClientDataRequest*)));
	connect(this, SIGNAL(dataRequestReady(AMDSClientDataRequest*)), dataServer_->server(), SLOT(onDataRequestReady(AMDSClientDataRequest*)));

//	QTimer::singleShot(1000, this, SLOT(onDataServerBufferGroupInfoRequested()));
}

//void AMDSCentralServer::onDataServerBufferGroupInfoRequested(){
//	QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroups_.constBegin();
//	while (i != bufferGroups_.constEnd()) {
//		AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
//		qDebug() << i.key() << " is " << oneInfo.name() << oneInfo.description() << oneInfo.units() << oneInfo.size().toString();
//		++i;
//	}
//}

void AMDSCentralServer::onDataServerDataRequested(AMDSClientDataRequest *dataRequest){
	if(dataRequest->requestType() == AMDSClientDataRequest::Introspection){
		QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroups_.constBegin();
		while (i != bufferGroups_.constEnd()) {
			AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
			qDebug() << i.key() << " is " << oneInfo.name() << oneInfo.description() << oneInfo.units() << oneInfo.size().toString();
			dataRequest->appendBufferGroupInfo(oneInfo);
			++i;
		}

		emit dataRequestReady(dataRequest);
	}
}
