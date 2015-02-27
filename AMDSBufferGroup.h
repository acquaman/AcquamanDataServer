#ifndef AMDSBUFFERGROUP_H
#define AMDSBUFFERGROUP_H

#include <QObject>

#include "AMDSBuffer.h"
#include "AMDSBufferGroupInfo.h"
#include "AMDSDataHolder.h"

class AMDSClientRequest;
class AMDSClientStartTimePlusCountDataRequest;
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
	AMDSBufferGroup(AMDSBufferGroupInfo bufferGroupInfo, quint64 maxSize, QObject *parent = 0);
	/// Copy constructor
	AMDSBufferGroup(const AMDSBufferGroup& other);

	/// Returns the data dimension and other information included in the bufferGroupInfo
	inline AMDSBufferGroupInfo bufferGroupInfo() const { return bufferGroupInfo_; }

	/// Subscript operator for use in lhs assignment (ie b[i] = 5)
	/// Returns the pointer at provided index
	inline AMDSDataHolder* operator[](int index);
	/// Subscript operator for use in rhs assignment (ie x = b[i])
	/// Returns the pointer at provided index
	inline const AMDSDataHolder* operator[](int index) const;
	/// The total capacity of the buffer
	inline quint64 maxSize() const;
	/// Adds a new AMDSDataHolder pointer to the end of the buffer. The buffer group takes ownership
	/// of the passed AMDSDataHolder, becoming responsible for its destruction
	inline void append(AMDSDataHolder* value);
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
	/// Helper functions which populate request data based on the parameters passed:

	/// Fills the request with count number of spectra after (and including) startTime
	void populateData(AMDSClientStartTimePlusCountDataRequest* clientStartTimePlusCountDataRequest);
	/// Fills the request with all data acquired after lastFetch
	void populateData(AMDSClientContinuousDataRequest* clientContinuousDataRequest, const QDateTime& lastFetch);

//	/// Fills the request with count number of spectra either side of the entry relativeCount ago
//	void populateData(AMDSClientRequest* request, int relativeCount, int count);
//	/// Fills the request with all data between (and including) startTime and endTime
//	void populateData(AMDSClientRequest* request, const QDateTime& startTime, const QDateTime& endTime);
//	/// Fills the request with countBefore entries before middleTime to countAfter entries after.
//	void populateData(AMDSClientRequest* request, const QDateTime& middleTime, int countBefore, int countAfter);

	/// Helper function that returns the index in the buffer 1 before the given dwellTime, or at the point at
	/// which the given dwellTime would occur within the buffer, if an exact match is not found.
	int lowerBound(const QDateTime& dwellTime);

protected:
	AMDSBufferGroupInfo bufferGroupInfo_;
	mutable QReadWriteLock lock_;
	/// A buffer which contains histogram data collection, sorted by the startDwellTime
	AMDSBuffer<AMDSDataHolder*> dataHolders_;
};

quint64 AMDSBufferGroup::maxSize() const
{
	QReadLocker readLock(&lock_);
	return dataHolders_.maxSize();
}

void AMDSBufferGroup::append(AMDSDataHolder *value)
{
	QWriteLocker writeLock(&lock_);
	AMDSDataHolder* dataHolderRemoved = dataHolders_.append(value);
	if(dataHolderRemoved)
		delete dataHolderRemoved;
}

void AMDSBufferGroup::clear()
{
	QWriteLocker writeLock(&lock_);
	for(int iElement = 0, elementCount = dataHolders_.count(); iElement < elementCount; iElement++)
		delete dataHolders_[iElement];

	dataHolders_.clear();
}

int AMDSBufferGroup::count() const
{
	QReadLocker readLock(&lock_);
	return dataHolders_.count();
}

#endif // AMDSBUFFERGROUP_H
