#ifndef AMPTEKSDD123DETECTOR_H
#define AMPTEKSDD123DETECTOR_H

#include <QObject>
#include <QStringList>
#include <QVector>

#include "DataElement/AMDSDataTypeDefinitions.h"
#include "DataElement/AMDSFlatArray.h"
#include "Detector/Amptek/AmptekEventDefinitions.h"

#define AMPTEK_ALERT_CHANNEL_NUMBER_UNMATCH 40101

class AmptekSDD123Detector : public QObject
{
	Q_OBJECT

public:
	AmptekSDD123Detector(const QString &name, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, int bufferSize, QObject *parent = 0);

	/// function to handle the system events
	virtual bool event(QEvent *e);

	/// returns the dataType the detector will return
	inline AMDSDataTypeDefinitions::DataType dataType() { return dataType(); }
	/// returns the buffersize (spectrum size) the detector will return
	inline int bufferSize() { return bufferSize_; }

	/// returns the name of the detector
	inline QString name() const { return name_;}
	/// returns the basePVName of the detector
	inline QString basePVName() const { return basePVName_;}

	/// set the receiver of the new spectrum data
	inline void setSpectrumReceiver(QObject *spectrumReceiver) { spectrumReceiver_ = spectrumReceiver; }

signals:
	/// signal to indicate that spectrum changed with a list of spectrums
	void spectrumChanged(QList<double> spectrum);
	/// signal to indicate that status data is read
	void statusDataRead();
//	void textConfigurationRead();

	/// signal to indicate that fast counts changed
	void fastCountsChanged(int fastCounts);
	void fastCountsChanged(const QString &fastCounts);
	/// signal to indicate that slow counts changed
	void slowCountsChanged(int slowCounts);
	void slowCountsChanged(const QString &slowCounts);
	/// signal to indicate that genral purpose counter changed
	void generalPurposeCounterChanged(int generalPurposeCounter);
	void generalPurposeCounterChanged(const QString &generalPurposeCounter);
	/// signal to indicate that accumulation time changed
	void accumulationTimeChanged(double accumulationTime);
	void accumulationTimeChanged(const QString &accumulationTime);
	/// signal to indicate that live time changed
	void liveTimeChanged(double liveTime);
	void liveTimeChanged(const QString &liveTime);
	/// signal to indicate that real time changed
	void realTimeChanged(double realTime);
	void realTimeChanged(const QString &realTime);
	/// signal to indicate that firmware major version changed
	void firmwareMajorVersionChanged(int firmwareMajorVersion);
	void firmwareMajorVersionChanged(const QString &firmwareMajorVersion);
	/// signal to indicate that firmware minor version changed
	void firmwareMinorVersionChanged(int firmwareMinorVersion);
	void firmwareMinorVersionChanged(const QString &firmwareMinorVersion);
	/// signal to indicate that fpga major version changed
	void fpgaMajorVersionChanged(int fpgaMajorVersion);
	void fpgaMajorVersionChanged(const QString &fpgaMajorVersion);
	/// signal to indicate that fpga minor version changed
	void fpgaMinorVersionChanged(int fpgaMinorVersion);
	void fpgaMinorVersionChanged(const QString &fpgaMinorVersion);
	/// signal to indicate that the serial number changed
	void serialNumberChanged(int serialNumber);
	void serialNumberChanged(const QString &serialNumber);
	/// signal to indicate that the high voltage changed
	void highVoltageChanged(double highVoltage);
	void highVoltageChanged(const QString &highVoltage);
	/// signal to indicate that the detector temperature changed
	void detectorTemperatureChanged(double detectorTemperature);
	void detectorTemperatureChanged(const QString &detectorTemperature);
	/// signal to indicate that the board temperature changed
	void boardTemperatureChanged(int boardTemperature);
	void boardTemperatureChanged(const QString &boardTemperature);
	/// signal to indicate that the preset real time reached changed
	void presetRealTimeReachedChanged(bool presetRealTimeReached);
	void presetRealTimeReachedChanged(const QString &presetRealTimeReached);
	/// signal to indicate that the auto fast threshold locked changed
	void autoFastThresholdLockedChanged(bool autoFastThresholdLocked);
	void autoFastThresholdLockedChanged(const QString &autoFastThresholdLocked);
	/// signal to indicate that the mca enabled changed
	void mcaEnabledChanged(bool mcaEnabled);
	void mcaEnabledChanged(const QString &mcaEnabled);
	/// signal to indicate that the preset count reached changed
	void presetCountReachedChanged(bool presetCountReached);
	void presetCountReachedChanged(const QString &presetCountReached);
	/// signal to indicate that the oscilloscope data ready changed
	void oscilloscopeDataReadyChanged(bool oscilloscopeDataReady);
	void oscilloscopeDataReadyChanged(const QString &oscilloscopeDataReady);
	/// signal to indicate that the unit configured changed
	void unitConfiguredChanged(bool unitConfigured);
	void unitConfiguredChanged(const QString &unitConfigured);
	/// signal to indicate that the auto input offset searching changed
	void autoInputOffsetSearchingChanged(bool autoInputOffsetSearching);
	void autoInputOffsetSearchingChanged(const QString &autoInputOffsetSearching);
	/// signal to indicate that the mcs finished changed
	void mcsFinishedChanged(bool mcsFinished);
	void mcsFinishedChanged(const QString &mcsFinished);
	/// signal to indicate that the using 20MHz changed
	void using20MHzChanged(bool using20MHz);
	void using20MHzChanged(const QString &using20MHz);
	/// signal to indicate that the using 80MHz changed
	void using80MHzChanged(bool using80MHz);
	void using80MHzChanged(const QString &using80MHz);
	/// signal to indicate that the clock auto set changed
	void clockAutoSetChanged(bool clockAutoSet);
	void clockAutoSetChanged(const QString &clockAutoSet);
	/// signal to indicate that the pc5 detected at power up changed
	void pc5DetectedAtPowerUpChanged(bool pc5DetectedAtPowerUp);
	void pc5DetectedAtPowerUpChanged(const QString &pc5DetectedAtPowerUp);
	/// signal to indicate that the pc5 HV negative  changed
	void pc5HVNegativeChanged(bool pc5HVNegative);
	void pc5HVNegativeChanged(const QString &pc5HVNegative);
	/// signal to indicate that the pc5 HV positive changed
	void pc5HVPositiveChanged(bool pc5HVPositive);
	void pc5HVPositiveChanged(const QString &pc5HVPositive);
	/// signal to indicate that the pc5 preamp 5V changed
	void pc5Preamp5VChanged(bool pc5Preamp5V);
	void pc5Preamp5VChanged(const QString &pc5Preamp5V);
	/// signal to indicate that the pc5 preamp 8p5V changed
	void pc5Preamp8p5VChanged(bool pc5Preamp8p5V);
	void pc5Preamp8p5VChanged(const QString &pc5Preamp8p5V);
	/// signal to indicate that the deviceID changed
	void deviceIDChanged(int deviceID);
	void deviceIDChanged(const QString &deviceID);

	/// signal to indicate that the preset time changed
	void presetTimeChanged(double presetTime);
	void presetTimeChanged(const QString &presetTime);
	/// signal to indicate that the mca channels changed
	void mcaChannelsChanged(int mcaChannels);
	void mcaChannelsChanged(const QString &mcaChannels);

protected:

	/// helper function to handle the SpectrumPacketEvent
	void onSpectrumPacketEventReceived(AmptekSpectrumPacketEvent* event);
	/// helper function to handle the ConfigurationReadbackEvent
	void onConfigurationReadbackEventReceived(AmptekConfigurationReadbackEvent* event);

	/// helper function to read the spectrum data
	AMDSFlatArray *readSpectrumData(const QByteArray &spectrumData, int numChannels);
	/// helper function to read the spectrum status data
	void readStatusData(const QByteArray &statusData);
	/// helper function to read the text configuration readback
	void readTextConfigurationReadback(const QString &textConfiguration);

	/// helper function to copy date backward from "start" till "start+length" of inputData to outputData
	void backwardsMid(int start, int length, const QByteArray &inputData, QByteArray &outputData);
	/// helper function to covert bool true/false to string "true"/"false"
	QString stringFromBool(bool input) const;

	/// helper functions to setup the detector attributes and emit related signals
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

	/// returns the current fastCounts
	inline int fastCounts() const {return fastCounts_;}
	/// returns the current slowCounts
	inline int slowCounts() const {return slowCounts_;}
	/// returns the current generalPurposeCounter
	inline int generalPurposeCounter() const {return generalPurposeCounter_;}
	/// returns the current accumulationTime
	inline double accumulationTime() const {return accumulationTime_;}
	/// returns the current liveTime
	inline double liveTime() const {return liveTime_;}
	/// returns the current realTime
	inline double realTime() const {return realTime_;}
	/// returns the current detectorTemperature
	inline double detectorTemperature() const {return detectorTemperature_;}

	/// The following methods are NOT used
	inline bool configurationParametersInitialized() const { return configurationParametersInitialized_;}
	inline int firmwareMajorVersion() const {return firmwareMajorVersion_;}
	inline int firmwareMinorVersion() const {return firmwareMinorVersion_;}
	inline int fpgaMajorVersion() const {return fpgaMajorVersion_;}
	inline int fpgaMinorVersion() const {return fpgaMinorVersion_;}
	inline int serialNumber() const {return serialNumber_;}
	inline double highVoltage() const {return highVoltage_;}
	inline int boardTemperature() const {return boardTemperature_;}
	inline bool presetRealTimeReached() const {return presetRealTimeReached_;}
	inline bool autoFastThresholdLocked() const {return autoFastThresholdLocked_;}
	inline bool mcaEnabled() const {return mcaEnabled_;}
	inline bool presetCountReached() const {return presetCountReached_;}
	inline bool oscilloscopeDataReady() const {return oscilloscopeDataReady_;}
	inline bool unitConfigured() const {return unitConfigured_;}
	inline bool autoInputOffsetSearching() const {return autoInputOffsetSearching_;}
	inline bool mcsFinished() const {return mcsFinished_;}
	inline bool using20MHz() const {return using20MHz_;}
	inline bool using80MHz() const {return using80MHz_;}
	inline bool clockAutoSet() const {return clockAutoSet_;}
	inline bool pc5DetectedAtPowerUp() const {return pc5DetectedAtPowerUp_;}
	inline bool pc5HVNegative() const {return pc5HVNegative_;}
	inline bool pc5HVPositive() const {return pc5HVPositive_;}
	inline bool pc5Preamp5V() const {return pc5Preamp5V_;}
	inline bool pc5Preamp8p5V() const {return pc5Preamp8p5V_;}
	inline int deviceID() const {return deviceID_;}
	/// Returns the preset time (PRET). Returns -1 if the value was set to "OFF".
	inline double presetTime() const {return presetTime_;}
	/// Returns the number of MCA Channels being used (MCAC)
	inline int mcaChannels() const { return mcaChannels_;}

protected:
	/// the event receiver for spectrum package
	QObject *spectrumReceiver_;

	/// the name of the detector
	QString name_;
	/// the basePVName of the detector
	QString basePVName_;
	///	the dataType of the data in the spectrum
	AMDSDataTypeDefinitions::DataType dataType_;
	///	the size of the spectrum
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

	/// Holds whether or not the text configuration has been fully readback at least once
	bool configurationParametersInitialized_;

	//// START OF CONFIGURATION READBACK PARAMETERS ////
	/// Preset time for configuration (PRET). A value of -1 is associated with the "OFF" value.
	double presetTime_;
	/// Number of MCA Channels in use (MCAC)
	int mcaChannels_;
	//// END OF CONFIGURATION READBACK PARAMETERS ////
};

#endif // AMPTEKSDD123DETECTOR_H
