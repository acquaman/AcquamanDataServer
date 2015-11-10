#include "AmptekSDD123ConfigurationMap.h"

AmptekSDD123ConfigurationMap::AmptekSDD123ConfigurationMap(const QString &detectorName, const QString &detectorBasePVName, QHostAddress detectorAddress, QHostAddress localAddress, int port, QHostAddress windowsAddress, AMDSDataTypeDefinitions::DataType dataType, quint64 spectrumCountSize, QObject *parent) :
	QObject(parent)
{
	detectorAddress_ = detectorAddress;
	localAddress_ = localAddress;
	port_ = port;
	windowsAddress_ = windowsAddress;

	detectorName_ = detectorName;

	detectorBasePVName_ = detectorBasePVName;

	dataType_ = dataType;
	spectrumCountSize_ = spectrumCountSize;
}
