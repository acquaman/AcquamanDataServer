#include "AmptekEventDefinitions.h"

/// =============== implementation of AmptekConfigurationData ================
AmptekStatusData::AmptekStatusData(int fastCounts, int slowCounts, double detectorTemperature, double accumulationTime, double liveTime, double realTime, int generalPurposeCounter, const QTime &dwellStartTime, const QTime &dwellEndTime, const QTime &dwellReplyTime) :
	fastCounts_(fastCounts), slowCounts_(slowCounts), detectorTemperature_(detectorTemperature), accumulationTime_(accumulationTime), liveTime_(liveTime), realTime_(realTime), generalPurposeCounter_(generalPurposeCounter)
{
	dwellStartTime_.setHMS(dwellStartTime.hour(), dwellStartTime.minute(), dwellStartTime.second(), dwellStartTime.msec());
	dwellEndTime_.setHMS(dwellEndTime.hour(), dwellEndTime.minute(), dwellEndTime.second(), dwellEndTime.msec());
	dwellReplyTime_.setHMS(dwellReplyTime.hour(), dwellReplyTime.minute(), dwellReplyTime.second(), dwellReplyTime.msec());
}

AmptekStatusData::AmptekStatusData(const AmptekStatusData &other)
{
	this->operator =(other);
}

AmptekStatusData& AmptekStatusData::operator =(const AmptekStatusData &other)
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

void AmptekConfigurationData::parseConfiguration(QString configuration)
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
//TODO: figure out the definitions of these two configurations...
//	else if (configuration.contains("HightIncides"))
//		scaHighIndices_.append(configurationValue.toInt());
//	else if (configuration.contains("LowIncides"))
//		scaLowIndices_.append(configurationValue.toInt());
	else if(configuration.contains("SCAH")){
		//ignore this
	}
	else if(configuration.contains("SCAL")){
		//ignore this
	}
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
void AmptekConfigurationValuesEvent::parseConfigurations(QStringList configurationList)
{
	for(int x = 0, size = configurationList.count(); x < size; x++){
		QString configuration = configurationList.at(x);
		QString configurationValue = configuration.section('=', -1);
// TODO: after figuring out the defintions of configurationValueHightIndices / configurationValueLowIndices, we can use this function
//		configurationData_.parseConfiguration(configuration);

		if(configuration.contains("GAIA"))
			configurationData_.analogGainIndex_ = configurationValue.toInt();
		else if(configuration.contains("GAIF"))
			configurationData_.fineGain_ = configurationValue.toDouble();
		else if(configuration.contains("GAIN"))
			configurationData_.totalGain_ = configurationValue.toDouble();
		else if(configuration.contains("HVSE"))
			configurationData_.hvSet_ = configurationValue;
		else if(configuration.contains("MCAC"))
			configurationData_.mcaCount_ = configurationValue.toInt();
		else if(configuration.contains("MCAE"))
			configurationData_.mcaEnabled_ = configurationValue;
		else if(configuration.contains("MCAS"))
			configurationData_.mcaSource_ = configurationValue;
		else if(configuration.contains("PAPS"))
			configurationData_.preampState_ = configurationValue;
		else if(configuration.contains("PDMD"))
			configurationData_.peakDetectMode_ = configurationValue;
		else if(configuration.contains("PURE"))
			configurationData_.pileUpRejection_ = configurationValue;
		else if(configuration.contains("SCAI")){
			QString configurationValueHightIndices = configurationList.at(x+1).section('=', -1);
			QString configurationValueLowIndices = configurationList.at(x+2).section('=', -1);

			configurationData_.scaIndices_.append(configurationValue.toInt());
			configurationData_.scaHighIndices_.append(configurationValueHightIndices.toInt());
			configurationData_.scaLowIndices_.append(configurationValueLowIndices.toInt());
		}
		else if(configuration.contains("SCAH")){
			//ignore this
		}
		else if(configuration.contains("SCAL")){
			//ignore this
		}
		else if(configuration.contains("TECS"))
			configurationData_.coolerSetting_ = configurationValue;
		else if(configuration.contains("THFA"))
			configurationData_.fastThreshold_ = configurationValue.toDouble();
		else if(configuration.contains("THSL"))
			configurationData_.slowThreshold_ = configurationValue.toDouble();
		else if(configuration.contains("TPEA"))
			configurationData_.peakingTime_ = configurationValue.toDouble();
		else if(configuration.contains("TPFA"))
			configurationData_.fastChannelPeakingTime_ = configurationValue.toInt();
	}

}
