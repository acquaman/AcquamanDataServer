#ifndef AMDSCLIENTCONTINUOUSDATAREQUEST_H
#define AMDSCLIENTCONTINUOUSDATAREQUEST_H

#include "AMDSClientDataRequest.h"

#include <QTimer>
#include <QDateTime>
#include <QStringList>
#include <QMap>

class AMDSClientContinuousDataRequest : public AMDSClientDataRequest
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDSClientContinuousDataRequest(QObject *parent = 0);
	explicit AMDSClientContinuousDataRequest(ResponseType responseType, const QString &socketKey, const QStringList &bufferNames, bool includeStatusData, const quint32 updateInterval, const QString &handShakeSocketKey, const AMDSBufferGroupInfo &bufferGroupInfo, QObject *parent = 0);
	virtual ~AMDSClientContinuousDataRequest();
	/// Copy constructor
	AMDSClientContinuousDataRequest(const AMDSClientContinuousDataRequest &other);

	AMDSClientContinuousDataRequest& operator =(const AMDSClientContinuousDataRequest &other);

	/// returns whether this is a continuous message
	virtual bool isContinuousMessage() { return true;}

	/// Sets the socket key identifier
	virtual void setSocketKey(const QString &socketKey);
	/// Returns the list of buffernames
	inline QStringList bufferNames() const { return bufferNameList_; }
	/// Returns the start time for the data request
	inline QDateTime startTime() const { return startTime_; }
	/// Returns the update interval
	inline quint32 updateInterval() const { return updateInterval_; }
	/// Returns the last fetch time for the data request
	inline QDateTime lastFetchTime() const { return lastFetchTime_; }
	/// return the handShaking socket key
	inline QString handShakeSocketKey() const { return handShakeSocketKey_; }
	/// Returns the last handshake time for the data request
	inline QDateTime lastHandShakingTime() const { return lastHandShakeTime_; }
	/// check if it is hand shaking message
	inline bool isHandShakingMessage() { return handShakeSocketKey_.length() > 0; }

	/// Returns the buffer data request message
	inline AMDSClientContinuousDataRequest *bufferDataRequest(QString bufferName) { return bufferDataRequestList_.value(bufferName); }

	/// Returns whether message expired (didn't receive handshake in a given time interval
	bool isExpired();

	/// Sets the buffer names
	void setBufferNames(const QStringList &names);
	/// Sets the start time for the data request
	inline void setStartTime(const QDateTime &startTime) { startTime_ = startTime; }
	/// Sets the updateInterval for the data request
	inline void setUpdateInterval(const quint32 updateInterval) { updateInterval_ = (updateInterval == 0 ? 500 : updateInterval); }
	/// Sets the last fetch time for the data request
	inline void setLastFetchTime(const QDateTime &lastFetchTime) { lastFetchTime_ = lastFetchTime; }
	/// Sets the last handshake time for the data request
	inline void setHandShakeSocketKey(const QString &handShakeSocketKey) { handShakeSocketKey_ = handShakeSocketKey; }
	/// Sets the last handshake time for the data request
	inline void setHandShakeTime(const QDateTime &handShakeTime) { lastHandShakeTime_ = handShakeTime; }

	/// Writes this AMDSClientContinuousRequest to an AMDSDataStream, returns true if no errors are encountered
	virtual bool writeToDataStream(AMDSDataStream *dataStream) const;
	/// Reads this AMDSClientContinuousRequest from the AMDSDataStream, returns true if no errors are encountered
	virtual bool readFromDataStream(AMDSDataStream *dataStream);

	/// start the coninuousRequestTimer for next message
	bool startContinuousRequestTimer();

	/// perform hand shaking with the client
	///    - if the handShaking message contains no interested buffername or interested buffers has no intersection with existing ones,
	///      the current message will be deregistered
	void handShaking(AMDSClientContinuousDataRequest *handShakingMessage);

signals:
	/// SIGNAL of new message requst
	void sendNewContinuousDataRequest(AMDSClientRequest* message);
	/// SIGNAL of task accomplished, the related resources can be removed
	void clientRequestTaskAccomplished(AMDSClientRequest* message);

protected slots:
	/// to handle the timeout message of the timer: to send the new message
	void onDataRequestTimerTimeout();

protected:
	/// the timer of continuous request
	QTimer continuousDataRequestTimer_;

	/// the list of requested buffer names
	QStringList bufferNameList_;

	/// Start time for data
	QDateTime startTime_;
	/// Update interval // default 500ms
	quint32 updateInterval_;
	/// The datetime of last fetch
	QDateTime lastFetchTime_;

	/// The socket key for handshake purpose
	QString handShakeSocketKey_;
	/// The datetime of last handshake
	QDateTime lastHandShakeTime_;

	/// The list of mapped buffer data request
	QMap<QString, AMDSClientContinuousDataRequest *> bufferDataRequestList_;
};

#endif // AMDSCLIENTCONTINUOUSDATAREQUEST_H
