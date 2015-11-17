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

/// ==================== implementation of AMDSScalerDetectorManager ============================
AMDSScalerDetectorManager::AMDSScalerDetectorManager(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
	: QObject(parent)
{
	detectorManagerThread_ = new QThread();

	scalerDetector_ = new AMDSScalerDetector(scalerConfiguration);
	scalerDetector_->moveToThread(detectorManagerThread_);

	connect(detectorManagerThread_, SIGNAL(finished()), scalerDetector_, SLOT(deleteLater()));

	detectorManagerThread_->start();
}

AMDSScalerDetectorManager::~AMDSScalerDetectorManager()
{
	if (detectorManagerThread_->isRunning())
		detectorManagerThread_->quit();

	detectorManagerThread_->deleteLater();
	detectorManagerThread_ = 0;
}

/// ==================== implementation of AMDSScalerDetector ============================
AMDSScalerDetector::AMDSScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
	: AMDSDwellDetector(parent)
{
	scalerConfiguration_ = scalerConfiguration;

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
	configuredChannelControlSet_.clear();
	pvControlSet_->clear();
}

void AMDSScalerDetector::onServerGoingToStartDwelling()
{
	scanControl_->move(AMDSScalerDetector::Dwelling); // switch to dwelling mode
}

void AMDSScalerDetector::onServerStopDwelling()
{
	scanControl_->move(AMDSScalerDetector::Configuration); // switch to normal mode
}

void AMDSScalerDetector::onEnableChannel(int channelId)
{
	configureChannelControl(true, channelId);
}

void AMDSScalerDetector::onDisableChannel(int channelId)
{
	configureChannelControl(false, channelId);
}


void AMDSScalerDetector::onAllControlsConnected(bool connected)
{
	if(connected_ != connected)
		connected_ = connected;

	if (!connected_) {
		AMErrorMon::alert(this, 0, "PV control is NOT connected.");
		return;
	}

	// to initialize the list of enabled/disabled channel ids
	if (!initialized_) {
		onScanControlValueChanged(scanControl_->value());
		onChannelControlValueChanged(0);
		onDwellTimeControlValueChanged(dwellTimeControl_->value());
		onScansInABufferControlValueChanged(scansInABufferControl_->value());
		onTotalNumberOfScansControlValueChanged(totalNumberOfScansControl_->value());

		initialized_ = true;
	}
}

void AMDSScalerDetector::onAllControlsTimedOut()
{
	connected_ = false;
	AMErrorMon::alert(this, 0, "PV control is NOT connected.");
}

void AMDSScalerDetector::onChannelControlValueChanged(double value)
{
	Q_UNUSED(value)

	foreach (int channelId, scalerConfiguration_->configuredChannels()) {
		AMSinglePVControl *channelControl = configuredChannelControlSet_.value(channelId);
		if (channelControl->withinTolerance(AMDSScalerDetector::Enabled)) {
			scalerConfiguration_->enableChannel(channelId);
		} else {
			scalerConfiguration_->disableChannel(channelId);
		}
	}
}

void AMDSScalerDetector::onScanControlValueChanged(double newValue)
{
	if ((int)newValue != currentScanMode_) {
		currentScanMode_ = (int)newValue == 0 ? AMDSDwellDetector::Configuration : AMDSDwellDetector::Dwelling;
		emit detectorScanModeChanged(currentScanMode_);

		if (currentScanMode_ == AMDSDwellDetector::Dwelling) {
			AMErrorMon::alert(this, 0, QString("Scaler %1 switched to Dwelling/Continuous scan mode.").arg(scalerName()));
		} else {
			AMErrorMon::alert(this, 0, QString("Scaler %1 switched to Normal scan mode.").arg(scalerName()));
		}
	}
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

void AMDSScalerDetector::onTotalNumberOfScansControlValueChanged(double newValue)
{
	if (totalNumberOfScans_ != newValue) {
		AMErrorMon::alert(this, 0, QString("Scaler %1 total # scans changed from %2 to %3.").arg(scalerName()).arg(totalNumberOfScans_).arg(newValue));

		totalNumberOfScans_ = (int)newValue;
	}
}

void AMDSScalerDetector::onFetchScanBuffer()
{
	if (!connected_)
		return;

	AMDSDataHolderList scanBufferDataHolderList;

	int channelCount = enabledChannelCount();
	QVector<int> countBuffer(channelCount * scansInABuffer_ + 1); // need one extra buffer for the # of channels
	scanBufferControl_->values(channelCount, countBuffer.data());

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

void AMDSScalerDetector::initializePVControls()
{
	configuredChannelControlSet_.clear();
	pvControlSet_ = new AMControlSet(this);

	scanControl_ = new AMSinglePVControl(scalerName() + "_scanStatus", scalerBasePVName()+":startScan", this);
	dwellTimeControl_ = new AMSinglePVControl(scalerName() + "_dwellTime", scalerBasePVName()+":delay", this);
	scansInABufferControl_ = new AMSinglePVControl(scalerName() + "_numberScans", scalerBasePVName()+":nscan", this);
	totalNumberOfScansControl_ = new AMSinglePVControl(scalerName() + "_totalNumberScans", scalerBasePVName()+":scanCount", this);

	scanBufferControl_ = new AMWaveformBinningSinglePVControl(scalerName()+ "_scanBuffer", scalerBasePVName()+":scan", 0, scansInABuffer_*enabledChannelCount(), this);

	AMSinglePVControl *channelControl;
	foreach (quint8 channelId, scalerConfiguration_->configuredChannels()) {
		QString channelIdTemplate = channelId < 10 ? "0%1" : "%1"; // when channel id is 1 ~ 9, the PV is "0" + channelId
		QString scalerChannelPVName = QString("%1%2:enable").arg(scalerBasePVName()).arg(channelIdTemplate.arg(channelId));
		channelControl = new AMSinglePVControl(scalerChannelPVName, scalerChannelPVName, this);

		configuredChannelControlSet_.insert(channelId, channelControl);
		pvControlSet_->addControl(channelControl);

		connect(channelControl, SIGNAL(valueChanged(double)), this, SLOT(onChannelControlValueChanged(double)));
	}

	pvControlSet_->addControl(scanControl_);
	pvControlSet_->addControl(dwellTimeControl_);
	pvControlSet_->addControl(scansInABufferControl_);
	pvControlSet_->addControl(totalNumberOfScansControl_);
	pvControlSet_->addControl(scanBufferControl_);

	connect(pvControlSet_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	connect(pvControlSet_, SIGNAL(controlSetTimedOut()), this, SLOT(onAllControlsTimedOut()));


	connect(scanControl_, SIGNAL(valueChanged(double)), this, SLOT(onScanControlValueChanged(double)));
	connect(dwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellTimeControlValueChanged(double)));
	connect(scansInABufferControl_, SIGNAL(valueChanged(double)), this, SLOT(onScansInABufferControlValueChanged(double)));
	connect(totalNumberOfScansControl_, SIGNAL(valueChanged(double)), this, SLOT(onTotalNumberOfScansControlValueChanged(double)));
	connect(scanBufferControl_, SIGNAL(valueChanged(double)), this, SLOT(onFetchScanBuffer()));
}

void AMDSScalerDetector::configureChannelControl(bool enable, int channelId)
{
	bool applyConfiguration = false;
	int targetValue = 0;
	if (enable) { // enable a channel
		applyConfiguration = scalerConfiguration_->disabledChannels().contains(channelId);
		targetValue = AMDSScalerDetector::Enabled;
	} else { // disable a channel
		applyConfiguration = scalerConfiguration_->enabledChannels().contains(channelId);
		targetValue = AMDSScalerDetector::Disabled;
	}

	if (applyConfiguration) {
		AMSinglePVControl *channelPVControl = configuredChannelControlSet_.value(channelId);
		if (channelPVControl) {
			channelPVControl->move(targetValue);
		} else {
			AMErrorMon::alert(this, 0, QString("Failed to move channel %1 for scaler %2, which might NOT be configured for AMDS.").arg(channelId).arg(scalerName()));
		}
	}
}
