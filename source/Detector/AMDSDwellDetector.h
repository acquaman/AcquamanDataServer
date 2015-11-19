#ifndef AMDSDWELLDETECTOR_H
#define AMDSDWELLDETECTOR_H

#include <QObject>

class AMDSDwellDetector : public QObject
{
    Q_OBJECT
public:
	enum DwellScanStatus {
		Configuration = 0,
		Dwelling = 1
	};

	explicit AMDSDwellDetector(QObject *parent = 0) : QObject(parent)
	{
		initialized_ = false;
		connected_ = false;
		currentScanMode_ = AMDSDwellDetector::Configuration;
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
	AMDSDwellDetector::DwellScanStatus currentScanMode_;

};

#endif // AMDSDWELLDETECTOR_H
