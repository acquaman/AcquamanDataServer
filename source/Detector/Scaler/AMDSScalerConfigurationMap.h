#ifndef AMDSSCALERCONFIGURATIONMAP_H
#define AMDSSCALERCONFIGURATIONMAP_H

#include <QObject>

#include "DataElement/AMDSDataTypeDefinitions.h"

class AMDSScalerConfigurationMap : public QObject
{
    Q_OBJECT
public:
	explicit AMDSScalerConfigurationMap(const QString &scalerName, const QString &basePVName, AMDSDataTypeDefinitions::DataType dataType, const QList<quint8> &configuredChannelIds, QObject *parent = 0);
	~AMDSScalerConfigurationMap();

	/// returns the name of the scaler
	inline QString scalerName() const { return scalerName_; }
	/// returns the base PV name of the scaler
	inline QString scalerBasePVName() const { return scalerBasePVName_; }
	/// returns the datatype of the scaler
	inline AMDSDataTypeDefinitions::DataType dataType() const { return dataType_; }
	/// returns the list of configured channel IDs
	inline QList<quint8> configuredChannels() const { return configuredChannels_; }
	/// returns the list of enabled channel IDs
	inline QList<quint8> enabledChannels() const { return enabledChannels_; }
	/// returns the list of disabled channel IDs
	inline QList<quint8> disabledChannels() const { return disabledChannels_; }

public slots:
	/// slot to enable a CONFIGURED channel
	bool enableChannel(int channelId);
	/// slot to disable a CONFIGURED channel
	bool disableChannel(int channelId);

protected:
	/// the name of the scaler
	QString scalerName_;
	/// the base PV name of the scaler detector
	QString scalerBasePVName_;

	/// the default data type the scaler host server will send to the AMDS
	AMDSDataTypeDefinitions::DataType dataType_;
	/// the configured channel list, the scaler channels which are configured for this given scaler
	QList<quint8> configuredChannels_;

	/// the enabled channel list, which is a subset of the configured channels
	QList<quint8> enabledChannels_;
	/// the disabled channel list, which is a subset of the configured channels
	QList<quint8> disabledChannels_;
};

#endif // AMDSSCALERCONFIGURATIONMAP_H
