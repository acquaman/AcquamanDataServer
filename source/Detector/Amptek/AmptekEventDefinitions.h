#ifndef AMPTEKEVENTDEFINITIONS_H
#define AMPTEKEVENTDEFINITIONS_H

#include <QEvent>
#include <QVector>
#include <QTime>
#include <QStringList>

namespace AmptekEventDefinitions{
	enum EventType{
		SpectrumEvent = QEvent::User+2727,
		SpectrumPacketEvent = QEvent::User+2728,
		ConfigurationReadbackEvent = QEvent::User+2729,
		ConfigurationValuesEvent = QEvent::User+2730,
		ConfigurationInitiateRequestEvent = QEvent::User+2731,
		ConfigurationRequestEvent = QEvent::User+2732,
		DwellRequestEvent = QEvent::User+2733,
		ConfigurationSetEvent = QEvent::User+2734,
		ConfigurationModeConfirmationEvent = QEvent::User+2735
	};
}

class AmptekStatusData{
public:
	AmptekStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime()) :
		fastCounts_(fastCounts), slowCounts_(slowCounts), detectorTemperature_(detectorTemperature), accumulationTime_(accumulationTime), liveTime_(liveTime), realTime_(realTime), generalPurposeCounter_(generalPurposeCounter)
	{
		dwellStartTime_.setHMS(dwellStartTime.hour(), dwellStartTime.minute(), dwellStartTime.second(), dwellStartTime.msec());
		dwellEndTime_.setHMS(dwellEndTime.hour(), dwellEndTime.minute(), dwellEndTime.second(), dwellEndTime.msec());
		dwellReplyTime_.setHMS(dwellReplyTime.hour(), dwellReplyTime.minute(), dwellReplyTime.second(), dwellReplyTime.msec());
	}

	AmptekStatusData(const AmptekStatusData &other)
	{
		this->operator =(other);
	}

	AmptekStatusData& operator =(const AmptekStatusData &other)
	{
		if(this != &other){
			fastCounts_ = other.fastCounts_;
			slowCounts_ = other.slowCounts_;
			detectorTemperature_ = other.detectorTemperature_;
			accumulationTime_ = other.accumulationTime_;
			liveTime_ = other.liveTime_;
			realTime_ = other.realTime_;
			generalPurposeCounter_ = other.generalPurposeCounter_;
			dwellStartTime_.setHMS(other.dwellStartTime_.hour(), other.dwellStartTime_.minute(), other.dwellStartTime_.second(), other.dwellStartTime_.msec());
			dwellEndTime_.setHMS(other.dwellEndTime_.hour(), other.dwellEndTime_.minute(), other.dwellEndTime_.second(), other.dwellEndTime_.msec());
			dwellReplyTime_.setHMS(other.dwellReplyTime_.hour(), other.dwellReplyTime_.minute(), other.dwellReplyTime_.second(), other.dwellReplyTime_.msec());

		}
		return *this;
	}

	int fastCounts_;
	int slowCounts_;
	double detectorTemperature_;
	double accumulationTime_;
	double liveTime_;
	double realTime_;
	int generalPurposeCounter_;
	QTime dwellStartTime_;
	QTime dwellEndTime_;
	QTime dwellReplyTime_;
};

class AmptekSpectrumEvent : public QEvent
{
public:
	AmptekSpectrumEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::SpectrumEvent) {}

//	inline QString detectorSourceName() const { return detectorSourceName_; }
//	inline QVector<double> spectrum() { return spectrum_; }
//	inline AmptekStatusData statusData() { return statusData_; }

//	inline void setDetectorSourceName(const QString &detectorSourceName) { detectorSourceName_ = detectorSourceName; }
//	inline void setSpectrum(QVector<double> spectrums) { spectrum_.clear(); spectrum_ = spectrums; }
//	inline void setStatusData(AmptekStatusData statusData) { statusData_ = statusData; }

//protected:
	QString detectorSourceName_;
	QVector<double> spectrum_;
	AmptekStatusData statusData_;
};

class AmptekSpectrumPacketEvent : public QEvent
{
public:
	AmptekSpectrumPacketEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::SpectrumPacketEvent)
	{}

	QByteArray spectrumByteArray_;
	QByteArray statusDataArray_;
	int channelCount_;
	QTime dwellStartTime_;
	QTime dwellEndTime_;
	QTime dwellReplyTime_;
};

class AmptekConfigurationReadbackEvent : public QEvent
{
public:
	AmptekConfigurationReadbackEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationReadbackEvent)
	{}

	QString configurationReadback_;
};

class AmptekConfigurationData{
public:
	AmptekConfigurationData(int analogGainIndex = -1, double fineGain = -1, double totalGain = -1, QString hvSet = "NULL", int mcaCount = -1, QString mcaEnabled = "NULL", QString mcaSource = "NULL", QString preampState = "NULL", QString peakDetectMode = "NULL", QString pileUpRejection = "NULL", QList<int> scaIndices = QList<int>(), QList<int> scaHighIndices = QList<int>(), QList<int> scaLowIndices = QList<int>(), QString coolerSetting = "NULL", int fastThreshold = -1, double slowThreshold = -1, double peakingTime = -1, int fastChannelPeakingTime = -1)
	{
		analogGainIndex_ = analogGainIndex;
		fineGain_ = fineGain;
		totalGain_ = totalGain;
		hvSet_ = hvSet;
		mcaCount_ = mcaCount;
		mcaEnabled_ = mcaEnabled;
		mcaSource_ = mcaSource;
		preampState_ = preampState;
		peakDetectMode_ = peakDetectMode;
		pileUpRejection_ = pileUpRejection;
		scaIndices_ = scaIndices;
		scaHighIndices_ = scaHighIndices;
		scaLowIndices_ = scaLowIndices;
		coolerSetting_ = coolerSetting;
		fastThreshold_ = fastThreshold;
		slowThreshold_ = slowThreshold;
		peakingTime_ = peakingTime;
		fastChannelPeakingTime_ = fastChannelPeakingTime;
	}

	AmptekConfigurationData(const AmptekConfigurationData &other)
	{
		this->operator =(other);
	}

	AmptekConfigurationData& operator =(const AmptekConfigurationData &other)
	{
		if(this != &other){
			analogGainIndex_ = other.analogGainIndex_;
			fineGain_ = other.fineGain_;
			totalGain_ = other.totalGain_;
			hvSet_ = other.hvSet_;
			mcaCount_ = other.mcaCount_;
			mcaEnabled_ = other.mcaEnabled_;
			mcaSource_ = other.mcaSource_;
			preampState_ = other.preampState_;
			peakDetectMode_ = other.peakDetectMode_;
			pileUpRejection_ = other.pileUpRejection_;
			coolerSetting_ = other.coolerSetting_;
			scaIndices_ = other.scaIndices_;
			scaHighIndices_ = other.scaHighIndices_;
			scaLowIndices_ = other.scaLowIndices_;
			fastThreshold_ = other.fastThreshold_;
			slowThreshold_ = other.slowThreshold_;
			peakingTime_ = other.peakingTime_;
			fastChannelPeakingTime_ = other.fastChannelPeakingTime_;
		}
		return *this;
	}

	int analogGainIndex_; //GAIA
	double fineGain_; //GAIF
	double totalGain_; //GAIN
	QString hvSet_; //HVSE
	int mcaCount_; //MCAC
	QString mcaEnabled_; //MCAE
	QString mcaSource_; //MCAS
	QString preampState_; //PAPS
	QString peakDetectMode_; //PDMD
	QString pileUpRejection_; //PURE
	QList<int> scaIndices_; //SCAI
	QList<int> scaHighIndices_; //SCAH
	QList<int> scaLowIndices_; //SCAL
	QString coolerSetting_; //TECS
	int fastThreshold_; //THFA
	double slowThreshold_; //THSL
	double peakingTime_; //TPEA
	int fastChannelPeakingTime_; //TPFA
};

class AmptekConfigurationValuesEvent : public QEvent
{
public:
	AmptekConfigurationValuesEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationValuesEvent)
	{}

	AmptekConfigurationData configurationData_;
};

class AmptekConfigurationInitiateRequestEvent : public QEvent
{
public:
	AmptekConfigurationInitiateRequestEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationInitiateRequestEvent)
	{
		configurationMode_ = false;
	}

	bool configurationMode_;
};

class AmptekConfigurationRequestEvent : public QEvent
{
public:
	AmptekConfigurationRequestEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationRequestEvent)
	{
		allParametersRequest_ = false;
	}

	bool allParametersRequest_;
};

class AmptekDwellRequestEvent : public QEvent
{
public:
	AmptekDwellRequestEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::DwellRequestEvent)
	{
		dwellMode_ = false;
	}

	bool dwellMode_;
};

class AmptekConfigurationSetEvent : public QEvent
{
public:
	AmptekConfigurationSetEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationSetEvent)
	{}

	QStringList configurationCommands_;
};

class AmptekConfigurationModeConfirmationEvent : public QEvent
{
public:
	AmptekConfigurationModeConfirmationEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationModeConfirmationEvent)
	{
		confirmConfigurationMode_ = false;
	}

	bool confirmConfigurationMode_;
};
#endif // AMPTEKEVENTDEFINITIONS_H
