#include "AMDSScalerDetector.h"

#include <QTimer>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "DataHolder/AMDSScalarDataHolder.h"
#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"

#include "Detector/Scaler/AMDSScalerConfigurationMap.h"

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
	defaultScansInABuffer_ = 100;
	scansInABuffer_ = 100;
	defaultTotalNumberOfScans_ = 0;
	totalNumberOfScans_ = 0;

	channelValueChangedSignalMapper_ = new QSignalMapper(this);

	triggerDwellTimer_ = new QTimer(this);
	connect(triggerDwellTimer_, SIGNAL(timeout()), this, SLOT(onTriggerDwellTimerTimeout()));

	initializePVControls();
}

AMDSScalerDetector::~AMDSScalerDetector()
{
	configuredChannelControlSet_.clear();
	pvControlSet_->clear();

	if (triggerDwellTimer_->isActive()) {
		triggerDwellTimer_->stop();
	}
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

void AMDSScalerDetector::setFlattenedData(const QVector<double> &data)
{
	QMap<quint8, int> channelToIndexMap;
	for(int x = 0, size = scalerConfiguration_->configuredChannels().count(); x < size; x++)
		channelToIndexMap.insert(scalerConfiguration_->configuredChannels().at(x), x);


	for(int x = 0, size = triggerDwellInterfaceChannelFeedbackControls_.count(); x < size; x++){
		QString controlPVName = triggerDwellInterfaceChannelFeedbackControls_.value(x)->readPVName();

		QMap<quint8, int>::const_iterator i = channelToIndexMap.constBegin();
		while(i != channelToIndexMap.constEnd()){
			QString channelIdTemplate = i.key() < 10 ? QString("0%1").arg(i.key()) : QString("%1").arg(i.key()); // when channel id is 0 ~ 9, the PV is "0" + channelId
			if(controlPVName.remove("BL1611-ID-1:AMDS:scaler").contains(channelIdTemplate))
				triggerDwellInterfaceChannelFeedbackControls_.value(x)->move(data.at(i.value()));

			i++;
		}
	}
}

void AMDSScalerDetector::onAllControlsConnected(bool connected)
{
	if(connected_ != connected)
		connected_ = connected;

	if (!connected_) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
		return;
	}

	// to initialize the list of enabled/disabled channel ids
	if (!initialized_) {
		onScanControlValueChanged(scanControl_->value());
		onChannelControlValueChanged(0);
		onDwellTimeControlValueChanged(dwellTimeControl_->value());
		onScansInABufferControlValueChanged(scansInABufferControl_->value());
		onTotalNumberOfScansControlValueChanged(totalNumberOfScansControl_->value());

		triggerDwellInterfaceStartControl_->move(0.0);
		triggerDwellInterfaceDwellStateControl_->move(0.0);
		triggerDwellInterfaceDwellModeControl_->move(0.0);


		QMap<int, AMSinglePVControl*>::const_iterator i = triggerDwellInterfaceChannelEnableControls_.constBegin();
		while(i != triggerDwellInterfaceChannelEnableControls_.constEnd()){
			if(scalerConfiguration_->configuredChannels().contains(i.key()))
				i.value()->move(1.0);
			i++;
		}

		initialized_ = true;
	}
}

void AMDSScalerDetector::onAllControlsTimedOut()
{
	connected_ = false;
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this,  AMDS_SCALER_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
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
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_SWITCH_TO_DWELLING, QString("Scaler %1 switched to Dwelling/Continuous scan mode.").arg(scalerName()));
		} else {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_SWITCH_TO_CONFIGURATION, QString("Scaler %1 switched to Normal scan mode.").arg(scalerName()));
		}
	}
}

void AMDSScalerDetector::onDwellTimeControlValueChanged(double newValue)
{
	int newDwellControlValue = (int)newValue;
	if (dwellTime_ != newDwellControlValue) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_DWELL_TIME_CHANGED, QString("Scaler %1 dwell time changed from %2ms to %3ms.").arg(scalerName()).arg(dwellTime_).arg(newDwellControlValue));

		dwellTime_ = newDwellControlValue;
	}
}

void AMDSScalerDetector::onScansInABufferControlValueChanged(double newValue)
{
	if (scansInABuffer_ != newValue) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_SCAN_IN_BUFFER_CHANGED, QString("Scaler %1 # scans in a buffer changed from %2 to %3.").arg(scalerName()).arg(scansInABuffer_).arg(newValue));

		scansInABuffer_ = (int)newValue;
	}
}

void AMDSScalerDetector::onTotalNumberOfScansControlValueChanged(double newValue)
{
	if (totalNumberOfScans_ != newValue) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_TOTAL_NUM_SCANS, QString("Scaler %1 total # scans changed from %2 to %3.").arg(scalerName()).arg(totalNumberOfScans_).arg(newValue));

		totalNumberOfScans_ = (int)newValue;
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

void AMDSScalerDetector::onTriggerDwellInterfaceDwellModeControlValueChanged(double value)
{
	Q_UNUSED(value)

	if(triggerDwellInterfaceDwellModeControl_->withinTolerance(1.0) && triggerDwellInterfaceStartControl_->withinTolerance(1.0)) {
		stopScalerDwellAcquisition(true);
	}
}

void AMDSScalerDetector::onTriggerDwellInterfaceStartControlValueChanged(double value)
{
	Q_UNUSED(value)

	if(connected_){
		if(triggerDwellInterfaceStartControl_->withinTolerance(1.0)){
			startScalerDwellAcquisition();
		}
		else if(triggerDwellInterfaceStartControl_->withinTolerance(0.0) && triggerDwellInterfaceDwellModeControl_->withinTolerance(1.0) && triggerDwellTimer_->isActive()){
			stopScalerDwellAcquisition(true);
		}
	}
}

void AMDSScalerDetector::onTriggerDwellInterfaceDwellTimeControlValueChanged(double value)
{
	Q_UNUSED(value)

	if(connected_){
		// Do nothing right now
	}
}

void AMDSScalerDetector::onTriggerDwellTimerTimeout()
{
	if(triggerDwellInterfaceDwellModeControl_->withinTolerance(0.0)){
		triggerDwellTimer_->stop();
	}
	emit requestFlattenedData(triggerDwellInterfaceDwellTimeControl_->value());
}

void AMDSScalerDetector::startScalerDwellAcquisition()
{
	double seconds = triggerDwellInterfaceDwellTimeControl_->value();
	if(seconds > 0 && seconds < 100) {
		if (triggerDwellInterfaceDwellModeControl_->withinTolerance(0.0)) {
			// set the mapping of value changed. NOTE: This should be in a more graceful way
			waitingChannels_.clear();

			QMap<quint8, int> channelToIndexMap;
			for(int x = 0, size = scalerConfiguration_->configuredChannels().count(); x < size; x++)
				channelToIndexMap.insert(scalerConfiguration_->configuredChannels().at(x), x);


			for(int x = 0, size = triggerDwellInterfaceChannelFeedbackControls_.count(); x < size; x++){
				QString controlPVName = triggerDwellInterfaceChannelFeedbackControls_.value(x)->readPVName();

				QMap<quint8, int>::const_iterator i = channelToIndexMap.constBegin();
				while(i != channelToIndexMap.constEnd()){
					QString channelIdTemplate = i.key() < 10 ? QString("0%1").arg(i.key()) : QString("%1").arg(i.key()); // when channel id is 0 ~ 9, the PV is "0" + channelId
					if(controlPVName.remove("BL1611-ID-1:AMDS:scaler").contains(channelIdTemplate)) {
						waitingChannels_.append(x);
						channelValueChangedSignalMapper_->setMapping(triggerDwellInterfaceChannelFeedbackControls_.value(x), x);
					}

					i++;
				}
			}
			connect(channelValueChangedSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onScalerChannelValueChanged(int)));
		}

		// start the time
		int asMsecs = (int)(seconds*1000);
		triggerDwellTimer_->setInterval(asMsecs);
		triggerDwellInterfaceDwellStateControl_->move(1.0);
		triggerDwellTimer_->start();
	}
}

void AMDSScalerDetector::stopScalerDwellAcquisition(bool forceStop)
{
	if (forceStop) {
		if (triggerDwellTimer_->isActive())
			triggerDwellTimer_->stop();

		foreach(int channelIndex, waitingChannels_) {
			waitingChannels_.removeAll(channelIndex);
			channelValueChangedSignalMapper_->removeMappings(triggerDwellInterfaceChannelFeedbackControls_.value(channelIndex));
		}
	}

	if(waitingChannels_.count() == 0) {
		disconnect(channelValueChangedSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onScalerChannelValueChanged(int)));

		triggerDwellInterfaceStartControl_->move(0);
		triggerDwellInterfaceDwellStateControl_->move(0);
	}
}

void AMDSScalerDetector::onScalerChannelValueChanged(int channelIndex)
{
	if(waitingChannels_.contains(channelIndex)){

		waitingChannels_.removeAll(channelIndex);
		channelValueChangedSignalMapper_->removeMappings(triggerDwellInterfaceChannelFeedbackControls_.value(channelIndex));
	}

	stopScalerDwellAcquisition();
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
		QString channelIdTemplate = channelId < 10 ? "0%1" : "%1"; // when channel id is 0 ~ 9, the PV is "0" + channelId
		QString scalerChannelPVName = QString("%1%2:enable").arg(scalerBasePVName()).arg(channelIdTemplate.arg(channelId));
		channelControl = new AMSinglePVControl(scalerChannelPVName, scalerChannelPVName, this);

		configuredChannelControlSet_.insert(channelId, channelControl);
		pvControlSet_->addControl(channelControl);

		connect(channelControl, SIGNAL(valueChanged(double)), this, SLOT(onChannelControlValueChanged(double)));
	}

	QString triggerDwellName = "BL1611-ID-1:AMDS:scaler";
	triggerDwellInterfaceStartControl_ = new AMSinglePVControl("ScalerStart", QString("%1:start").arg(triggerDwellName), this, 0.5);
	triggerDwellInterfaceDwellTimeControl_ = new AMSinglePVControl("ScalerDwellTime", QString("%1:dwellTime").arg(triggerDwellName), this, 0.05);
	triggerDwellInterfaceDwellStateControl_ = new AMSinglePVControl("ScalerDwellState", QString("%1:dwellState").arg(triggerDwellName), this, 0.5);
	triggerDwellInterfaceDwellModeControl_ = new AMSinglePVControl("ScalerDwellMode", QString("%1:dwellMode").arg(triggerDwellName), this, 0.5);

	pvControlSet_->addControl(triggerDwellInterfaceStartControl_);
	pvControlSet_->addControl(triggerDwellInterfaceDwellTimeControl_);
	pvControlSet_->addControl(triggerDwellInterfaceDwellStateControl_);
	pvControlSet_->addControl(triggerDwellInterfaceDwellModeControl_);

	AMSinglePVControl *channelFeedbackControl;
	AMSinglePVControl *channelEnableControl;
	for(int x = 0, size = 32; x < size; x++){
		QString channelIdTemplate = x < 10 ? "0%1" : "%1"; // when channel id is 0 ~ 9, the PV is "0" + channelId

		QString scalerChannelFeedbackName = QString("%1:%2:fbk").arg(triggerDwellName).arg(channelIdTemplate.arg(x));
		channelFeedbackControl = new AMSinglePVControl(scalerChannelFeedbackName, scalerChannelFeedbackName, this, 0.5);
		QString scalerChannelEnableName = QString("%1:%2:enable").arg(triggerDwellName).arg(channelIdTemplate.arg(x));
		channelEnableControl = new AMSinglePVControl(scalerChannelEnableName, scalerChannelEnableName, this, 0.5);

		triggerDwellInterfaceChannelFeedbackControls_.insert(x, channelFeedbackControl);
		triggerDwellInterfaceChannelEnableControls_.insert(x, channelEnableControl);

		pvControlSet_->addControl(channelFeedbackControl);
		pvControlSet_->addControl(channelEnableControl);

		connect(channelFeedbackControl, SIGNAL(valueChanged(double)), channelValueChangedSignalMapper_, SLOT(map()));
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

	connect(triggerDwellInterfaceDwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerDwellInterfaceDwellModeControlValueChanged(double)));
	connect(triggerDwellInterfaceStartControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerDwellInterfaceStartControlValueChanged(double)));
	connect(triggerDwellInterfaceDwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onTriggerDwellInterfaceDwellTimeControlValueChanged(double)));
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
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_SCALER_DETECTOR_ALT_FAILED_TO_CONFIG_CHANNEL, QString("Failed to move channel %1 for scaler %2, which might NOT be configured for AMDS.").arg(channelId).arg(scalerName()));
		}
	}
}
