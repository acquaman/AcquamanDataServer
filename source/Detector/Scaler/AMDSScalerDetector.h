#ifndef AMDSSCALERDETECTOR_H
#define AMDSSCALERDETECTOR_H

#include <QObject>
#include <QTimer>

class AMControlSet;
class AMSinglePVControl;
class AMWaveformBinningSinglePVControl;

class AMDSDataHolder;

class AMDSScalerDetector : public QObject
{
	Q_OBJECT

	enum ScanStatus {
		Normal = 0,
		Continuous = 1
	};

public:
	explicit AMDSScalerDetector(const QString &scalerName, const QString &basePVName, const QList<quint8> &enabledChannelIdList, QObject *parent = 0);
	virtual ~AMDSScalerDetector();

	/// returns the name of the scaler
	inline QString scalerName() const { return scalerName_; }
	/// returns the number of channels
	inline quint8 enabledChannelCount() const { return enabledChannelIdList_.count(); }

signals:
	/// signal to indicate that we received new counts data
	void newScalerScanDataReceived(const QList<AMDSDataHolder *> &scanCountsDataHolder);

public slots:
	/// slot to fetch the channel data buffer from the scaler scan control
	void onFetchScanBuffer();

protected slots:
	/// slot to handle the all PV connected signal
	void onAllControlsConnected(bool connected);
	/// slot to handle the PV connected timout signal
	void onAllControlsTimedOut();

	///
	void onDwellTimeControlValueChanged(double);
	void onScansInABufferControlValueChanged(double);
	void onContinuousScanControlValueChanged(double);

	void onStartScalerScanTimer();

protected:
	/// helper function to initialize the PV controls of the scaler
	void initializePVControls();

protected:
	/// the name of the scaler
	QString scalerName_;
	/// the PV baseName
	QString basePVName_;

	/// the list of enabled channel
	QList<quint8> enabledChannelIdList_;

	/// the control set to manager all the controls
	AMControlSet *pvControlSet_;

	/// the PV control to access to the scaler status Control (scanning / stopped )
	AMSinglePVControl *statusControl_;
	/// the PV control to access to the scan mode Control (continuous or normal)
	AMSinglePVControl *continuousScanControl_;
	/// the PV control to access to the dwellTimeControl
	AMSinglePVControl *dwellTimeControl_;
	/// the PV control to access to the number of scans in a single buffer Control
	AMSinglePVControl *scansInABufferControl_;
	/// the PV control to access to the total number of scans Control
	AMSinglePVControl *totalNumberOfScansControl_;

	/// the pv control to access the channel scan array
	AMWaveformBinningSinglePVControl *scanControl_;

	/// the list of PV controls for channel status
	QList<AMSinglePVControl *> channelStatusControlList_;

	/// running status
	/// flag to indicate whether all PVs are connected
	bool connected_;
	/// the dwell time to read the scaler buffer (in ms, default 1ms)
	int dwellTime_;
	/// the number of scans in a single buffer (default 1000)
	int scansInABuffer_;
	/// the total number of scans (default 0)
	int totalNumberOfScans_;

	/// the timer to fetch the scaler scan data
	QTimer* scalerScanTimer_;
};

#endif // AMDSSCALERDETECTOR_H
