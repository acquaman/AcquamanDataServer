#ifndef AMPTEKEVENTDEFINITIONS_H
#define AMPTEKEVENTDEFINITIONS_H

#include <QEvent>
#include <QVector>
#include <QTime>
#include <QStringList>

#include "DataElement/AMDSFlatArray.h"
#include "DataElement/AMDSDwellStatusData.h"

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

//class AmptekStatusData
//{
//public:
//	AmptekStatusData(int fastCounts = -1, int slowCounts = -1, double detectorTemperature = -1, double accumulationTime = -1, double liveTime = -1, double realTime = -1, int generalPurposeCounter = -1, const QTime &dwellStartTime = QTime(), const QTime &dwellEndTime = QTime(), const QTime &dwellReplyTime = QTime());
//	AmptekStatusData(const AmptekStatusData &other);
//	AmptekStatusData& operator =(const AmptekStatusData &other);

//	int fastCounts_;
//	int slowCounts_;
//	double detectorTemperature_;
//	double accumulationTime_;
//	double liveTime_;
//	double realTime_;
//	int generalPurposeCounter_;
//	QTime dwellStartTime_;
//	QTime dwellEndTime_;
//	QTime dwellReplyTime_;
//};

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
//	QVector<double> spectrum_;
	AMDSFlatArray spectrum_;
	AMDSDwellStatusData statusData_;
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

class AmptekConfigurationData
{
public:
	AmptekConfigurationData(int analogGainIndex = -1, double fineGain = -1, double totalGain = -1, QString hvSet = "NULL", int mcaCount = -1, QString mcaEnabled = "NULL", QString mcaSource = "NULL", QString preampState = "NULL", QString peakDetectMode = "NULL", QString pileUpRejection = "NULL", QList<int> scaIndices = QList<int>(), QList<int> scaHighIndices = QList<int>(), QList<int> scaLowIndices = QList<int>(), QString coolerSetting = "NULL", double fastThreshold = -1, double slowThreshold = -1, double peakingTime = -1, int fastChannelPeakingTime = -1);
	AmptekConfigurationData(const AmptekConfigurationData &other);

	AmptekConfigurationData& operator =(const AmptekConfigurationData &other);

	/// helper function to pase the configurations
	void parseConfiguration(QString configuration);

//protected:
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
	double fastThreshold_; //THFA
	double slowThreshold_; //THSL
	double peakingTime_; //TPEA
	int fastChannelPeakingTime_; //TPFA
};

class AmptekConfigurationValuesEvent: public QEvent
{
public:
	AmptekConfigurationValuesEvent() : QEvent( (QEvent::Type)AmptekEventDefinitions::ConfigurationValuesEvent) {}

	/// helper function to pase the list of configurations in the event
	void parseConfigurations(QStringList configurationList);

	/// returns the configurationData
	inline AmptekConfigurationData configurationData() { return configurationData_; }

protected:
	/// the configuraiton data
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
