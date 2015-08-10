#ifndef AMDSCLIENTCONTINUOUSWITHBATCHSTREAMSDATAREQUEST_H
#define AMDSCLIENTCONTINUOUSWITHBATCHSTREAMSDATAREQUEST_H

#include "AMDSClientContinuousDataRequest.h"

#include <QStringList>

class AMDSClientContinuousWithBatchStreamsDataRequest : public AMDSClientContinuousDataRequest
{
	Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientContinuousWithBatchStreamsDataRequest(QObject *parent = 0);
	explicit AMDSClientContinuousWithBatchStreamsDataRequest(ResponseType responseType, const QString &socketKey, const QStringList &bufferNames, bool includeStatusData, const quint64 updateInterval, const QString &handShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientContinuousWithBatchStreamsDataRequest();
	/// Copy constructor
	AMDSClientContinuousWithBatchStreamsDataRequest(const AMDSClientContinuousWithBatchStreamsDataRequest &other);

	/// the = opration
	AMDSClientContinuousWithBatchStreamsDataRequest& operator =(const AMDSClientContinuousWithBatchStreamsDataRequest &other);

	/// set the list of interested buffer names
	void setBufferNames(const QStringList &bufferNames);
	/// get the list of interested buffer names
	inline QStringList bufferNames() const { return bufferNamesList; }

	/// Writes this AMDSClientContinuousRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientContinuousRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

protected:
	/// the list of interested buffer names, for ContinuousWithBatchStreams data request, the bufferName() is not valid anymore
	QStringList bufferNamesList;
};

#endif // AMDSCLIENTCONTINUOUSWITHBATCHSTREAMSDATAREQUEST_H
