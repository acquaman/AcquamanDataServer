#ifndef AMPTEKSDD123DETECTOR_H
#define AMPTEKSDD123DETECTOR_H

#include <QObject>
#include <QStringList>
#include <QVector>

#include "DataElement/AMDSDataTypeDefinitions.h"

class AMDSFlatArray;
class AMDSBufferGroupInfo;

class AmptekSDD123Detector : public QObject
{
	Q_OBJECT

public:
	AmptekSDD123Detector(const QString &name, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, int bufferSize, QObject *parent = 0);

	AMDSDataTypeDefinitions::DataType dataType() { return dataType(); }
	int bufferSize() { return bufferSize_; }

	QString name() const { return name_;}
	QString basePVName() const { return basePVName_;}
	bool configurationParametersInitialized() const { return configurationParametersInitialized_;}

	int fastCounts() const {return fastCounts_;}
	int slowCounts() const {return slowCounts_;}
	int generalPurposeCounter() const {return generalPurposeCounter_;}
	double accumulationTime() const {return accumulationTime_;}
	double liveTime() const {return liveTime_;}
	double realTime() const {return realTime_;}
	int firmwareMajorVersion() const {return firmwareMajorVersion_;}
	int firmwareMinorVersion() const {return firmwareMinorVersion_;}
	int fpgaMajorVersion() const {return fpgaMajorVersion_;}
	int fpgaMinorVersion() const {return fpgaMinorVersion_;}
	int serialNumber() const {return serialNumber_;}
	double highVoltage() const {return highVoltage_;}
	double detectorTemperature() const {return detectorTemperature_;}
	int boardTemperature() const {return boardTemperature_;}
	bool presetRealTimeReached() const {return presetRealTimeReached_;}
	bool autoFastThresholdLocked() const {return autoFastThresholdLocked_;}
	bool mcaEnabled() const {return mcaEnabled_;}
	bool presetCountReached() const {return presetCountReached_;}
	bool oscilloscopeDataReady() const {return oscilloscopeDataReady_;}
	bool unitConfigured() const {return unitConfigured_;}
	bool autoInputOffsetSearching() const {return autoInputOffsetSearching_;}
	bool mcsFinished() const {return mcsFinished_;}
	bool using20MHz() const {return using20MHz_;}
	bool using80MHz() const {return using80MHz_;}
	bool clockAutoSet() const {return clockAutoSet_;}
	bool pc5DetectedAtPowerUp() const {return pc5DetectedAtPowerUp_;}
	bool pc5HVNegative() const {return pc5HVNegative_;}
	bool pc5HVPositive() const {return pc5HVPositive_;}
	bool pc5Preamp5V() const {return pc5Preamp5V_;}
	bool pc5Preamp8p5V() const {return pc5Preamp8p5V_;}
	int deviceID() const {return deviceID_;}

	/// Returns the preset time (PRET). Returns -1 if the value was set to "OFF".
	double presetTime() const {return presetTime_;}
	/// Returns the number of MCA Channels being used (MCAC)
	int mcaChannels() const { return mcaChannels_;}

	virtual bool event(QEvent *e);

public slots:
	void setName(const QString &name) { name_ = name;}
	void setBasePVName(const QString &basePVName) { basePVName_ = basePVName; }

	void readSpectrumData(const QByteArray &spectrumData, int numChannels);
	void readStatusData(const QByteArray &statusData);

	void readTextConfigurationReadback(const QString &textConfiguration);

	void setSpectrumReceiver(QObject *spectrumReceiver);

signals:
	void spectrumChanged(QList<double> spectrum);
	void statusDataRead();
	void textConfigurationRead();

	void fastCountsChanged(int fastCounts);
	void slowCountsChanged(int slowCounts);
	void generalPurposeCounterChanged(int generalPurposeCounter);
	void accumulationTimeChanged(double accumulationTime);
	void liveTimeChanged(double liveTime);
	void realTimeChanged(double realTime);
	void firmwareMajorVersionChanged(int firmwareMajorVersion);
	void firmwareMinorVersionChanged(int firmwareMinorVersion);
	void fpgaMajorVersionChanged(int fpgaMajorVersion);
	void fpgaMinorVersionChanged(int fpgaMinorVersion);
	void serialNumberChanged(int serialNumber);
	void highVoltageChanged(double highVoltage);
	void detectorTemperatureChanged(double detectorTemperature);
	void boardTemperatureChanged(int boardTemperature);
	void presetRealTimeReachedChanged(bool presetRealTimeReached);
	void autoFastThresholdLockedChanged(bool autoFastThresholdLocked);
	void mcaEnabledChanged(bool mcaEnabled);
	void presetCountReachedChanged(bool presetCountReached);
	void oscilloscopeDataReadyChanged(bool oscilloscopeDataReady);
	void unitConfiguredChanged(bool unitConfigured);
	void autoInputOffsetSearchingChanged(bool autoInputOffsetSearching);
	void mcsFinishedChanged(bool mcsFinished);
	void using20MHzChanged(bool using20MHz);
	void using80MHzChanged(bool using80MHz);
	void clockAutoSetChanged(bool clockAutoSet);
	void pc5DetectedAtPowerUpChanged(bool pc5DetectedAtPowerUp);
	void pc5HVNegativeChanged(bool pc5HVNegative);
	void pc5HVPositiveChanged(bool pc5HVPositive);
	void pc5Preamp5VChanged(bool pc5Preamp5V);
	void pc5Preamp8p5VChanged(bool pc5Preamp8p5V);
	void deviceIDChanged(int deviceID);

	void fastCountsChanged(const QString &fastCounts);
	void slowCountsChanged(const QString &slowCounts);
	void generalPurposeCounterChanged(const QString &generalPurposeCounter);
	void accumulationTimeChanged(const QString &accumulationTime);
	void liveTimeChanged(const QString &liveTime);
	void realTimeChanged(const QString &realTime);
	void firmwareMajorVersionChanged(const QString &firmwareMajorVersion);
	void firmwareMinorVersionChanged(const QString &firmwareMinorVersion);
	void fpgaMajorVersionChanged(const QString &fpgaMajorVersion);
	void fpgaMinorVersionChanged(const QString &fpgaMinorVersion);
	void serialNumberChanged(const QString &serialNumber);
	void highVoltageChanged(const QString &highVoltage);
	void detectorTemperatureChanged(const QString &detectorTemperature);
	void boardTemperatureChanged(const QString &boardTemperature);
	void presetRealTimeReachedChanged(const QString &presetRealTimeReached);
	void autoFastThresholdLockedChanged(const QString &autoFastThresholdLocked);
	void mcaEnabledChanged(const QString &mcaEnabled);
	void presetCountReachedChanged(const QString &presetCountReached);
	void oscilloscopeDataReadyChanged(const QString &oscilloscopeDataReady);
	void unitConfiguredChanged(const QString &unitConfigured);
	void autoInputOffsetSearchingChanged(const QString &autoInputOffsetSearching);
	void mcsFinishedChanged(const QString &mcsFinished);
	void using20MHzChanged(const QString &using20MHz);
	void using80MHzChanged(const QString &using80MHz);
	void clockAutoSetChanged(const QString &clockAutoSet);
	void pc5DetectedAtPowerUpChanged(const QString &pc5DetectedAtPowerUp);
	void pc5HVNegativeChanged(const QString &pc5HVNegative);
	void pc5HVPositiveChanged(const QString &pc5HVPositive);
	void pc5Preamp5VChanged(const QString &pc5Preamp5V);
	void pc5Preamp8p5VChanged(const QString &pc5Preamp8p5V);
	void deviceIDChanged(const QString &deviceID);

	void presetTimeChanged(double presetTime);
	void mcaChannelsChanged(int mcaChannels);

protected:
	void backwardsMid(int start, int length, const QByteArray &inputData, QByteArray &outputData);
	QString stringFromBool(bool input) const;

	void internalSetFastCounts(int fastCounts);
	void internalSetSlowCounts(int slowCounts);
	void internalSetGeneralPurposeCounter(int generalPurposeCounter);
	void internalSetAccumulationTime(double accumulationTime);
	void internalSetLiveTime(double liveTime);
	void internalSetRealTime(double realTime);
	void internalSetFirmwareMajorVersion(int firmwareMajorVersion);
	void internalSetFirmwareMinorVersion(int firmwareMinorVersion);
	void internalSetFpgaMajorVersion(int fpgaMajorVersion);
	void internalSetFpgaMinorVersion(int fpgaMinorVersion);
	void internalSetSerialNumber(int serialNumber);
	void internalSetHighVoltage(double highVoltage);
	void internalSetDetectorTemperature(double detectorTemperature);
	void internalSetBoardTemperature(int boardTemperature);
	void internalSetPresetRealTimeReached(bool presetRealTimeReached);
	void internalSetAutoFastThresholdLocked(bool autoFastThresholdLocked);
	void internalSetMcaEnabled(bool mcaEnabled);
	void internalSetPresetCountReached(bool presetCountReached);
	void internalSetOscilloscopeDataReady(bool oscilloscopeDataReady);
	void internalSetUnitConfigured(bool unitConfigured);
	void internalSetAutoInputOffsetSearching(bool autoInputOffsetSearching);
	void internalSetMcsFinished(bool mcsFinished);
	void internalSetUsing20MHz(bool using20MHz);
	void internalSetUsing80MHz(bool using80MHz);
	void internalSetClockAutoSet(bool clockAutoSet);
	void internalSetPc5DetectedAtPowerUp(bool pc5DetectedAtPowerUp);
	void internalSetPc5HVNegative(bool pc5HVNegative);
	void internalSetPc5HVPositive(bool pc5HVPositive);
	void internalSetPc5Preamp5V(bool pc5Preamp5V);
	void internalSetPc5Preamp8p5V(bool pc5Preamp8p5V);
	void internalSetDeviceID(int deviceID);

	/// Set the preset time (for use by the configuration text readback system)
	void internalSetPresetTime(double presetTime);
	/// Set the number of MCA channels (for use by the configuration text readback system)
	void internalSetMCAChannels(int mcaChannels);

protected:
	AMDSDataTypeDefinitions::DataType dataType_;
	int bufferSize_;

	//// START OF STATUS PACKET PARAMETERS ////
	int fastCounts_;
	int slowCounts_;
	int generalPurposeCounter_;
	double accumulationTime_;
	double liveTime_;
	double realTime_;
	int firmwareMajorVersion_;
	int firmwareMinorVersion_;
	int fpgaMajorVersion_;
	int fpgaMinorVersion_;
	int serialNumber_;
	double highVoltage_;
	double detectorTemperature_;
	int boardTemperature_;
	bool presetRealTimeReached_;
	bool autoFastThresholdLocked_;
	bool mcaEnabled_;
	bool presetCountReached_;
	bool oscilloscopeDataReady_;
	bool unitConfigured_;
	bool autoInputOffsetSearching_;
	bool mcsFinished_;
	bool using20MHz_;
	bool using80MHz_;
	bool clockAutoSet_;
	bool pc5DetectedAtPowerUp_;
	bool pc5HVNegative_;
	bool pc5HVPositive_;
	bool pc5Preamp5V_;
	bool pc5Preamp8p5V_;

	bool presetRealTimeReachedInitialized_;
	bool autoFastThresholdLockedInitialized_;
	bool mcaEnabledInitialized_;
	bool presetCountReachedInitialized_;
	bool oscilloscopeDataReadyInitialized_;
	bool unitConfiguredInitialized_;
	bool autoInputOffsetSearchingInitialized_;
	bool mcsFinishedInitialized_;
	bool using20MHzInitialized_;
	bool using80MHzInitialized_;
	bool clockAutoSetInitialized_;
	bool pc5DetectedAtPowerUpInitialized_;
	bool pc5HVNegativeInitialized_;
	bool pc5HVPositiveInitialized_;
	bool pc5Preamp5VInitialized_;
	bool pc5Preamp8p5VInitialized_;

	int deviceID_;
	//// END OF STATUS PACKET PARAMETERS ////

	//// START OF CONFIGURATION READBACK PARAMETERS ////
	/// Preset time for configuration (PRET). A value of -1 is associated with the "OFF" value.
	double presetTime_;
	/// Number of MCA Channels in use (MCAC)
	int mcaChannels_;
	//// END OF CONFIGURATION READBACK PARAMETERS ////

	QString name_;
	QString basePVName_;
	/// Holds whether or not the text configuration has been fully readback at least once
	bool configurationParametersInitialized_;

	QObject *spectrumReceiver_;
//	QVector<double> lastSpectrumVector_;
	AMDSFlatArray *lastSpectrumVector_;
};

#endif // AMPTEKSDD123DETECTOR_H
