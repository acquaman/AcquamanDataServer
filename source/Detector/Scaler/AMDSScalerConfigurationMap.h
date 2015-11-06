#ifndef AMDSSCALERCONFIGURATIONMAP_H
#define AMDSSCALERCONFIGURATIONMAP_H

#include <QObject>

#include "DataElement/AMDSDataTypeDefinitions.h"

class AMDSScalerConfigurationMap : public QObject
{
    Q_OBJECT
public:
	explicit AMDSScalerConfigurationMap(const QString &scalerName, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, const QList<quint8> &enabledChannelId, QObject *parent = 0);
	~AMDSScalerConfigurationMap();

	inline const QString scalerName() { return scalerName_; }
	inline const QString scalerBasePVName() { return scalerBasePVName_; }
	inline AMDSDataTypeDefinitions::DataType dataType() { return dataType_; }
	inline const QList<quint8> enabledChannels() { return enabledChannels_; }

protected:
	/// the name of the scaler
	QString scalerName_;
	/// the base PV name of the scaler detector
	QString scalerBasePVName_;

	/// the default data type the scaler host server will send to the AMDS
	AMDSDataTypeDefinitions::DataType dataType_;
	/// the enabled channel list
	QList<quint8> enabledChannels_;

};

#endif // AMDSSCALERCONFIGURATIONMAP_H
