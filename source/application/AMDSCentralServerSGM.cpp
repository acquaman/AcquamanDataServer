#include "AMDSCentralServerSGM.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "source/AMDSThreadedTCPDataServer.h"
#include "source/AMDSThreadedBufferGroup.h"
#include "source/AMDSBufferGroup.h"
#include "source/AMDSBufferGroupInfo.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/ClientRequest/AMDSClientIntrospectionRequest.h"
#include "source/ClientRequest/AMDSClientDataRequest.h"
#include "source/ClientRequest/AMDSClientContinuousDataRequest.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServerSGM::AMDSCentralServerSGM(QObject *parent) :
	AMDSCentralServer(parent)
{
	fiftyMillisecondTimer_ = new QTimer(this);
	hundredMillisecondTimer_ = new QTimer(this);

	initializeBufferGroup(1000*60*60*10); // 10 hours of 1kHz signal

	connect(fiftyMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onFiftyMillisecondTimerUpdate()));
	connect(hundredMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondTimerUpdate()));

	startTimer();
}

#include "source/DataHolder/AMDSScalarDataHolder.h"
void AMDSCentralServerSGM::onFiftyMillisecondTimerUpdate(){
	AMDSLightWeightScalarDataHolder *oneScalerDataHolder = new AMDSLightWeightScalarDataHolder();
	double valueAsDouble = (double)simpleCounter_++;
	oneScalerDataHolder->setSingleValue(valueAsDouble);

	energyBufferGroup_->append(oneScalerDataHolder);
}

#include "source/DataHolder/AMDSSpectralDataHolder.h"
void AMDSCentralServerSGM::onHundredMillisecondTimerUpdate(){
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

void AMDSCentralServerSGM::initializeBufferGroup(quint64 maxCountSize)
{
	QList<AMDSAxisInfo> mcpBufferGroupAxes;
	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", false, AMDSBufferGroupInfo::NoFlatten, mcpBufferGroupAxes);
	AMDSBufferGroup *mcpBufferGroup = new AMDSBufferGroup(mcpBufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *mcpThreadedBufferGroup = new AMDSThreadedBufferGroup(mcpBufferGroup);
	bufferGroups_.insert(mcpThreadedBufferGroup->bufferGroupInfo().name(), mcpThreadedBufferGroup);

	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", false, AMDSBufferGroupInfo::NoFlatten, amptek1BufferGroupAxes);
	amptek1BufferGroup_ = new AMDSBufferGroup(amptek1BufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroup_);
	bufferGroups_.insert(amptek1ThreadedBufferGroup->bufferGroupInfo().name(), amptek1ThreadedBufferGroup);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV", true, AMDSBufferGroupInfo::Average);
	energyBufferGroup_ = new AMDSBufferGroup(energyBufferGroupInfo, maxCountSize);
	AMDSThreadedBufferGroup *energyThreadedBufferGroup = new AMDSThreadedBufferGroup(energyBufferGroup_);
	bufferGroups_.insert(energyThreadedBufferGroup->bufferGroupInfo().name(), energyThreadedBufferGroup);

	connect(mcpBufferGroup, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
	connect(amptek1BufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
	connect(energyBufferGroup_, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), dataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));

}

void AMDSCentralServerSGM::startTimer()
{
	AMErrorMon::information(this, 0, "Starting the timer to update data buffer ...");
	simpleCounter_ = 0;
	spectralCounter_ = 0;

	fiftyMillisecondTimer_->start(50);
	hundredMillisecondTimer_->start(100);
}
