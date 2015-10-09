#ifndef AMPTEKSDD123CONFIGURATIONMAP_H
#define AMPTEKSDD123CONFIGURATIONMAP_H

#include <QHostAddress>

#include "DataElement/AMDSDataTypeDefinitions.h"

/**
  * A class containing all the configuration data relating to a single connection to a
  * detector.
  */
class AmptekSDD123ConfigurationMap : public QObject
{
	Q_OBJECT
public:
	/// Creates a detector connection configuration from the provided arguments
//	AmptekSDD123ConfigurationMap(const QString &detectorName, const QString &detectorBasePVName, QHostAddress detectorAddress, QHostAddress localAddress, int port, QHostAddress windowsAddress, AMDSDataTypeDefinitions::DataType dataType, quint64 spectrumCountSize, QObject *parent = 0);
	AmptekSDD123ConfigurationMap(const QString &detectorName, const QString &detectorBasePVName, QHostAddress detectorAddress, QHostAddress localAddress, int port, QHostAddress windowsAddress, quint64 spectrumCountSize, QObject *parent = 0);

	/// The host address on the network of the detector
	inline QHostAddress detectorAddress() const { return detectorAddress_; }
	/// The local address used to connect to the detector. Note that a single host address
	/// must be used per detector (though not necessarily network interface)
	inline QHostAddress localAddress() const { return localAddress_;}
	/// The port used to connect to the detector
	inline int port() const { return port_;}
	/// returns the windowsAddress
	inline QHostAddress windowsAddress() const { return windowsAddress_;}

	/// returns the name of the detector: A human readable name used to refer to the detector
	inline QString detectorName() const { return detectorName_; }
	/// return the name of the base ProcessVariable that the detector will be accessible through using the EPICS interface
	inline QString detectorBasePVName() const { return detectorBasePVName_; }

	/// returns the datatype of this Amptek
	inline AMDSDataTypeDefinitions::DataType dataType() { return dataType_; }
	/// returns the spectrum count size of this amptek
	inline quint64 spectrumCountSize() { return spectrumCountSize_; }

protected:
	/// the address of AmptekDetector
	QHostAddress detectorAddress_;
	/// the local address of AmptekDetector DPP server
	QHostAddress localAddress_;
	/// the port of AmptekDetector DPP server
	int port_;
	/// the windows address
	QHostAddress windowsAddress_;

	/// the name of the Amptek detector
	QString detectorName_;
	/// the base PV name of the Amptek detector
	QString detectorBasePVName_;

	/// the default data type the Amptek host server will send to the AMDS
	AMDSDataTypeDefinitions::DataType dataType_;
	/// the specturm count size the hsot server will send
	quint64 spectrumCountSize_;
};

#endif // AMPTEKSDD123CONFIGURATIONMAP_H
