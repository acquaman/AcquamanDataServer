#include "AMDSScalerDetector.h"

#include <QDebug>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"

#include "util/AMErrorMonitor.h"

AMDSScalerDetector::AMDSScalerDetector(const QString &scalerName, const QString &basePVName, const QList<quint8> &enabledChannelIdList, QObject *parent)
    : QObject(parent)
{
	scalerName_ = scalerName;
	basePVName_ = basePVName;

	enabledChannelIdList_ = enabledChannelIdList;

	connected_ = false;
	dwellTime_ = 1; // default 1ms
	scansInABuffer_ = 1000;
	totalNumberOfScans_ = 0;

	initializePVControls();
}

AMDSScalerDetector::~AMDSScalerDetector()
{

}

void AMDSScalerDetector::onFetchScanBuffer()
{
	QList<AMDSFlatArray> scanBufferArray;

	int channelCount = enabledChannelCount();
	QVector<int> countBuffer(channelCount);

	scanControl_->values(channelCount, countBuffer.data());
	qDebug() << "AMDSScalerDetector::onFetchScanBuffer: readed buffer --- " << countBuffer;

	// put the counts array to the FlatArrayList
	for (int scanIndex=0; scanIndex < scansInABuffer_; scanIndex++) {
		AMDSFlatArray channelCountArray(AMDSDataTypeDefinitions::Unsigned16, channelCount);
		for (int channelIndex=0; channelIndex < channelCount; channelIndex++) {
			channelCountArray.setValue(channelIndex, countBuffer.value(scanIndex*channelCount + channelIndex));
		}

		scanBufferArray.append(channelCountArray);
	}

	if (scanBufferArray.count() > 0) {
		emit newScanDataReceived(scanBufferArray);
	}
}

void AMDSScalerDetector::onAllControlsConnected(bool connected)
{
	if(connected_ != connected)
		connected_ = connected;

	if (!connected_) {
		AMErrorMon::alert(this, 0, "PV control is NOT connected.");
		return;
	}

	dwellTimeControl_->move(dwellTime_);
	scansInABufferControl_->move(scansInABuffer_);
	totalNumberOfScansControl_->move(totalNumberOfScans_);

	continuousScansControl_->move(1); // switch to continuous mode
}

void AMDSScalerDetector::onAllControlsTimedOut()
{
	connected_ = false;
	AMErrorMon::alert(this, 0, "PV control is NOT connected.");
}

void AMDSScalerDetector::initializePVControls()
{
	pvControlSet_ = new AMControlSet(this);

	statusControl_ = new AMSinglePVControl(scalerName_ + "_scanStatus", basePVName_+":startScan", this);
	continuousScansControl_ = new AMSinglePVControl(scalerName_ + "_continuousScan", basePVName_+":continuous", this);
	dwellTimeControl_ = new AMSinglePVControl(scalerName_ + "_dwellTime", basePVName_+":delay", this);
	scansInABufferControl_ = new AMSinglePVControl(scalerName_ + "_numberScans", basePVName_+":nscan", this);
	totalNumberOfScansControl_ = new AMSinglePVControl(scalerName_ + "_totalNumberScans", basePVName_+":scanCount", this);

	scanControl_ = new AMWaveformBinningSinglePVControl(scalerName_ + "_scanBuffer", basePVName_+":scan", 0, scansInABuffer_*enabledChannelCount(), this);

	AMSinglePVControl *channelStatusControl;
	foreach (quint8 channelId, enabledChannelIdList_) {
		channelStatusControl = new AMSinglePVControl(scalerName_+"Channel_"+channelId, basePVName_+channelId+":Enable", this);

		channelStatusControlList_->append(channelStatusControl);
		pvControlSet_->addControl(channelStatusControl);
	}

	pvControlSet_->addControl(statusControl_);
	pvControlSet_->addControl(continuousScansControl_);
	pvControlSet_->addControl(dwellTimeControl_);
	pvControlSet_->addControl(scansInABufferControl_);
	pvControlSet_->addControl(totalNumberOfScansControl_);
	pvControlSet_->addControl(scanControl_);

	connect(pvControlSet_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	connect(pvControlSet_, SIGNAL(controlSetTimedOut()), this, SLOT(onAllControlsTimedOut()));

}
