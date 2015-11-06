#include "AmptekSDD123Detector.h"

#include <QCoreApplication>
#include <QDebug>

#include "util/AMErrorMonitor.h"

AmptekSDD123Detector::AmptekSDD123Detector(const QString &name, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, int bufferSize, QObject *parent)
	:QObject(parent)
{
	spectrumReceiver_ = 0;
	name_ = name;
	basePVName_ = basePVName;
	dataType_ = dataType;
	bufferSize_ = bufferSize;

	configurationParametersInitialized_ = false;

	fastCounts_ = -1;
	slowCounts_ = -1;
	generalPurposeCounter_ = -1;
	accumulationTime_ = -1;
	liveTime_ = -1;
	realTime_ = -1;
	firmwareMajorVersion_ = -1;
	firmwareMinorVersion_ = -1;
	fpgaMajorVersion_ = -1;
	fpgaMinorVersion_ = -1;
	serialNumber_ = -1;
	highVoltage_ = 272727272727;
	detectorTemperature_ = -1;
	boardTemperature_ = 2727272727;
	presetRealTimeReached_ = false;
	autoFastThresholdLocked_ = false;
	mcaEnabled_ = false;
	presetCountReached_ = false;
	oscilloscopeDataReady_ = false;
	unitConfigured_ = false;
	autoInputOffsetSearching_ = false;
	mcsFinished_ = false;
	using20MHz_ = false;
	using80MHz_ = false;
	clockAutoSet_ = false;
	pc5DetectedAtPowerUp_ = false;
	pc5HVNegative_ = false;
	pc5HVPositive_ = false;
	pc5Preamp5V_ = false;
	pc5Preamp8p5V_ = false;

	presetRealTimeReachedInitialized_ = false;
	autoFastThresholdLockedInitialized_ = false;
	mcaEnabledInitialized_ = false;
	presetCountReachedInitialized_ = false;
	oscilloscopeDataReadyInitialized_ = false;
	unitConfiguredInitialized_ = false;
	autoInputOffsetSearchingInitialized_ = false;
	mcsFinishedInitialized_ = false;
	using20MHzInitialized_ = false;
	using80MHzInitialized_ = false;
	clockAutoSetInitialized_ = false;
	pc5DetectedAtPowerUpInitialized_ = false;
	pc5HVNegativeInitialized_ = false;
	pc5HVPositiveInitialized_ = false;
	pc5Preamp5VInitialized_ = false;
	pc5Preamp8p5VInitialized_ = false;

	deviceID_ = -1;
}

AmptekSDD123Detector::~AmptekSDD123Detector()
{
	if (spectrumReceiver_ != 0)
		spectrumReceiver_ = 0;
}

bool AmptekSDD123Detector::event(QEvent *e){
	if(e->type() == (QEvent::Type)AmptekEventDefinitions::SpectrumPacketEvent){
		onSpectrumPacketEventReceived((AmptekSpectrumPacketEvent*)e);
		e->accept();
		return true;
	}
	else if(e->type() == (QEvent::Type)AmptekEventDefinitions::ConfigurationReadbackEvent){
		onConfigurationReadbackEventReceived((AmptekConfigurationReadbackEvent*)e);
	}

	return QObject::event(e);
}

void AmptekSDD123Detector::onSpectrumPacketEventReceived(AmptekSpectrumPacketEvent* event)
{

	QByteArray spectrumByteArray = event->spectrumByteArray_;
	int channelCount = event->channelCount_;
	QByteArray statusDataArray = event->statusDataArray_;

	AMDSFlatArray *spectrumArray = readSpectrumData(spectrumByteArray, channelCount);
	readStatusData(statusDataArray);

	if(spectrumReceiver_){

//		qDebug() << "At the AmptekSDD123Detector level: dwellStartTime, dwellEndTime, dwellReplyTime: " << event->dwellStartTime_ << event->dwellEndTime_ << event->dwellReplyTime_;

		AMDSDwellStatusData statusData(fastCounts(), slowCounts(), detectorTemperature(), accumulationTime(), liveTime(), realTime(), generalPurposeCounter(), event->dwellStartTime_, event->dwellEndTime_, event->dwellReplyTime_);

		AmptekSpectrumEvent *spectrumEvent = new AmptekSpectrumEvent();
		spectrumEvent->detectorSourceName_ = name();
		spectrumEvent->spectrum_ = *spectrumArray;
		spectrumEvent->statusData_ = statusData;

		QCoreApplication::postEvent(spectrumReceiver_, spectrumEvent);
	} else {
		AMErrorMon::alert(this, AMPTEK_ALERT_NO_SPECTRUM_EVENT_RECEIVER, QString("No spectrum receiver"));
	}
}

void AmptekSDD123Detector::onConfigurationReadbackEventReceived(AmptekConfigurationReadbackEvent* event)
{
	if (spectrumReceiver_) {
		QStringList allConfigurations = event->configurationReadback_.split(';');

		AmptekConfigurationValuesEvent *configurationValuesEvent = new AmptekConfigurationValuesEvent();
		configurationValuesEvent->parseConfigurations(allConfigurations);

		QCoreApplication::postEvent(spectrumReceiver_, configurationValuesEvent);
	} else {
		AMErrorMon::alert(this, AMPTEK_ALERT_NO_SPECTRUM_EVENT_RECEIVER, QString("No spectrum receiver"));
	}
}

AMDSFlatArray *AmptekSDD123Detector::readSpectrumData(const QByteArray &spectrumData, int numChannels){
	if (numChannels != bufferSize()) {
		AMErrorMon::alert(this, AMPTEK_ALERT_CHANNEL_NUMBER_UNMATCH, QString("Spectrum channel number (%1) doesn't match with the expected buffer size (%2)").arg(numChannels).arg(bufferSize()));
	}

	AMDSFlatArray *spectrumArray = new AMDSFlatArray(dataType(), bufferSize());

	QList<double> spectrum;
	QByteArray tmpData;
	bool ok;
	//qDebug() << spectrumData.size();

	for(int index = 0; index < numChannels; index++){
		backwardsMid(index*3, 3, spectrumData, tmpData);
		spectrum.append(tmpData.toHex().toInt(&ok, 16));

//		spectrumArray->setValue(index, tmpData.toHex().toInt(&ok, 16));
		spectrumArray->setValue(index, spectrum.last());
	}

	//qDebug() << "Spectrum ready:\n " << spectrum;
	emit spectrumChanged(spectrum);

	return spectrumArray;
}

void AmptekSDD123Detector::readStatusData(const QByteArray &statusData){

	//qDebug() << "Status hex: " << statusData.toHex();

	QString spacedStatus = statusData.toHex();
	int endVal = spacedStatus.size()-8;
	for(int x = endVal; x > 0; x-=8)
		spacedStatus.insert(x, " ");
//	//qDebug() << spacedStatus;

	bool ok;
	QByteArray tmpData;
	backwardsMid(0, 4, statusData, tmpData);
	internalSetFastCounts(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "Fast: " << tmpData.toHex() << fastCounts_;

	backwardsMid(4, 4, statusData, tmpData);
	internalSetSlowCounts(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "Slow: " << tmpData.toHex() << slowCounts_;

	backwardsMid(8, 4, statusData, tmpData);
	internalSetGeneralPurposeCounter(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "gpCounter: " << tmpData.toHex() << generalPurposeCounter_;

	backwardsMid(12, 1, statusData, tmpData);
	int accTime = tmpData.toHex().toInt(&ok, 16);
	//qDebug() << "accTime: " << tmpData.toHex() << accTime;

	backwardsMid(13, 3, statusData, tmpData);
	accTime += tmpData.toHex().toInt(&ok, 16)*100;
	internalSetAccumulationTime( ((double)(accTime))/1000);
	//qDebug() << "accTime: " << tmpData.toHex() << accTime;

	backwardsMid(16, 4, statusData, tmpData);
	internalSetLiveTime( (double)tmpData.toHex().toInt(&ok, 16)/1000);
	//qDebug() << "liveTime: " << tmpData.toHex() << liveTime;

	backwardsMid(20, 4, statusData, tmpData);
	internalSetRealTime( (double)tmpData.toHex().toInt(&ok, 16)/1000);
	//qDebug() << "realTime: " << tmpData.toHex() << realTime;

	backwardsMid(24, 1, statusData, tmpData);
	tmpData[0] = tmpData[0] & 0xf0;
	tmpData[0] = tmpData[0] >> 4;
	internalSetFirmwareMajorVersion(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "firmwareMajorVersion: " << tmpData.toHex() << firmwareMajorVersion;

	backwardsMid(24, 1, statusData, tmpData);
	tmpData[0] = tmpData[0] & 0x0f;
	internalSetFirmwareMinorVersion(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "firmwareMinorVersion: " << tmpData.toHex() << firmwareMinorVersion;

	backwardsMid(25, 1, statusData, tmpData);
	tmpData[0] = tmpData[0] & 0xf0;
	tmpData[0] = tmpData[0] >> 4;
	internalSetFpgaMajorVersion(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "fpgaMajorVersion: " << tmpData.toHex() << fpgaMajorVersion;

	backwardsMid(25, 1, statusData, tmpData);
	tmpData[0] = tmpData[0] & 0x0f;
	internalSetFpgaMinorVersion(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "fpgaMinorVersion: " << tmpData.toHex() << fpgaMinorVersion;

	backwardsMid(26, 4, statusData, tmpData);
	internalSetSerialNumber(tmpData.toHex().toInt(&ok, 16));
	 //qDebug() << "serialNumber: " << tmpData.toHex() << serialNumber;

	tmpData = statusData.mid(30, 2);
	internalSetHighVoltage((double)(tmpData.toHex().toUShort(&ok, 16)) * 0.5);
	//qDebug() << "hv: " << tmpData.toHex() << shortHV << hv;

	tmpData = statusData.mid(32, 2);
	tmpData[0] = tmpData[0] & 0x0f;
	internalSetDetectorTemperature(tmpData.toHex().toInt(&ok, 16) * 0.1);
	//qDebug() << "detectorTemp: " << tmpData.toHex() << detectorTemp;

	tmpData = statusData.mid(34, 1);
	internalSetBoardTemperature(tmpData.at(0));
	//qDebug() << "boardTemp: " << tmpData.toHex() << boardTemp;

	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x80;
	tmpData[0] = tmpData[0] >> 7;
	internalSetPresetRealTimeReached((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "presetRealTimeReached " << presetRealTimeReached_;

	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x40;
	tmpData[0] = tmpData[0] >> 6;
	internalSetAutoFastThresholdLocked((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "autoFastThresholdLocked " << autoFastThresholdLocked;

	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x20;
	tmpData[0] = tmpData[0] >> 5;
	internalSetMcaEnabled((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "mcaEnabled " << mcaEnabled_;

	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x10;
	tmpData[0] = tmpData[0] >> 4;
	internalSetPresetCountReached((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "presetCountReached " << presetCountReached;

	//qDebug() << "unusedByteThree " << unusedByteThree;
	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x04;
	tmpData[0] = tmpData[0] >> 2;
	internalSetOscilloscopeDataReady((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "oscilloscopeDataReady " << oscilloscopeDataReady;

	tmpData = statusData.mid(35, 1);
	tmpData[0] = tmpData[0] & 0x02;
	tmpData[0] = tmpData[0] >> 1;
	internalSetUnitConfigured((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "unitConfigured " << unitConfigured;

	//qDebug() << "unusedByte0 " << unusedByte0;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x80;
	tmpData[0] = tmpData[0] >> 7;
	internalSetAutoInputOffsetSearching((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "autoInputOffsetSearching " << autoInputOffsetSearching;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x40;
	tmpData[0] = tmpData[0] >> 6;
	internalSetMcsFinished((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "mcsFinished " << mcsFinished;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x02;
	tmpData[0] = tmpData[0] >> 1;
	internalSetUsing20MHz(!(bool)(tmpData.toHex().toInt(&ok, 16)));
	internalSetUsing80MHz((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "using20MHz " << using20MHz << " using80MHz " << using80MHz;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x01;
	internalSetClockAutoSet((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "clockAutoSet " << clockAutoSet_;

	tmpData = statusData.mid(37, 1);
	//UNUSED BYTE

	tmpData = statusData.mid(38, 1);
	tmpData[0] = tmpData[0] & 0x80;
	tmpData[0] = tmpData[0] >> 7;
	internalSetPc5DetectedAtPowerUp((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "pc5DetectedAtPowerUp " << pc5DetectedAtPowerUp;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x40;
	tmpData[0] = tmpData[0] >> 6;
	internalSetPc5HVNegative(!(bool)(tmpData.toHex().toInt(&ok, 16)));
	internalSetPc5HVPositive((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "pc5HVNegative " << pc5HVNegative << " pc5HVPositive " << pc5HVPositive;

	tmpData = statusData.mid(36, 1);
	tmpData[0] = tmpData[0] & 0x20;
	tmpData[0] = tmpData[0] >> 5;
	internalSetPc5Preamp5V(!(bool)(tmpData.toHex().toInt(&ok, 16)));
	internalSetPc5Preamp8p5V((bool)(tmpData.toHex().toInt(&ok, 16)));
	//qDebug() << "pc5Preamp5V " << pc5Preamp5V << " pc5Preamp8p5V " << pc5Preamp8p5V;

	tmpData = statusData.mid(39, 1);
	internalSetDeviceID(tmpData.toHex().toInt(&ok, 16));
	//qDebug() << "deviceID: " << deviceID;

	//qDebug() << QString("%1 GPCounter %2 %3").arg(serialNumber_).arg(QString(tmpData.toHex())).arg(generalPurposeCounter_);
	//qDebug() << "gpCounter: " << tmpData.toHex() << generalPurposeCounter_;

	//BYTES 40-63 ARE UNUSED
	//qDebug() << "Emit statusDataRead" << name_;
	emit statusDataRead();
}

void AmptekSDD123Detector::readTextConfigurationReadback(const QString &textConfiguration){
	QStringList ASCIICommandList = textConfiguration.split(';', QString::SkipEmptyParts);
	if(ASCIICommandList.contains("ALLP=??")){
		ASCIICommandList.removeAll("ALLP=??");
		configurationParametersInitialized_ = true;
	}
	QString ASCIICommand;
	foreach(ASCIICommand, ASCIICommandList){
		if(ASCIICommand.section('=', 0, 0) == "MCAC"){
			//qDebug() << "I think MCAC is " << ASCIICommand.section('=', 1).toDouble();
			//mcaChannels_->move(ASCIICommand.section('=', 1).toDouble());
			internalSetMCAChannels(ASCIICommand.section('=', 1).toInt());
		}
		else if(ASCIICommand.section('=', 0, 0) == "PRET")
			internalSetPresetTime(ASCIICommand.section('=', 1).toDouble());
			//presetTime_->move(ASCIICommand.section('=', 1).toDouble());
	}
}

void AmptekSDD123Detector::backwardsMid(int start, int length, const QByteArray &inputData, QByteArray &outputData){
	outputData.clear();
	for(int x = start+length; x > start; x--)
		outputData.append(inputData.at(x-1));
}

void AmptekSDD123Detector::internalSetFastCounts(int fastCounts){
	if(fastCounts != fastCounts_){
		fastCounts_ = fastCounts;
		emit fastCountsChanged(fastCounts_);
	}
}

void AmptekSDD123Detector::internalSetSlowCounts(int slowCounts){
	if(slowCounts != slowCounts_){
		slowCounts_ = slowCounts;
		emit slowCountsChanged(slowCounts_);
	}
}

void AmptekSDD123Detector::internalSetGeneralPurposeCounter(int generalPurposeCounter){
	if(generalPurposeCounter != generalPurposeCounter_){
		generalPurposeCounter_ = generalPurposeCounter;
		emit generalPurposeCounterChanged(generalPurposeCounter_);
	}
}

void AmptekSDD123Detector::internalSetAccumulationTime(double accumulationTime){
	if(accumulationTime != accumulationTime_){
		accumulationTime_ = accumulationTime;
		emit accumulationTimeChanged(accumulationTime_);
	}
}

void AmptekSDD123Detector::internalSetLiveTime(double liveTime){
	if(liveTime != liveTime_){
		liveTime_ = liveTime;
		emit liveTimeChanged(liveTime);
	}
}

void AmptekSDD123Detector::internalSetRealTime(double realTime){
	if(realTime != realTime_){
		realTime_ = realTime;
		emit realTimeChanged(realTime_);
	}
}

void AmptekSDD123Detector::internalSetFirmwareMajorVersion(int firmwareMajorVersion){
	if(firmwareMajorVersion != firmwareMajorVersion_){
		firmwareMajorVersion_ = firmwareMajorVersion;
		emit firmwareMajorVersionChanged(firmwareMajorVersion_);
	}
}

void AmptekSDD123Detector::internalSetFirmwareMinorVersion(int firmwareMinorVersion){
	if(firmwareMinorVersion != firmwareMinorVersion_){
		firmwareMinorVersion_ = firmwareMinorVersion;
		emit firmwareMinorVersionChanged(firmwareMinorVersion_);
	}
}

void AmptekSDD123Detector::internalSetFpgaMajorVersion(int fpgaMajorVersion){
	if(fpgaMajorVersion != fpgaMajorVersion_){
		fpgaMajorVersion_ = fpgaMajorVersion;
		emit fpgaMajorVersionChanged(fpgaMajorVersion_);
	}
}

void AmptekSDD123Detector::internalSetFpgaMinorVersion(int fpgaMinorVersion){
	if(fpgaMinorVersion != fpgaMinorVersion_){
		fpgaMinorVersion_ = fpgaMinorVersion;
		emit fpgaMinorVersionChanged(fpgaMinorVersion_);
	}
}

void AmptekSDD123Detector::internalSetSerialNumber(int serialNumber){
	if(serialNumber != serialNumber_){
		serialNumber_ = serialNumber;
		emit serialNumberChanged(serialNumber_);
	}
}

void AmptekSDD123Detector::internalSetHighVoltage(double highVoltage){
	if(highVoltage != highVoltage_){
		highVoltage_ = highVoltage;
		emit highVoltageChanged(highVoltage_);
	}
}

void AmptekSDD123Detector::internalSetDetectorTemperature(double detectorTemperature){
	if(detectorTemperature != detectorTemperature_){
		detectorTemperature_ = detectorTemperature;
		emit detectorTemperatureChanged(detectorTemperature_);
	}
}

void AmptekSDD123Detector::internalSetBoardTemperature(int boardTemperature){
	if(boardTemperature != boardTemperature_){
		boardTemperature_ = boardTemperature;
		emit boardTemperatureChanged(boardTemperature_);
	}
}

void AmptekSDD123Detector::internalSetPresetRealTimeReached(bool presetRealTimeReached){
	if(presetRealTimeReached != presetRealTimeReached_ || !presetRealTimeReachedInitialized_){
		presetRealTimeReachedInitialized_ = true;
		presetRealTimeReached_ = presetRealTimeReached;
		emit presetRealTimeReachedChanged(presetRealTimeReached_);
	}
}

void AmptekSDD123Detector::internalSetAutoFastThresholdLocked(bool autoFastThresholdLocked){
	if(autoFastThresholdLocked != autoFastThresholdLocked_ || !autoFastThresholdLockedInitialized_){
		autoFastThresholdLockedInitialized_ = true;
		autoFastThresholdLocked_ = autoFastThresholdLocked;
		emit autoFastThresholdLockedChanged(autoFastThresholdLocked_);
	}
}

void AmptekSDD123Detector::internalSetMcaEnabled(bool mcaEnabled){
	if(mcaEnabled != mcaEnabled_ || !mcaEnabledInitialized_){
		mcaEnabledInitialized_ = true;
		mcaEnabled_ = mcaEnabled;
		emit mcaEnabledChanged(mcaEnabled_);
	}
}

void AmptekSDD123Detector::internalSetPresetCountReached(bool presetCountReached){
	if(presetCountReached != presetCountReached_ || !presetCountReachedInitialized_){
		presetCountReachedInitialized_ = true;
		presetCountReached_ = presetCountReached;
		emit presetCountReachedChanged(presetCountReached_);
	}
}

void AmptekSDD123Detector::internalSetOscilloscopeDataReady(bool oscilloscopeDataReady){
	if(oscilloscopeDataReady != oscilloscopeDataReady_ || !oscilloscopeDataReadyInitialized_){
		oscilloscopeDataReadyInitialized_ = true;
		oscilloscopeDataReady_ = oscilloscopeDataReady;
		emit oscilloscopeDataReadyChanged(oscilloscopeDataReady_);
	}
}

void AmptekSDD123Detector::internalSetUnitConfigured(bool unitConfigured){
	if(unitConfigured != unitConfigured_ || !unitConfiguredInitialized_){
		unitConfiguredInitialized_ = true;
		unitConfigured_ = unitConfigured;
		emit unitConfiguredChanged(unitConfigured_);
	}
}

void AmptekSDD123Detector::internalSetAutoInputOffsetSearching(bool autoInputOffsetSearching){
	if(autoInputOffsetSearching != autoInputOffsetSearching_ || !autoInputOffsetSearchingInitialized_){
		autoInputOffsetSearchingInitialized_ = true;
		autoInputOffsetSearching_ = autoInputOffsetSearching;
		emit autoInputOffsetSearchingChanged(autoInputOffsetSearching_);
	}
}

void AmptekSDD123Detector::internalSetMcsFinished(bool mcsFinished){
	if(mcsFinished != mcsFinished_ || !mcsFinishedInitialized_){
		mcsFinishedInitialized_ = true;
		mcsFinished_ = mcsFinished;
		emit mcsFinishedChanged(mcsFinished_);
	}
}

void AmptekSDD123Detector::internalSetUsing20MHz(bool using20MHz){
	if(using20MHz != using20MHz_ || !using20MHzInitialized_){
		using20MHzInitialized_ = true;
		using80MHzInitialized_ = true;
		using20MHz_ = using20MHz;
		using80MHz_ = !using20MHz;
		emit using20MHzChanged(using20MHz_);
		emit using80MHzChanged(using80MHz_);
	}
}

void AmptekSDD123Detector::internalSetUsing80MHz(bool using80MHz){
	if(using80MHz != using80MHz_ || !using80MHzInitialized_){
		using20MHzInitialized_ = true;
		using80MHzInitialized_ = true;
		using80MHz_ = using80MHz;
		using20MHz_ = !using80MHz;
		emit using20MHzChanged(using20MHz_);
		emit using80MHzChanged(using80MHz_);
	}
}

void AmptekSDD123Detector::internalSetClockAutoSet(bool clockAutoSet){
	if(clockAutoSet != clockAutoSet_ || !clockAutoSetInitialized_){
		clockAutoSetInitialized_ = true;
		clockAutoSet_ = clockAutoSet;
		emit clockAutoSetChanged(clockAutoSet_);
	}
}

void AmptekSDD123Detector::internalSetPc5DetectedAtPowerUp(bool pc5DetectedAtPowerUp){
	if(pc5DetectedAtPowerUp != pc5DetectedAtPowerUp_ || !pc5DetectedAtPowerUpInitialized_){
		pc5DetectedAtPowerUpInitialized_ = true;
		pc5DetectedAtPowerUp_ = pc5DetectedAtPowerUp;
		emit pc5DetectedAtPowerUpChanged(pc5DetectedAtPowerUp_);
	}
}

void AmptekSDD123Detector::internalSetPc5HVNegative(bool pc5HVNegative){
	if(pc5HVNegative != pc5HVNegative_ || !pc5HVNegativeInitialized_){
		pc5HVNegativeInitialized_ = true;
		pc5HVPositiveInitialized_ = true;
		pc5HVNegative_ = pc5HVNegative;
		pc5HVPositive_ = !pc5HVNegative;
		emit pc5HVNegativeChanged(pc5HVNegative_);
		emit pc5HVPositiveChanged(pc5HVPositive_);
	}
}

void AmptekSDD123Detector::internalSetPc5HVPositive(bool pc5HVPositive){
	if(pc5HVPositive != pc5HVPositive_ || !pc5HVPositiveInitialized_){
		pc5HVNegativeInitialized_ = true;
		pc5HVPositiveInitialized_ = true;
		pc5HVPositive_ = pc5HVPositive;
		pc5HVNegative_ = !pc5HVPositive;
		emit pc5HVNegativeChanged(pc5HVNegative_);
		emit pc5HVPositiveChanged(pc5HVPositive_);
	}
}

void AmptekSDD123Detector::internalSetPc5Preamp5V(bool pc5Preamp5V){
	if(pc5Preamp5V != pc5Preamp5V_ || !pc5Preamp5VInitialized_){
		pc5Preamp5VInitialized_ = true;
		pc5Preamp8p5VInitialized_ = true;
		pc5Preamp5V_ = pc5Preamp5V;
		pc5Preamp8p5V_ = !pc5Preamp5V;
		emit pc5Preamp5VChanged(pc5Preamp5V_);
		emit pc5Preamp8p5VChanged(pc5Preamp8p5V_);
	}
}

void AmptekSDD123Detector::internalSetPc5Preamp8p5V(bool pc5Preamp8p5V){
	if(pc5Preamp8p5V != pc5Preamp8p5V_ || !pc5Preamp8p5VInitialized_){
		pc5Preamp5VInitialized_ = true;
		pc5Preamp8p5VInitialized_ = true;
		pc5Preamp8p5V_ = pc5Preamp8p5V;
		pc5Preamp5V_ = !pc5Preamp8p5V;
		emit pc5Preamp5VChanged(pc5Preamp5V_);
		emit pc5Preamp8p5VChanged(pc5Preamp8p5V_);
	}
}

void AmptekSDD123Detector::internalSetDeviceID(int deviceID){
	if(deviceID != deviceID_){
		deviceID_ = deviceID;
		emit deviceIDChanged(deviceID_);
	}
}

void AmptekSDD123Detector::internalSetPresetTime(double presetTime){
	if(presetTime_ != presetTime){
		presetTime_ = presetTime;
		emit presetTimeChanged(presetTime_);
	}
}

void AmptekSDD123Detector::internalSetMCAChannels(int mcaChannels){
	if(mcaChannels_ != mcaChannels){
		mcaChannels_ = mcaChannels;
		emit mcaChannelsChanged(mcaChannels_);
	}
}
