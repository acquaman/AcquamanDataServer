#include "AMDSDwellScalerDetector.h"

#include <QTimer>

#include "beamline/AMPVControl.h"
#include "beamline/AMControlSet.h"

#include "Detector/Scaler/AMDSScalerConfigurationMap.h"
#include "util/AMDSRunTimeSupport.h"

/// ==================== implementation of AMDSDwellScalerDetector ============================
AMDSDwellScalerDetector::AMDSDwellScalerDetector(AMDSScalerConfigurationMap *scalerConfiguration, QObject *parent)
	: AMDSDwellDetector(parent)
{
	dwellTimeInSecond_ = 1;

	scalerConfiguration_ = scalerConfiguration;

	channelValueChangedSignalMapper_ = new QSignalMapper(this);

	initializePVControls();

	triggerDwellTimer_ = new QTimer(this);
	connect(triggerDwellTimer_, SIGNAL(timeout()), this, SLOT(onTriggerDwellTimerTimeout()));
}

AMDSDwellScalerDetector::~AMDSDwellScalerDetector()
{
	pvControlSet_->clear();
	dwellScalerChannelFeedbackControls_.clear();
	dwellScalerChannelEnableControls_.clear();

	if (triggerDwellTimer_->isActive()) {
		triggerDwellTimer_->stop();
	}
}

void AMDSDwellScalerDetector::setFlattenedData(const QVector<double> &data)
{
	QMap<quint8, int> channelToIndexMap;
	for(int x = 0, size = scalerConfiguration_->configuredChannels().count(); x < size; x++)
		channelToIndexMap.insert(scalerConfiguration_->configuredChannels().at(x), x);


	for(int x = 0, size = dwellScalerChannelFeedbackControls_.count(); x < size; x++){
		QString controlPVName = dwellScalerChannelFeedbackControls_.value(x)->readPVName();

		QMap<quint8, int>::const_iterator i = channelToIndexMap.constBegin();
		while(i != channelToIndexMap.constEnd()){
			QString channelIdTemplate = i.key() < 10 ? QString("0%1").arg(i.key()) : QString("%1").arg(i.key()); // when channel id is 0 ~ 9, the PV is "0" + channelId
			if(controlPVName.remove("BL1611-ID-1:AMDS:scaler").contains(channelIdTemplate))
				dwellScalerChannelFeedbackControls_.value(x)->move(data.at(i.value()));

			i++;
		}
	}
}

void AMDSDwellScalerDetector::onAllControlsConnected(bool connected)
{
	if(connected_ == connected)
		connected_ = connected;

	if (!connected_) {
		AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
		return;
	}

	// to initialize the list of enabled/disabled channel ids
	if (!initialized_) {
		dwellScalerStartControl_->move(0.0);
		dwellScalerDwellStateControl_->move(0.0);
		dwellScalerDwellModeControl_->move(0.0);

		foreach(int channelId, dwellScalerChannelEnableControls_.keys()) {
			if (scalerConfiguration_->enabledChannels().contains(channelId)) {
				dwellScalerChannelEnableControls_.value(channelId)->move(1.0);
			}
		}

		onDwellScalerDwellTimeControlValueChanged(dwellScalerDwellTimeControl_->value());

		initialized_ = true;
	}
}

void AMDSDwellScalerDetector::onAllControlsTimedOut()
{
	connected_ = false;
	AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this,  AMDS_DWELL_DETECTOR_ALT_PV_NOT_CONNECTED, "PV control is NOT connected.");
}

void AMDSDwellScalerDetector::onDwellScalerDwellModeControlValueChanged(double value)
{
	Q_UNUSED(value)

	// we will force to stop the current running data acquiring as long as the dwell mode is changed
	if(isDwellStarted()) {
		stopScalerDwellAcquisition(true);
	}
}

void AMDSDwellScalerDetector::onDwellScalerStartControlValueChanged(double value)
{
	Q_UNUSED(value)

	if(connected_){
		if(isDwellStarted()){
			startScalerDwellAcquisition();
		} else {
			// force-stop the current running continuous dwelling
			if(isContinuousDwell() && triggerDwellTimer_->isActive()){
				stopScalerDwellAcquisition(true);
			}

			// only change the dwell state when the start control is set to 0 (stop)
			dwellScalerDwellStateControl_->move(0);
		}
	}
}

void AMDSDwellScalerDetector::onDwellScalerDwellTimeControlValueChanged(double value)
{
	if(connected_){
		if (value > 0 && value < 100)
			dwellTimeInSecond_ = value;
		else {
			AMDSRunTimeSupport::debugMessage(AMDSRunTimeSupport::AlertMsg, this, AMDS_DWELL_DETECTOR_ALT_INCORRECT_DWELL_TIME, QString("The dwell time for %1 should be none-zero and less than 100s (given %2ms).").arg(dwellScalerDwellTimeControl_->readPVName()).arg(value));

			dwellScalerDwellTimeControl_->move(dwellTimeInSecond_);
		}
	}
}

void AMDSDwellScalerDetector::onTriggerDwellTimerTimeout()
{
	if(!isContinuousDwell()){
		// sigle-shot dwelling, stop data acquiring after the first data request
		triggerDwellTimer_->stop();
	}

	emit requestFlattenedData(dwellScalerDwellTimeControl_->value());
}

void AMDSDwellScalerDetector::startScalerDwellAcquisition()
{
	if (!isContinuousDwell()) {
		// for single-shot dwelling (step scan), we need to make sure to indicate the scan finished when all the channels received data

		// set the mapping of value changed.
		waitingChannels_.clear();

		AMSinglePVControl* scalerChannelFeedbackPVControl;
		foreach(int channelId, scalerConfiguration_->enabledChannels()) {
			scalerChannelFeedbackPVControl = dwellScalerChannelFeedbackControls_.value(channelId);
			if (scalerChannelFeedbackPVControl) {
				waitingChannels_.append(channelId);
				channelValueChangedSignalMapper_->setMapping(scalerChannelFeedbackPVControl, channelId);
			}
		}

		connect(channelValueChangedSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onScalerChannelValueChanged(int)));
	}

	// start the time
	int asMsecs = (int)(dwellTimeInSecond_*1000);
	triggerDwellTimer_->setInterval(asMsecs);

	dwellScalerDwellStateControl_->move(1.0);
	triggerDwellTimer_->start();
}

void AMDSDwellScalerDetector::stopScalerDwellAcquisition(bool forceStop)
{
	if (forceStop) {
		if (triggerDwellTimer_->isActive())
			triggerDwellTimer_->stop();

		foreach(int channelId, waitingChannels_) {
			waitingChannels_.removeAll(channelId);
			channelValueChangedSignalMapper_->removeMappings(dwellScalerChannelFeedbackControls_.value(channelId));
		}
	}

	if(waitingChannels_.count() == 0) {
		disconnect(channelValueChangedSignalMapper_, SIGNAL(mapped(int)), this, SLOT(onScalerChannelValueChanged(int)));

		dwellScalerStartControl_->move(0);
	}
}

void AMDSDwellScalerDetector::onScalerChannelValueChanged(int channelId)
{
	if(waitingChannels_.contains(channelId)){

		waitingChannels_.removeAll(channelId);
		channelValueChangedSignalMapper_->removeMappings(dwellScalerChannelFeedbackControls_.value(channelId));
	}

	stopScalerDwellAcquisition();
}

void AMDSDwellScalerDetector::initializePVControls()
{
	pvControlSet_ = new AMControlSet(this);
	dwellScalerChannelFeedbackControls_.clear();
	dwellScalerChannelEnableControls_.clear();

	// initialize the AMDS softIOC PV controls for scaler
	QString amdsScalerPVBaseName = scalerBasePVName()+":AMDS:scaler";
	dwellScalerStartControl_ = new AMSinglePVControl("ScalerStart", QString("%1:start").arg(amdsScalerPVBaseName), this, 0.5);
	dwellScalerDwellTimeControl_ = new AMSinglePVControl("ScalerDwellTime", QString("%1:dwellTime").arg(amdsScalerPVBaseName), this, 0.05);
	dwellScalerDwellStateControl_ = new AMSinglePVControl("ScalerDwellState", QString("%1:dwellState").arg(amdsScalerPVBaseName), this, 0.5);
	dwellScalerDwellModeControl_ = new AMSinglePVControl("ScalerDwellMode", QString("%1:dwellMode").arg(amdsScalerPVBaseName), this, 0.5);

	AMSinglePVControl *channelFeedbackControl;
	AMSinglePVControl *channelEnableControl;
	foreach (quint8 channelId, scalerConfiguration_->configuredChannels()) {
		QString channelIdTemplate = channelId < 10 ? "0%1" : "%1"; // when channel id is 0 ~ 9, the PV is "0" + channelId
		QString dwellScalerChannelFeedbackPVName = QString("%1:%2:fbk").arg(amdsScalerPVBaseName).arg(channelIdTemplate.arg(channelId));
		QString dwellScalerChannelEnablePVName = QString("%1:%2:enable").arg(amdsScalerPVBaseName).arg(channelIdTemplate.arg(channelId));

		channelFeedbackControl = new AMSinglePVControl(dwellScalerChannelFeedbackPVName, dwellScalerChannelFeedbackPVName, this, 0.5);
		channelEnableControl = new AMSinglePVControl(dwellScalerChannelEnablePVName, dwellScalerChannelEnablePVName, this, 0.5);

		dwellScalerChannelFeedbackControls_.insert(channelId, channelFeedbackControl);
		dwellScalerChannelEnableControls_.insert(channelId, channelEnableControl);
		pvControlSet_->addControl(channelFeedbackControl);
		pvControlSet_->addControl(channelEnableControl);

		connect(channelFeedbackControl, SIGNAL(valueChanged(double)), channelValueChangedSignalMapper_, SLOT(map()));
	}

	pvControlSet_->addControl(dwellScalerStartControl_);
	pvControlSet_->addControl(dwellScalerDwellTimeControl_);
	pvControlSet_->addControl(dwellScalerDwellStateControl_);
	pvControlSet_->addControl(dwellScalerDwellModeControl_);

	// connect the signal / slots
	connect(pvControlSet_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
	connect(pvControlSet_, SIGNAL(controlSetTimedOut()), this, SLOT(onAllControlsTimedOut()));

	connect(dwellScalerDwellModeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellScalerDwellModeControlValueChanged(double)));
	connect(dwellScalerStartControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellScalerStartControlValueChanged(double)));
	connect(dwellScalerDwellTimeControl_, SIGNAL(valueChanged(double)), this, SLOT(onDwellScalerDwellTimeControlValueChanged(double)));
}

const bool AMDSDwellScalerDetector::isDwellStarted()
{
	return dwellScalerStartControl_->withinTolerance(1.0);
}

const bool AMDSDwellScalerDetector::isContinuousDwell()
{
	return dwellScalerDwellModeControl_->withinTolerance(1.0);
}

const bool AMDSDwellScalerDetector::isAcquiring()
{
	return dwellScalerDwellStateControl_->withinTolerance(1.0);
}
