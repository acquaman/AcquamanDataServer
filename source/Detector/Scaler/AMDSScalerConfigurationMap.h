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

	inline QString scalerName() const { return scalerName_; }
	inline QString scalerBasePVName() const { return scalerBasePVName_; }
	inline AMDSDataTypeDefinitions::DataType dataType() const { return dataType_; }
	inline QList<quint8> enabledChannels() const { return enabledChannels_; }

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
