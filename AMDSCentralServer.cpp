#include "AMDSCentralServer.h"

#include <QTimer>

#include "AMDSThreadedTcpDataServer.h"
#include "AMDSThreadedBufferGroup.h"
#include "AMDSBufferGroup.h"
#include "AMDSBufferGroupInfo.h"
#include "AMDSClientDataRequest.h"

AMDSCentralServer::AMDSCentralServer(QObject *parent) :
	QObject(parent)
{
	dataServer_ = new AMDSThreadedTcpDataServer(this);

	quint64 maxCountSize = 1000*60*60*10; // 10 hours of 1kHz signal

	QList<AMDSAxisInfo> mcpBufferGroupAxes;
	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", mcpBufferGroupAxes);
	AMDSBufferGroup *mcpBufferGroup = new AMDSBufferGroup(mcpBufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *mcpThreadedBufferGroup = new AMDSThreadedBufferGroup(mcpBufferGroup);
	bufferGroups_.insert(mcpThreadedBufferGroup->bufferGroupInfo().name(), mcpThreadedBufferGroup);

	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", amptek1BufferGroupAxes);
	AMDSBufferGroup *amptek1BufferGroup = new AMDSBufferGroup(amptek1BufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroup);
	bufferGroups_.insert(amptek1ThreadedBufferGroup->bufferGroupInfo().name(), amptek1ThreadedBufferGroup);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV");
//	AMDSBufferGroup *energyBufferGroup = new AMDSBufferGroup(energyBufferGroupInfo, maxCountSize);
	energyBufferGroup_ = new AMDSBufferGroup(energyBufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *energyThreadedBufferGroup = new AMDSThreadedBufferGroup(energyBufferGroup_);
	bufferGroups_.insert(energyThreadedBufferGroup->bufferGroupInfo().name(), energyThreadedBufferGroup);

	simpleCounter_ = 0;
	fiftyMillisecondTimer_ = new QTimer(this);

	connect(dataServer_->server(), SIGNAL(requestData(AMDSClientDataRequest*)), this, SLOT(onDataServerDataRequested(AMDSClientDataRequest*)));
	connect(this, SIGNAL(dataRequestReady(AMDSClientDataRequest*)), dataServer_->server(), SLOT(onDataRequestReady(AMDSClientDataRequest*)));

	connect(fiftyMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onFiftyMillisecondTimerUpdate()));
	fiftyMillisecondTimer_->start(50);
}

void AMDSCentralServer::onDataServerDataRequested(AMDSClientDataRequest *dataRequest){
	if(dataRequest->requestType() == AMDSClientDataRequest::Introspection){

		if(dataRequest->bufferName() == "All"){
			QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroups_.constBegin();
			while (i != bufferGroups_.constEnd()) {
				AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
				qDebug() << i.key() << " is " << oneInfo.name() << oneInfo.description() << oneInfo.units() << oneInfo.size().toString();
				dataRequest->appendBufferGroupInfo(oneInfo);
				++i;
			}
		}
		else if(bufferGroups_.contains(dataRequest->bufferName()))
			dataRequest->appendBufferGroupInfo(bufferGroups_.value(dataRequest->bufferName())->bufferGroupInfo());
		else
			dataRequest->setErrorMessage(QString("No buffer named %1, cannot introspect").arg(dataRequest->bufferName()));

		emit dataRequestReady(dataRequest);
	}
	else if(dataRequest->requestType() == AMDSClientDataRequest::StartTimePlusCount){
		qDebug() << "Got the start time plus count request on " << dataRequest->bufferName();
		if(dataRequest->bufferName() == "Energy"){
			connect(energyBufferGroup_, SIGNAL(dataRequestReady(AMDSClientDataRequest*)), dataServer_->server(), SLOT(onDataRequestReady(AMDSClientDataRequest*)));
			energyBufferGroup_->requestData(dataRequest);
		}
	}
}

#include "AMDSScalarDataHolder.h"
void AMDSCentralServer::onFiftyMillisecondTimerUpdate(){
	AMDSScalarDataHolder *oneScalerDataHolder = new AMDSScalarDataHolder();
	oneScalerDataHolder->setSingleValue(simpleCounter_++);

	energyBufferGroup_->append(oneScalerDataHolder);
}
