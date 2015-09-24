#ifndef AMPTEKSDD123CONFIGURATIONMAP_H
#define AMPTEKSDD123CONFIGURATIONMAP_H

#include <QHostAddress>

/**
  * A class containing all the configuration data relating to a single connection to a
  * detector.
  */
class AmptekSDD123ConfigurationMap : public QObject
{
	Q_OBJECT
public:
		/**
		  * Creates a detector connection configuration from the provided arguments
		  */
	AmptekSDD123ConfigurationMap(const QString &detectorName, const QString &detectorBasePVName, QHostAddress detectorAddress, QHostAddress localAddress, int port, QHostAddress windowsAddress, QObject *parent = 0);
		/**
		 * The host address on the network of the detector
		 */
	QHostAddress detectorAddress() const { return detectorAddress_; }
		/**
		  * The local address used to connect to the detector. Note that a single host address
		  * must be used per detector (though not necessarily network interface)
		  */
	QHostAddress localAddress() const { return localAddress_;}
		/**
		  * The port used to connect to the detector
		  */
	int port() const { return port_;}
		/**
		  *
		  */
	QHostAddress windowsAddress() const { return windowsAddress_;}
		/**
		  * A human readable name used to refer to the detector
		  */
	QString detectorName() const { return detectorName_; }
		/**
		  * The name of the base ProcessVariable that the detector will be accessible through
		  * using the EPICS interface
		  */
	QString detectorBasePVName() const { return detectorBasePVName_;}

protected:
	QHostAddress detectorAddress_;
	QHostAddress localAddress_;
	int port_;
	QHostAddress windowsAddress_;

	QString detectorName_;

	QString detectorBasePVName_;
};

#endif // AMPTEKSDD123CONFIGURATIONMAP_H
