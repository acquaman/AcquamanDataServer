#ifndef AMDSDETECTORSERVER_H
#define AMDSDETECTORSERVER_H

#include <QObject>
#include <QThread>

#include "Detector/AMDSDwellDetector.h"

class AMDSClientRequest;
class AMDSDetectorServer;

class AMDSDetectorServerManager: public QObject
{
	Q_OBJECT

public:
	AMDSDetectorServerManager(AMDSDetectorServer *detectorServer, QObject *parent = 0);
	~AMDSDetectorServerManager();

	/// forward the configuration request
	void forwardConfigurationRequest(AMDSClientRequest *configurationRequest);

	/// returns the detector server of the current manager
	inline AMDSDetectorServer *detectorServer() { return detectorServer_;}

protected:
	/// the thread instance
	QThread *detectorServerThread_;
	/// the detector server instance
	AMDSDetectorServer *detectorServer_;

};

class AMDSDetectorServer : public QObject
{
    Q_OBJECT

public:
	explicit AMDSDetectorServer(const QString &detectorName, QObject *parent = 0);
	~AMDSDetectorServer();

	/// returns the name of the detector
	inline QString detectorName() const { return detectorName_; }
	/// returns whether the detector is working in dwell mode
	inline bool isDwelling() { return detectorMode_ == AMDSDwellDetector::Dwelling; }

signals:
	/// signal to indicate that the client request is been processed
	void clientRequestProcessed(AMDSClientRequest*);

	/// signal to indicate that the detector server switched to configuration state
	void serverChangedToConfigurationMode(const QString &detectorName);
	/// signal to indicate that the detector server switched to Dwell state
	void serverChangedToDwellMode(const QString &detectorName);
	/// signal to indicate that the detector server is going to switch to dwelling state
	void serverGoingToStartDwelling(const QString &detectorName);

public slots:
	/// the slot to perform the configuration request
	virtual void onConfigurationRequestReceived(AMDSClientRequest *configurationRequest);
	/// helper function to change the working mode fo the detector
	void setDetectorServerMode(AMDSDwellDetector::DwellScanStatus detectorMode);

protected:
	/// the name of the detector
	QString detectorName_;

	/// the mode of the dector
	AMDSDwellDetector::DwellScanStatus detectorMode_;
};

#endif // AMDSDETECTORSERVER_H
