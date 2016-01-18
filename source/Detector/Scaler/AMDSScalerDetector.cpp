#include "AMDSScalerDetector.h"

#include <QTimer>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"
#include "DataHolder/AMDSScalarDataHolder.h"
#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

#include "util/AMDSRunTimeSupport.h"


AMDSScalerDetector::AMDSScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
	: AMDSDwellDetector(parent)
{
	scalerConfiguration_ = scalerConfiguration;

	dwellTime_ = 1;
	scansInABuffer_ = 100;
	totalNumberOfScans_ = 0;

	initializePVControls();
}

AMDSScalerDetector::~AMDSScalerDetector()
{
	configuredChannelControlSet_.clear();
	pvControlSet_->clear();
}

void AMDSScalerDetector::onStartDwelling()
{
	scanControl_->move(AMDSScalerDetector::Dwelling); // switch to dwelling mode
}

void AMDSScalerDetector::onStopDwelling()
{
	scanControl_->move(AMDSScalerDetector::Normal); // switch to normal mode
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
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
		return;
	}

	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_PV_CONNECTED, "PV controls are connected.");

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
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this,  AMDS_DWELL_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
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

	scanBufferControl_->setBinParameters(0, scansInABuffer_*enabledChannelCount());
}

void AMDSScalerDetector::onScanControlValueChanged(double newValue)
{
	Q_UNUSED(newValue)

	if (!scanControl_->withinTolerance(currentScanMode_)) {
		currentScanMode_ = scanControl_->withinTolerance(AMDSDwellDetector::Normal) ? AMDSDwellDetector::Normal : AMDSDwellDetector::Dwelling;
		emit detectorScanModeChanged(currentScanMode_);

		if (currentScanMode_ == AMDSDwellDetector::Dwelling) {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_SWITCH_TO_DWELLING, QString("Scaler %1 switched to Dwelling/Continuous scan mode.").arg(scalerName()));
		} else {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_SWITCH_TO_CONFIGURATION, QString("Scaler %1 switched to Normal scan mode.").arg(scalerName()));
		}
	}
}

void AMDSScalerDetector::onDwellTimeControlValueChanged(double newValue)
{
	Q_UNUSED(newValue)

	if (!dwellTimeControl_->withinTolerance(dwellTime_)) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_DWELL_TIME_CHANGED, QString("Scaler %1 dwell time changed from %2ms to %3ms.").arg(scalerName()).arg(dwellTime_).arg((int)dwellTimeControl_->value()));

		dwellTime_ = (int)dwellTimeControl_->value();
	}
}

void AMDSScalerDetector::onScansInABufferControlValueChanged(double newValue)
{
	Q_UNUSED(newValue)

	if (!scansInABufferControl_->withinTolerance(scansInABuffer_)) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_SCAN_IN_BUFFER_CHANGED, QString("Scaler %1 # scans in a buffer changed from %2 to %3.").arg(scalerName()).arg(scansInABuffer_).arg((int)scansInABufferControl_->value()));

		scansInABuffer_ = (int)scansInABufferControl_->value();

		scanBufferControl_->setBinParameters(0, scansInABuffer_*enabledChannelCount());
	}
}

void AMDSScalerDetector::onTotalNumberOfScansControlValueChanged(double newValue)
{
	Q_UNUSED(newValue)

	if (!totalNumberOfScansControl_->withinTolerance(totalNumberOfScans_)) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_TOTAL_NUM_SCANS, QString("Scaler %1 total # scans changed from %2 to %3.").arg(scalerName()).arg(totalNumberOfScans_).arg((int)totalNumberOfScansControl_->value()));

		totalNumberOfScans_ = (int)totalNumberOfScansControl_->value();
	}
}

void AMDSScalerDetector::onFetchScanBuffer()
{
	if (!connected_)
		return;

	AMDSDataHolderList scanBufferDataHolderList;

	int channelCount = enabledChannelCount();
	int bufferSize = channelCount * scansInABuffer_ + 1;
	QVector<int> countBuffer(bufferSize); // need one extra buffer for the # of channels
	scanBufferControl_->values(bufferSize, countBuffer.data());

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

	// initialize the physical PV controls for scaler
	QString physicalPVBaseName = scalerBasePVName()+":mcs";
	scanControl_ = new AMSinglePVControl(scalerName() + "_scanStatus", physicalPVBaseName+":startScan", this, 0.5);
	dwellTimeControl_ = new AMSinglePVControl(scalerName() + "_dwellTime", physicalPVBaseName+":delay", this, 0.5);
	scansInABufferControl_ = new AMSinglePVControl(scalerName() + "_numberScans", physicalPVBaseName+":nscan", this, 0.5);
	totalNumberOfScansControl_ = new AMSinglePVControl(scalerName() + "_totalNumberScans", physicalPVBaseName+":scanCount", this, 0.5);

	scanBufferControl_ = new AMWaveformBinningSinglePVControl(scalerName()+ "_scanBuffer", physicalPVBaseName+":scan", 0, scansInABuffer_*enabledChannelCount(), this);

	AMSinglePVControl *channelControl;
	foreach (quint8 channelId, scalerConfiguration_->configuredChannels()) {
		QString channelIdTemplate = channelId < 10 ? "0%1" : "%1"; // when channel id is 0 ~ 9, the PV is "0" + channelId
		QString scalerChannelEnablePVName = QString("%1%2:enable").arg(physicalPVBaseName).arg(channelIdTemplate.arg(channelId));
		channelControl = new AMSinglePVControl(scalerChannelEnablePVName, scalerChannelEnablePVName, this);

		configuredChannelControlSet_.insert(channelId, channelControl);
		pvControlSet_->addControl(channelControl);

		connect(channelControl, SIGNAL(valueChanged(double)), this, SLOT(onChannelControlValueChanged(double)));
	}

	pvControlSet_->addControl(scanControl_);
	pvControlSet_->addControl(dwellTimeControl_);
	pvControlSet_->addControl(scansInABufferControl_);
	pvControlSet_->addControl(totalNumberOfScansControl_);
	pvControlSet_->addControl(scanBufferControl_);

	// connect the signals/slots
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
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_FAILED_TO_CONFIG_CHANNEL, QString("Failed to move channel %1 for scaler %2, which might NOT be configured for AMDS.").arg(channelId).arg(scalerName()));
		}
	} else {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_FAILED_TO_CONFIG_CHANNEL, QString("Failed to move channel %1 for scaler %2, which might NOT be configured for AMDS.").arg(channelId).arg(scalerName()));
	}
}
