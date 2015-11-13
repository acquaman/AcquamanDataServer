#ifndef AMDSCLIENTCONFIGURATIONREQUEST_H
#define AMDSCLIENTCONFIGURATIONREQUEST_H

#include <QDataStream>
#include <QVariant>

#include "ClientRequest/AMDSClientRequest.h"
#include "DataElement/AMDSCommandManager.h"

class AMDSClientConfigurationRequest : public AMDSClientRequest
{
	Q_OBJECT
public:
	explicit AMDSClientConfigurationRequest(QObject *parent = 0);
	explicit AMDSClientConfigurationRequest(const QString &bufferName, const QString &socketKey, const QString &errorMessage, AMDSClientRequest::ResponseType responseType, const QDateTime &localTime, QObject *parent = 0);
	virtual ~AMDSClientConfigurationRequest();

	/// add a new command value pair
	void appendCommand(int commandId, const QVariant &value);

	/// set the buffername
	inline void setBufferName(const QString &bufferName) { bufferName_ = bufferName; }

	/// returns the bufferName
	inline QString bufferName() const { return bufferName_; }
	/// returns the configuration commands
	inline QMap<int, QVariant> configurationCommands() const { return configurationCommands_; }

protected:
	/// Writes this AMDSClientConfigurationRequest to an QDataStream, returns 0 if no errors are encountered
	virtual int writeToDataStream(QDataStream *dataStream);
	/// Reads this AMDSClientConfigurationRequest from the QDataStream, returns 0 if no errors are encountered
	virtual int readFromDataStream(QDataStream *dataStream);

protected:
	/// the bufferName the request going to be sent to
	QString bufferName_;

	/// the list of the <commandId, value> pair
	QMap<int, QVariant> configurationCommands_;
};

#endif // AMDSCLIENTCONFIGURATIONREQUEST_H
