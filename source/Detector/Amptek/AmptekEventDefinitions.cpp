#include "AmptekEventDefinitions.h"

#include "util/AMDSRunTimeSupport.h"

/// =============== implementation of AmptekConfigurationData ================
AmptekConfigurationData::AmptekConfigurationData(int analogGainIndex, double fineGain, double totalGain, QString hvSet, int mcaCount, QString mcaEnabled, QString mcaSource, QString preampState, QString peakDetectMode, QString pileUpRejection, QList<int> scaIndices, QList<int> scaHighIndices, QList<int> scaLowIndices, QString coolerSetting, double fastThreshold, double slowThreshold, double peakingTime, int fastChannelPeakingTime)
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

AmptekConfigurationData::AmptekConfigurationData(const AmptekConfigurationData &other)
{
	this->operator =(other);
}

AmptekConfigurationData& AmptekConfigurationData::operator =(const AmptekConfigurationData &other)
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

void AmptekConfigurationData::parseConfiguration(const QString &configuration)
{
	QString configurationValue = configuration.section('=', -1);

	if(configuration.contains("GAIA"))
		analogGainIndex_ = configurationValue.toInt();
	else if(configuration.contains("GAIF"))
		fineGain_ = configurationValue.toDouble();
	else if(configuration.contains("GAIN"))
		totalGain_ = configurationValue.toDouble();
	else if(configuration.contains("HVSE"))
		hvSet_ = configurationValue;
	else if(configuration.contains("MCAC"))
		mcaCount_ = configurationValue.toInt();
	else if(configuration.contains("MCAE"))
		mcaEnabled_ = configurationValue;
	else if(configuration.contains("MCAS"))
		mcaSource_ = configurationValue;
	else if(configuration.contains("PAPS"))
		preampState_ = configurationValue;
	else if(configuration.contains("PDMD"))
		peakDetectMode_ = configurationValue;
	else if(configuration.contains("PURE"))
		pileUpRejection_ = configurationValue;
	else if(configuration.contains("SCAI"))
		scaIndices_.append(configurationValue.toInt());
	else if(configuration.contains("SCAH"))
		scaHighIndices_.append(configurationValue.toInt());
	else if(configuration.contains("SCAL"))
		scaLowIndices_.append(configurationValue.toInt());
	else if(configuration.contains("TECS"))
		coolerSetting_ = configurationValue;
	else if(configuration.contains("THFA"))
		fastThreshold_ = configurationValue.toDouble();
	else if(configuration.contains("THSL"))
		slowThreshold_ = configurationValue.toDouble();
	else if(configuration.contains("TPEA"))
		peakingTime_ = configurationValue.toDouble();
	else if(configuration.contains("TPFA"))
		fastChannelPeakingTime_ = configurationValue.toInt();
}


/// =============== implementation of AmptekConfigurationValuesEvent ================
void AmptekConfigurationValuesEvent::parseConfigurations(const QStringList &configurationList)
{
	for(int x = 0, size = configurationList.count(); x < size; x++){
		configurationData_.parseConfiguration(configurationList.at(x));
	}

}
