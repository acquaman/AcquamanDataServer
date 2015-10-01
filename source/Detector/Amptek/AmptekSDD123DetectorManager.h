#ifndef AMPTEKSDD123DETECTORMANAGER_H
#define AMPTEKSDD123DETECTORMANAGER_H

#include <QObject>
#include <QList>
#include <QVector>

#include "AmptekEventDefinitions.h"

class QSignalMapper;
class AMDSClientDataRequest;
class AmptekSDD123Detector;
class AmptekSDD123Histogram;
class AmptekSDD123DwellHistogramGroup;
class AmptekSDD123ThreadedHistogramGroup;

class AmptekSDD123ConfigurationMap;

class AmptekSDD123DetectorManager : public QObject
{
Q_OBJECT
public:
	enum DwellMode{
		ContinuousDwell = 0,
		PresetDwell = 1
	};

	enum ConfigurationModeReason{
		RequestConfigurationReason = 0,
		SetConfigurationReason = 1,
		InvalidReason
	};

	enum FastPeakingTimeValue{
		FastPeakingTime50 = 0,
		FastPeakingTime100 = 1,
		FastPeakingTime400 = 2
	};

	AmptekSDD123DetectorManager(AmptekSDD123Detector *detector, quint64 maxCountSize, QObject *parent = 0);

//	QString name() const;
//	bool dwellActive() const;

	double dwellTime() const;

	AmptekSDD123DetectorManager::DwellMode dwellMode() const;

	/// function to handle QEvent
	bool event(QEvent *e);

	/// helper function to return the detector of the current manager
	AmptekSDD123Detector *detector();

public slots:
	/// function to set the event receiver to handle the request
	void setRequestEventReceiver(QObject *requestEventReceiver);

	void clear();
	void startDwell();
	void stopDwell();

	void setDwellActive(bool dwellActive);
	void setDwellTime(double dwellTime);
	void setDwellMode(AmptekSDD123DetectorManager::DwellMode dwellMode);

	void requestDetectorConfigurationValues();

	void setDetectorConfigurationValues();
	void setDetectorAnalogGainIndex(int analogGainIndex);
	void setDetectorFineGain(double fineGain);
	void setDetectorTotalGain(double totalGain);
	void setDetectorHV(bool on);
	void setDetectorMCAEnabled(bool enabled);
	void setDetectorPileUpRejection(bool enabled);
	void setDetectorCoolerSetting(int coolerSetting);
	void setDetectorSingleChannelAnalyzer(int singleChannelAnalyzerIndex, int lowChannelIndex, int highChannelIndex);
	void setDetectorFastThreshold(int fastThreshold);
	void setDetectorSlowThreshold(double slowThreshold);
	void setDetectorPeakingTime(double peakingTime);
	void setDetectorFastPeakingTime(AmptekSDD123DetectorManager::FastPeakingTimeValue fastPeakingTimeValue);
	void forwardDataRequest(AMDSClientDataRequest*);

signals:
	void requestData(AMDSClientDataRequest*);
	void continuousDataUpdate(QVector<int> continuousSpectrum);
	void cumulativeStatusDataUpdate(AmptekStatusData statusData, int count);
	void continuousAllDataUpdate(QVector<int> spectrum, AmptekStatusData statusData, int count, double elapsedTime);

	void dwellFinishedDataUpdate(QVector<int> continuousSpectrum);
	void dwellFinishedStatusDataUpdate(AmptekStatusData statusData, int count);
	void dwellFinishedAllDataUpdate(QVector<int> spectrum, AmptekStatusData statusData, int count, double elapsedTime);

	void dwellFinishedTimeUpdate(double dwellTime);

	void configurationValuesUpdate(AmptekConfigurationData configurationData);
	void clientRequestProcessed(AMDSClientDataRequest*);


protected:
	void initiateRequestConfigurationModeHelper();

	void spectrumEventHelper(QEvent *e);
	void configurationValuesEventHelper(QEvent *e);
	void configurationModeConfirmationEventHelper(QEvent *e);

protected:
	AmptekSDD123Detector *detector_;
	AmptekConfigurationData configurationData_;

	QObject *requestEventReceiver_;

	bool dwellActive_;
	double dwellTime_;
	bool setPresetDwellEndTimeOnNextEvent_;
	QTime presetDwellEndTime_;
	QTime presetDwellLocalStartTime_;
	QTime presetDwellLocalEndTime_;
	AmptekSDD123DetectorManager::DwellMode dwellMode_;

	AmptekSDD123ThreadedHistogramGroup *allData_;
	int maxHistogramStackSize_;

	AmptekSDD123DwellHistogramGroup *dwellData_;
	bool presetDwellActive_;

	bool initialized_;

	AmptekSDD123DetectorManager::ConfigurationModeReason configurationRequestReason_;
	QString configurationSetCommand_;
};

#endif // AMPTEKSDD123DETECTORMANAGER_H
