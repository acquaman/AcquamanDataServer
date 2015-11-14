#include "AMDSScalerDetector.h"

#include <QDebug>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"

#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

#include "util/AMErrorMonitor.h"
#include "util/AMDSRunTimeSupport.h"

AMDSScalerDetector::AMDSScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
    : QObject(parent)
{
	scalerConfiguration_ = scalerConfiguration;

	connected_ = false;
	defaultDwellTime_ = 1; // default 1ms
	dwellTime_ = 1;
	defaultScansInABuffer_ = 1000;
	scansInABuffer_ = 1000;
	defaultTotalNumberOfScans_ = 0;
	totalNumberOfScans_ = 0;

	initializePVControls();
}

AMDSScalerDetector::~AMDSScalerDetector()
{
	channelStatusControlSet_->clear();
	pvControlSet_->clear();
}

void AMDSScalerDetector::onFetchScanBuffer()
{
	if (!connected_)
		return;

	AMDSDataHolderList scanBufferDataHolderList;

	int channelCount = enabledChannelCount();
	QVector<int> countBuffer(channelCount * scansInABuffer_ + 1); // need one extra buffer for the # of channels
	scanControl_->values(channelCount, countBuffer.data());

	// put the counts array to the FlatArrayList
	for (int scanIndex=0; scanIndex < scansInABuffer_; scanIndex++) {
		AMDSFlatArray channelCountArray(scalerConfiguration_->dataType(), channelCount);

		int channelScanStartIndex = scanIndex * channelCount + 1;
		channelCountArray.setValues(countBuffer.mid(channelScanStartIndex, channelCount));

		AMDSLightWeightScalarDataHolder *scalerDataHolder = new AMDSLightWeightScalarDataHolder(scalerConfiguration_->dataType());
		scalerDataHolder->setData(&channelCountArray);
		scanBufferDataHolderList.append(scalerDataHolder);
	}

	if (scanBufferDataHolderList.count() > 0) {
		emit newScalerScanDataReceived(scanBufferDataHolderList);
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

	dwellTimeControl_->move(defaultDwellTime_);
	scansInABufferControl_->move(defaultScansInABuffer_);
	totalNumberOfScansControl_->move(defaultTotalNumberOfScans_);

	continuousScanControl_->move(AMDSScalerDetector::Continuous); // switch to continuous mode
}

void AMDSScalerDetector::onAllControlsTimedOut()
{
	connected_ = false;
	AMErrorMon::alert(this, 0, "PV control is NOT connected.");
}

void AMDSScalerDetector::onDwellTimeControlValueChanged(double newValue)
{
	int newDwellControlValue = (int)newValue;
	if (dwellTime_ != newDwellControlValue) {
		AMErrorMon::alert(this, 0, QString("Scaler %1 dwell time changed from %2ms to %3ms.").arg(scalerName()).arg(dwellTime_).arg(newDwellControlValue));

		dwellTime_ = newDwellControlValue;
	}
}

void AMDSScalerDetector::onScansInABufferControlValueChanged(double newValue)
{
	if (scansInABuffer_ != newValue) {
		AMErrorMon::alert(this, 0, QString("Scaler %1 # scans in a buffer changed from %2 to %3.").arg(scalerName()).arg(scansInABuffer_).arg(newValue));

		scansInABuffer_ = (int)newValue;
	}
}

void AMDSScalerDetector::onContinuousScanControlValueChanged(double newValue)
{
	if ((newValue != AMDSScalerDetector::Continuous) && AMDSRunTimeSupport::debugAtLevel(1) ) {
		AMErrorMon::alert(this, 0, QString("Scaler %1 switched to Normal scan mode.").arg(scalerName()));
	}
}

void AMDSScalerDetector::initializePVControls()
{
	channelStatusControlSet_ = new AMControlSet(this);
	pvControlSet_ = new AMControlSet(this);

	statusControl_ = new AMSinglePVControl(scalerName() + "_scanStatus", scalerBasePVName()+":startScan", this);
	continuousScanControl_ = new AMSinglePVControl(scalerName()+ "_continuousScan", scalerBasePVName()+":continuous", this);
	dwellTimeControl_ = new AMSinglePVControl(scalerName() + "_dwellTime", scalerBasePVName()+":delay", this);
	scansInABufferControl_ = new AMSinglePVControl(scalerName() + "_numberScans", scalerBasePVName()+":nscan", this);
	totalNumberOfScansControl_ = new AMSinglePVControl(scalerName() + "_totalNumberScans", scalerBasePVName()+":scanCount", this);

	scanControl_ = new AMWaveformBinningSinglePVControl(scalerName()+ "_scanBuffer", scalerBasePVName()+":scan", 0, scansInABuffer_*enabledChannelCount(), this);

	AMSinglePVControl *channelStatusControl;
	foreach (quint8 channelId, enabledChannelIds()) {
		QString scalerChannelPVName = QString("%1%2:enable").arg(scalerBasePVName()).arg(channelId);
		channelStatusControl = new AMSinglePVControl(scalerChannelPVName, scalerChannelPVName, this);

		channelStatusControlSet_->addControl(channelStatusControl);
		pvControlSet_->addControl(channelStatusControl);
	}

	pvControlSet_->addControl(statusControl_);
	pvControlSet_->addControl(continuousScanControl_);
	pvControlSet_->addControl(dwellTimeControl_);
	pvControlSet_->addControl(scansInABufferControl_);
	pvControlSet_->addControl(totalNumberOfScansControl_);
	pvControlSet_->addControl(scanControl_);

	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlValueChanged(double)));
	connect(scansInABufferControl_, SIGNAL(valueChanged(double)), this, SLOT(onScansInABufferControlValueChanged(double)));
	connect(continuousScanControl_, SIGNAL(valueChanged(double)), this, SLOT(onContinuousScanControlValueChanged(double)));
	connect(scanControl_, SIGNAL(valueChanged(double)), this, SLOT(onFetchScanBuffer()));

	connect(pvControlSet_, SIGNAL(controlSetValuesChanged()), this, SLOT(onStartScalerScanTimer()));
	connect(pvControlSet_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	connect(pvControlSet_, SIGNAL(controlSetTimedOut()), this, SLOT(onAllControlsTimedOut()));
}
