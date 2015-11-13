#ifndef AMDSDETECTORSERVER_H
#define AMDSDETECTORSERVER_H

#include <QObject>
#include <QThread>

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
	enum DetectorMode {
		Dwell = 0,
		Configuration
	};

    explicit AMDSDetectorServer(QObject *parent = 0);
	~AMDSDetectorServer();

	/// helper function to change the working mode fo the detector
	void setDetectorMode(AMDSDetectorServer::DetectorMode detectorMode);

	/// returns whether the detector is working in dwell mode
	inline bool isDwelling() { return detectorMode_ == AMDSDetectorServer::Dwell; }

signals:
	/// signal to indicate that the detector server switched to configuration state
	void serverChangedToConfigurationState();
	/// signal to indicate that the detector server switched to Dwell state
	void serverChangedToDwellState();
	/// signal to indicate that the detector server is going to switch to dwell state
	void serverGoingToStartDwelling();

public slots:
	/// the function to perform the configuration request
	virtual void onConfigurationRequestReceived(AMDSClientRequest *configurationRequest);

protected:
	/// the mode of the dector
	AMDSDetectorServer::DetectorMode detectorMode_;

};

#endif // AMDSDETECTORSERVER_H
