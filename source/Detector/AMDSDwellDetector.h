#ifndef AMDSDWELLDETECTOR_H
#define AMDSDWELLDETECTOR_H

#include <QObject>


#define AMDS_DWELL_DETECTOR_ALT_PV_NOT_CONNECTED 50101
#define AMDS_DWELL_DETECTOR_ALT_PV_CONNECTED 50102
#define AMDS_DWELL_DETECTOR_ALT_SWITCH_TO_DWELLING  50103
#define AMDS_DWELL_DETECTOR_ALT_SWITCH_TO_CONFIGURATION  50104
#define AMDS_DWELL_DETECTOR_ALT_DWELL_TIME_CHANGED  50105
#define AMDS_DWELL_DETECTOR_ALT_SCAN_IN_BUFFER_CHANGED  50106
#define AMDS_DWELL_DETECTOR_ALT_TOTAL_NUM_SCANS  50107
#define AMDS_DWELL_DETECTOR_ALT_FAILED_TO_CONFIG_CHANNEL  50108
#define AMDS_DWELL_DETECTOR_ALT_INCORRECT_DWELL_TIME 50109


class AMDSDwellDetector : public QObject
{
    Q_OBJECT
public:
	enum DetectorMode {
		Normal = 0,
		Dwelling = 1
	};

	enum DwellMode {
		SingleShot = 0,
		Continuous = 1
	};

	explicit AMDSDwellDetector(QObject *parent = 0) : QObject(parent)
	{
		initialized_ = false;
		connected_ = false;
		currentScanMode_ = AMDSDwellDetector::Normal;
	}

signals:
	/// signal to indicate that the detector scan mode changed
	void detectorScanModeChanged(int);

protected:
	/// flag to indicate whether all PVs are initialized
	bool initialized_;
	/// flag to indicate whether all PVs are connected
	bool connected_;
	/// the current scan mode of the scaler
	AMDSDwellDetector::DetectorMode currentScanMode_;

};

#endif // AMDSDWELLDETECTOR_H
