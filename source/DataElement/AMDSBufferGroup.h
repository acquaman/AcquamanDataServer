#ifndef AMDSBUFFERGROUP_H
#define AMDSBUFFERGROUP_H

#include <QObject>

#include "DataElement/AMDSDwellStatusData.h"
#include "DataElement/AMDSBuffer.h"
#include "DataElement/AMDSBufferGroupInfo.h"
#include "DataHolder/AMDSDataHolder.h"


class AMDSClientRequest;
class AMDSClientDataRequest;
class AMDSClientStartTimePlusCountDataRequest;
class AMDSClientRelativeCountPlusCountDataRequest;
class AMDSClientStartTimeToEndTimeDataRequest;
class AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest;
class AMDSClientContinuousDataRequest;

/**
  * Class representing a large buffer of data to store AMDSDataHolders in, of a given size. Once the buffer
  * reaches its maxSize, it will automatically start removing its oldest members. Members are accessible
  * by subscript [int], giving members in order from the oldest added at [0] to the newest at [count()-1].
  */
class AMDSBufferGroup : public QObject
{
	Q_OBJECT
public:
	/// Creates a new buffer group with a maximum capacity maxSize and data dimensions based on the bufferGroupInfo
	AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, bool enableCumulative = false, QObject *parent = 0);
	/// Copy constructor
	AMDSBufferGroup(const AMDSBufferGroup& other);
	~AMDSBufferGroup();

	/// Returns the data dimension and other information included in the bufferGroupInfo
	inline AMDSBufferGroupInfo bufferGroupInfo() const { return bufferGroupInfo_; }

	/// returns whether Cumulative is enabled or not
	inline bool cumulativeEnabled() { return enableCumulative_; }
	/// returns the current dataholder of cumulative data
	AMDSDataHolder *cumulativeDataHolder() const;

	/// returns the dataHolder at given index
	AMDSDataHolder* at(int index);
	/// The number of items currently stored in the buffer (once the buffer reaches maxSize, this will always return maxSize)
	int count() const;
	/// Clears the buffer of all its members, and frees their resources
	void clear();
	/// Adds a new AMDSDataHolder pointer to the end of the buffer. The buffer group takes ownership
	/// of the passed AMDSDataHolder, becoming responsible for its destruction
	void append(AMDSDataHolder* value);

public slots:
	/// Slot which handles a request for data. The buffer group will attempt to populate the request
	/// based on the instructions it includes. When the data request is ready the dataRequestReady signal is emitted
	void processClientRequest(AMDSClientRequest *clientRequest);

signals:
	/// Signal which indicates that a request for data has been processed and is ready to be sent back to the client
	void clientRequestProcessed(AMDSClientRequest *clientRequest);

protected:
	/// Flatten the data based on the given flatten method, return True if no error happened
	bool flattenData(QList<AMDSDataHolder *> *dataArray);
	/// Fills the data to the clientRequest
	void populateData(AMDSClientDataRequest* clientDataRequest, int startIndex, int endIndex);
	/// Fills the request with count number of spectra after (and including) startTime
	void populateData(AMDSClientStartTimePlusCountDataRequest* clientDataRequest);
	/// Fills the request with count number of spectra after (and including) relative count (backward)
	void populateData(AMDSClientRelativeCountPlusCountDataRequest* clientDataRequest);
	/// Fills the request with all data between (and including) startTime and endTime
	void populateData(AMDSClientStartTimeToEndTimeDataRequest* clientDataRequest);
	/// Fills the request with all data acquired from middle time - countBefore to middle time + count after
	void populateData(AMDSClientMiddleTimePlusCountBeforeAndAfterDataRequest* clientDataRequest);
	/// Fills the request with all data acquired after lastFetch
	void populateData(AMDSClientContinuousDataRequest* clientDataRequest);

	/// Helper function that returns the index in the buffer 1 before the given dwellTime, or at the point at
	/// which the given dwellTime would occur within the buffer, if an exact match is not found.
	int getDataIndexByDateTime(const QDateTime& dwellTime);

protected:
	mutable QReadWriteLock lock_;
	/// the buffergroup information about this buffer group
	AMDSBufferGroupInfo bufferGroupInfo_;

	/// the flag to indicate whether we enabled the cumultive feature
	bool enableCumulative_;
	/// the Dataholder to holder the cumulative data
	AMDSDataHolder *cumulativeDataHolder_;

	/// A buffer which contains histogram data collection, sorted by the startDwellTime
	AMDSBuffer<AMDSDataHolder*> dataHolders_;
};

#endif // AMDSBUFFERGROUP_H
