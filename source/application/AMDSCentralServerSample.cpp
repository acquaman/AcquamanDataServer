#include "AMDSCentralServerSample.h"

#include <QtCore/QCoreApplication>
#include <QTimer>

#include "Connection/AMDSThreadedTCPDataServer.h"
#include "DataElement/AMDSBufferGroup.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataElement/AMDSThreadedBufferGroup.h"
#include "ClientRequest/AMDSClientRequest.h"
#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataHolder/AMDSSpectralDataHolder.h"
#include "util/AMErrorMonitor.h"

AMDSCentralServerSample::AMDSCentralServerSample(QObject *parent) :
	AMDSCentralServer(parent)
{
}

AMDSCentralServerSample::~AMDSCentralServerSample()
{
	if (fiftyMillisecondTimer_->isActive())
		fiftyMillisecondTimer_->stop();
	if (hundredMillisecondTimer_->isActive())
		hundredMillisecondTimer_->stop();

	fiftyMillisecondTimer_->deleteLater();
	hundredMillisecondTimer_->deleteLater();
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
		oneSpectralFlatArray.setValue(x, oneValue);
	}
	oneSpectralDataHolder->setData(&oneSpectralFlatArray);

	bufferGroupManagers_.value("Amptek1")->append(oneSpectralDataHolder);
}

void AMDSCentralServerSample::initializeBufferGroup()
{
	QList<AMDSAxisInfo> mcpBufferGroupAxes;
	mcpBufferGroupAxes << AMDSAxisInfo("X", 1024, "X Axis", "pixel");
	mcpBufferGroupAxes << AMDSAxisInfo("Y", 512, "Y Axis", "pixel");
	AMDSBufferGroupInfo mcpBufferGroupInfo("AFakeMCP", "Fake MCP Image", "Counts", AMDSDataTypeDefinitions::Double, AMDSBufferGroupInfo::NoFlatten, mcpBufferGroupAxes);
	AMDSThreadedBufferGroup *mcpThreadedBufferGroup = new AMDSThreadedBufferGroup(mcpBufferGroupInfo, maxBufferSize_);
	bufferGroupManagers_.insert(mcpThreadedBufferGroup->bufferGroupName(), mcpThreadedBufferGroup);

	QList<AMDSAxisInfo> amptek1BufferGroupAxes;
	amptek1BufferGroupAxes << AMDSAxisInfo("Energy", 1024, "Energy Axis", "eV");
	AMDSBufferGroupInfo amptek1BufferGroupInfo("Amptek1", "Amptek 1", "Counts", AMDSDataTypeDefinitions::Double, AMDSBufferGroupInfo::Summary, amptek1BufferGroupAxes);
	AMDSThreadedBufferGroup *amptek1ThreadedBufferGroup = new AMDSThreadedBufferGroup(amptek1BufferGroupInfo, maxBufferSize_);
	bufferGroupManagers_.insert(amptek1ThreadedBufferGroup->bufferGroupName(), amptek1ThreadedBufferGroup);

	AMDSBufferGroupInfo energyBufferGroupInfo("Energy", "SGM Beamline Energy", "eV", AMDSDataTypeDefinitions::Double, AMDSBufferGroupInfo::Average);
	AMDSThreadedBufferGroup *energyThreadedBufferGroup= new AMDSThreadedBufferGroup(energyBufferGroupInfo, maxBufferSize_);
	bufferGroupManagers_.insert(energyThreadedBufferGroup->bufferGroupName(), energyThreadedBufferGroup);

	foreach(AMDSThreadedBufferGroup *bufferGroupManager, bufferGroupManagers_) {
		connect(bufferGroupManager, SIGNAL(clientRequestProcessed(AMDSClientRequest*)), tcpDataServer_, SLOT(onClientRequestProcessed(AMDSClientRequest*)));
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

void AMDSCentralServerSample::wrappingUpInitialization()
{

}
