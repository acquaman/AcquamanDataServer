#ifndef AMDSBUFFERGROUP_H
#define AMDSBUFFERGROUP_H

#include <QObject>

#include "source/DataElement/AMDSBuffer.h"
#include "source/DataElement/AMDSBufferGroupInfo.h"
#include "source/DataHolder/AMDSDataHolder.h"

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

	AMDSDataHolder* at(int index) { return dataHolders_[index]; }
	/// Subscript operator for use in lhs assignment (ie b[i] = 5)
	/// Returns the pointer at provided index
//	inline AMDSDataHolder* operator[](int index);
	/// Subscript operator for use in rhs assignment (ie x = b[i])
	/// Returns the pointer at provided index
//	inline const AMDSDataHolder* operator[](int index) const;
	inline AMDSDataHolder *cumulativeDataHolder() {return cumulativeDataHolder_; }
	/// The total capacity of the buffer
	inline quint64 maxSize() const;
	/// Adds a new AMDSDataHolder pointer to the end of the buffer. The buffer group takes ownership
	/// of the passed AMDSDataHolder, becoming responsible for its destruction
	void append(AMDSDataHolder* value);
	/// Clears the buffer of all its members, and frees their resources
	inline void clear();
	/// The number of items currently stored in the buffer (once the buffer reaches maxSize, this will always return maxSize)
	inline int count() const;

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
	/// the flag to indicate whether we enabled the cumultive feature
	bool enableCumulative_;

	mutable QReadWriteLock lock_;
	/// the buffergroup information about this buffer group
	AMDSBufferGroupInfo bufferGroupInfo_;

	/// the Dataholder to holder the cumulative data
	AMDSDataHolder *cumulativeDataHolder_;

	/// A buffer which contains histogram data collection, sorted by the startDwellTime
	AMDSBuffer<AMDSDataHolder*> dataHolders_;
};

quint64 AMDSBufferGroup::maxSize() const
{
	QReadLocker readLock(&lock_);
	return dataHolders_.maxSize();
}

void AMDSBufferGroup::clear()
{
	QWriteLocker writeLock(&lock_);
	for(int iElement = 0, elementCount = dataHolders_.count(); iElement < elementCount; iElement++)
		delete dataHolders_[iElement];

	dataHolders_.clear();

//	if (enableCumulative_)
//		cumulativeDataHolder_->clear();
}

int AMDSBufferGroup::count() const
{
	QReadLocker readLock(&lock_);
	return dataHolders_.count();
}

#endif // AMDSBUFFERGROUP_H
