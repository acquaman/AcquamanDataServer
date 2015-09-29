#include "AMDSCentralServerSample.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "source/Connection/AMDSThreadedTCPDataServer.h"
#include "source/DataElement/AMDSBufferGroup.h"
#include "source/DataElement/AMDSBufferGroupInfo.h"
#include "source/DataElement/AMDSBufferGroupManager.h"
#include "source/ClientRequest/AMDSClientRequest.h"
#include "source/DataHolder/AMDSScalarDataHolder.h"
#include "source/DataHolder/AMDSSpectralDataHolder.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServerSample::AMDSCentralServerSample(QObject *parent) :
	AMDSCentralServer(parent)
{
}


void AMDSCentralServerSample::onFiftyMillisecondTimerUpdate(){
	AMDSLightWeightScalarDataHolder *oneScalerDataHolder = new AMDSLightWeightScalarDataHolder();
	double valueAsDouble = (double)simpleCounter_++;
	oneScalerDataHolder->setSingleValue(valueAsDouble);

	bufferGroupManagers_.value("Energy")->append(oneScalerDataHolder);
}

void AMDSCentralServerSample::onHundredMillisecondTimerUpdate(){
	AMDSLightWeightSpectralDataHolder *oneSpectralDataHolder = new AMDSLightWeightSpectralDataHolder(AMDSDataTypeDefinitions::Signed64, bufferGroupManagers_.value("Amptek1")->bufferGroupInfo().size(0));

	AMDSFlatArray oneSpectralFlatArray(AMDSDataTypeDefinitions::Signed64, bufferGroupManagers_.value("Amptek1")->bufferGroupInfo().size(0));
	spectralCounter_++;
	qint64 oneValue;
	for(int x = 0; x < oneSpectralFlatArray.constVectorQint64().size(); x++){
		oneValue = (spectralCounter_+x)*2;
		oneSpectralFlatArray.vectorQint64()[x] = oneValue;
	}
	oneSpectralDataHolder->setData(&oneSpectralFlatArray);

	bufferGroupManagers_.value("Amptek1")->append(oneSpectralDataHolder);
}

void AMDSCentralServerSample::initializeBufferGroup(quint64 maxCountSize)
{
	QList<AMDSAxisInfo> mcpBufferGroupAxes;
	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", AMDSBufferGroupInfo::NoFlatten, mcpBufferGroupAxes);
	AMDSBufferGroupManager *mcpBufferGroupManager = new AMDSBufferGroupManager(mcpBufferGroupInfo, maxCountSize);
	bufferGroupManagers_.insert(mcpBufferGroupManager->bufferGroupName(), mcpBufferGroupManager);

	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", AMDSBufferGroupInfo::Summary, amptek1BufferGroupAxes);
	AMDSBufferGroupManager *amptek1BufferGroupName = new AMDSBufferGroupManager(amptek1BufferGroupInfo, maxCountSize);
	bufferGroupManagers_.insert(amptek1BufferGroupName->bufferGroupName(), amptek1BufferGroupName);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV", AMDSBufferGroupInfo::Average);
	AMDSBufferGroupManager *energyBufferGroupManager = new AMDSBufferGroupManager(energyBufferGroupInfo, maxCountSize);
	bufferGroupManagers_.insert(energyBufferGroupManager->bufferGroupName(), energyBufferGroupManager);

	foreach(AMDSBufferGroupManager *bufferGroupManager, bufferGroupManagers_) {
		connect(bufferGroupManager->bufferGroup(), SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_->server(), SLOT(onClientRequestProcessed(AMDSClientRequest*)));
	}
}

void AMDSCentralServerSample::initializeAndStartDataServer()
{
	fiftyMillisecondTimer_ = new QTimer(this);
	hundredMillisecondTimer_ = new QTimer(this);

	connect(fiftyMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onFiftyMillisecondTimerUpdate()));
	connect(hundredMillisecondTimer_, SIGNAL(timeout()), this, SLOT(onHundredMillisecondTimerUpdate()));

	AMErrorMon::information(this, 0, "Starting the timer to update data buffer ...");
	simpleCounter_ = 0;
	spectralCounter_ = 0;

	fiftyMillisecondTimer_->start(50);
	hundredMillisecondTimer_->start(100);
}
