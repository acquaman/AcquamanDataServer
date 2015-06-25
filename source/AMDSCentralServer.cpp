#include "source/AMDSCentralServer.h"

#include <QTimer>

#include "source/AMDSThreadedTcpDataServer.h"
#include "source/AMDSThreadedBufferGroup.h"
#include "source/AMDSBufferGroup.h"
#include "source/AMDSBufferGroupInfo.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientDataRequest.h"

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
	amptek1BufferGroup_ = new AMDSBufferGroup(amptek1BufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroup_);
	bufferGroups_.insert(amptek1ThreadedBufferGroup->bufferGroupInfo().name(), amptek1ThreadedBufferGroup);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV");
	energyBufferGroup_ = new AMDSBufferGroup(energyBufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *energyThreadedBufferGroup = new AMDSThreadedBufferGroup(energyBufferGroup_);
	bufferGroups_.insert(energyThreadedBufferGroup->bufferGroupInfo().name(), energyThreadedBufferGroup);

	simpleCounter_ = 0;
	fiftyMillisecondTimer_ = new QTimer(this);

	spectralCounter_ = 0;
	hundredMillisecondTimer_ = new QTimer(this);

	connect(dataServer_->server(), SIGNAL(clientRequestRead(AMDSClientRequest*)), this, SLOT(onDataServerClientRequestReady(AMDSClientRequest*)));
	connect(this, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

	connect(fiftyMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onFiftyMillisecondTimerUpdate()));
	fiftyMillisecondTimer_->start(50);

	connect(hundredMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondTimerUpdate()));
	hundredMillisecondTimer_->start(100);
}

void AMDSCentralServer::onDataServerClientRequestReady(AMDSClientRequest *clientRequest){
	if(clientRequest->requestType() == AMDSClientRequestDefinitions::Introspection){
		AMDSClientIntrospectionRequest *clientIntrospectionRequest = qobject_cast<AMDSClientIntrospectionRequest*>(clientRequest);
		if(clientIntrospectionRequest){
			if(clientIntrospectionRequest->bufferName() == "All"){
				QMap<QString, AMDSThreadedBufferGroup*>::const_iterator i = bufferGroups_.constBegin();
				while (i != bufferGroups_.constEnd()) {
					AMDSBufferGroupInfo oneInfo = i.value()->bufferGroupInfo();
					qDebug() << i.key() << " is " << oneInfo.name() << oneInfo.description() << oneInfo.units() << oneInfo.size().toString();
					clientIntrospectionRequest->appendBufferGroupInfo(oneInfo);
					++i;
				}
			}
			else if(bufferGroups_.contains(clientIntrospectionRequest->bufferName()))
				clientIntrospectionRequest->appendBufferGroupInfo(bufferGroups_.value(clientIntrospectionRequest->bufferName())->bufferGroupInfo());
			else
				clientIntrospectionRequest->setErrorMessage(QString("No buffer named %1, cannot introspect").arg(clientIntrospectionRequest->bufferName()));

			emit clientRequestProcessed(clientIntrospectionRequest);
		}
	}
	else{
		AMDSClientDataRequest *clientDataRequest = qobject_cast<AMDSClientDataRequest*>(clientRequest);
		if(clientDataRequest){
			if(clientDataRequest->bufferName() == "Energy"){
				clientDataRequest->setBufferGroupInfo(energyBufferGroup_->bufferGroupInfo());
				connect(energyBufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
				energyBufferGroup_->processClientRequest(clientRequest);
			}
			if(clientDataRequest->bufferName() == "Amptek1"){
				clientDataRequest->setBufferGroupInfo(amptek1BufferGroup_->bufferGroupInfo());
				connect(amptek1BufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
				amptek1BufferGroup_->processClientRequest(clientRequest);
			}
		}
	}
}

#include "source/DataHolder/AMDSScalarDataHolder.h"
void AMDSCentralServer::onFiftyMillisecondTimerUpdate(){
	AMDSLightWeightScalarDataHolder *oneScalerDataHolder = new AMDSLightWeightScalarDataHolder();
	double valueAsDouble = (double)simpleCounter_++;
	oneScalerDataHolder->setSingleValue(valueAsDouble);

	energyBufferGroup_->append(oneScalerDataHolder);
}

#include "source/DataHolder/AMDSSpectralDataHolder.h"
void AMDSCentralServer::onHundredMillisecondTimerUpdate(){
	AMDSLightWeightSpectralDataHolder *oneSpectralDataHolder = new AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::Signed64, amptek1BufferGroup_->bufferGroupInfo().size(0));

	AMDSFlatArray oneSpectralFlatArray(AMDSDataTypeDefinitions::Signed64, amptek1BufferGroup_->bufferGroupInfo().size(0));
	spectralCounter_++;
	qint64 oneValue;
	for(int x = 0; x < oneSpectralFlatArray.constVectorQint64().size(); x++){
		oneValue = (spectralCounter_+x)*2;
		oneSpectralFlatArray.vectorQint64()[x] = oneValue;
	}
	oneSpectralDataHolder->setData(&oneSpectralFlatArray);

	amptek1BufferGroup_->append(oneSpectralDataHolder);
}