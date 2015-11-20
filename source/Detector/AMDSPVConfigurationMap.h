#ifndef AMDSPVCONFIGURATIONMAP_H
#define AMDSPVCONFIGURATIONMAP_H

#include <QObject>

#include "DataElement/AMDSDataTypeDefinitions.h"

class AMDSPVConfigurationMap : public QObject
{
    Q_OBJECT
public:
	explicit AMDSPVConfigurationMap(const QString &enabled, const QString &name, const QString &description, const QString &epicsPV, AMDSDataTypeDefinitions::DataType dataType, QObject *parent = 0)
		:QObject(parent)
	{
		enabled_ = (enabled.toLower() == "true");

		pvName_ = name;
		pvDescription_ = description;
		epicsPV_ = epicsPV;
		dataType_ = dataType;
	}

	/// returns whether this PV is enabled or not
	inline bool enabled() const { return enabled_; }
	/// returns the display name of the PV
	inline QString pvName() const { return pvName_; }
	/// returns the description of the PV
	inline QString pvDescription() const { return pvDescription_; }
	/// returns the epics PV of the PV
	inline QString epicsPV() { return epicsPV_; }
	/// returns the dataType of the PV
	inline AMDSDataTypeDefinitions::DataType dataType() { return dataType_; }

protected:
	/// whether this PV is enabled for tracking or not
	bool enabled_;
	/// the display name of the PV
	QString pvName_;
	/// the description of the PV
	QString pvDescription_;
	/// the epics PV of the PV
	QString epicsPV_;
	/// the dataType of the PV
	AMDSDataTypeDefinitions::DataType dataType_;
};

#endif // AMDSPVCONFIGURATIONMAP_H
