#ifndef AMPTEKSDD123HISTOGRAM_H
#define AMPTEKSDD123HISTOGRAM_H

#include <QReadWriteLock>
#include "AmptekEventDefinitions.h"
#include "Buffer.h"

class AMDSClientDataRequest;
/**
  * A class representing the data returned from a single detector response, contains the spectrum data, as well
  * as some status data for the detector relating to the spectrum data.
  */
class AmptekSDD123Histogram : public QObject
{
Q_OBJECT
public:

		/// Creates a new instance of a data histogram using the provided spectrum and status data
		AmptekSDD123Histogram(const QVector<int> &spectrum = QVector<int>(), const AmptekStatusData &statusData = AmptekStatusData(), QObject *parent = 0);

		/// The spectrum data relating to this data item. A QVector<int> of size 1024
	QVector<int> spectrum() const;

		/// The status data relating to the detector response
	AmptekStatusData statusData() const;

		/// The time the detector starting collecting data for this paricular spectrum
	QTime dwellStartTime() const;
		/// The time the detector finished collecting data for this particular spectrum
		QTime dwellEndTime() const;
		/// The time taken after the detector finished collecting data for the server to receive it
	QTime dwellReplyTime() const;
		/// Some operator overloads which allow the histogram to be compared to a QTime. Allows for ease
		/// of doing searches for histograms based on time. Comparison is taken against the dwellStartTime
		inline bool operator <(const QTime& rhs);
		inline bool operator >(const QTime& rhs);
		inline bool operator ==(const QTime& rhs);
public slots:
		/// Sets the spectrum data for the histogram
	void setSpectrum(const QVector<int> &spectrum);
		/// Sets the status data for the histogram
	void setStatusData(const AmptekStatusData &statusData);

protected:

	QVector<int> spectrum_;
	AmptekStatusData statusData_;
};

/**
  * A class which represents a collection of histograms relating to one particular period of data collection
  */
class AmptekSDD123DwellHistogramGroup : public QObject
{
	Q_OBJECT
public:
		/// Creates an empty, uninitialized DwellHistogramGroup
		explicit AmptekSDD123DwellHistogramGroup(QObject *parent = 0);
		/// Copy Constructor
		AmptekSDD123DwellHistogramGroup(const AmptekSDD123DwellHistogramGroup& other);
		/// Adds a new histogram to the end of the group, also recalculates the cumulative and
		/// average data
		void append(AmptekSDD123Histogram *histogram);

		/// Returns the single histogram at the given index of the group
		AmptekSDD123Histogram* at(int index) const;

		/// Empties the histogram group of all its data
	void clear();

		/// The number of histograms stored within the group
	int count() const;

		/// A total of the spectrum data collected for this particular group
		QVector<int> cumulativeSpectrum() const;
		/// A total of the status data collected for this particular group
		AmptekStatusData cumulativeStatusData() const;

		/// An averate of the spectrum data collected for this particular group
		QVector<int> averageSpectrum() const;
		/// An averate of the status data collected for this particular group
		AmptekStatusData averageStatusData() const;

		/// Overload of assignment operator. Performs deep copy of other into this
		AmptekSDD123DwellHistogramGroup& operator =(const AmptekSDD123DwellHistogramGroup& other);

protected:
		QList<AmptekSDD123Histogram*> histograms_;

	QVector<int> cumulativeSpectrum_;
	AmptekStatusData cumulativeStatusData_;


	bool initialized_;
};

/**
  * Class representing a large buffer of data to store histograms in, of a given size. Once the buffer
  * reaches its maxSize, it will automatically start removing its oldest members. Members are accessible
  * by subscript [int], giving members in order from the oldest added at [0] to the newest at [count()-1].
  */
class AmptekSDD123BufferedHistogramGroup : public QObject
{
	Q_OBJECT
public:
	/// Creates a new buffered histogram group with a maximum capacity maxSize
	explicit AmptekSDD123BufferedHistogramGroup(int maxSize, QObject *parent = 0);
	/// Copy constructor
	AmptekSDD123BufferedHistogramGroup(const AmptekSDD123BufferedHistogramGroup& other);

//    /// Subscript operator for use in lhs assignment (ie b[i] = 5)
//    /// Returns the pointer at provided index
//    inline AmptekSDD123Histogram* operator[](int index);
//    /// Subscript operator for use in rhs assignment (ie x = b[i])
//    /// Returns the pointer at provided index
//    inline const AmptekSDD123Histogram* operator[](int index) const;
	/// The total capacity of the buffer
	int maxSize() const;
	/// Adds a new histogram pointer to the end of the buffer. The buffered histogram group takes ownership
	/// of the passed histogram, becoming responsible for its destruction
	void append(AmptekSDD123Histogram* value);
	/// Clears the buffer of all its members, and frees their resources
	void clear();
	/// The number of items currently stored in the buffer (once the buffer reaches maxSize, this will
	/// always return maxSize)
	const int count() const;

public slots:
	/// Slot which handles a request for data. The Histogram group will attempt to populate the request
	/// based on the instructions it includes. When the data request is ready the dataRequestReady signal is emitted
	void requestData(AMDSClientDataRequest* request);
signals:
	/// Signal which indicates that a request for data has been processed and is ready to be sent back to the client
	void dataRequestReady(AMDSClientDataRequest* request);
protected:
private:
	mutable QReadWriteLock lock_;
	/// A buffer which contains histogram data collection, sorted by the startDwellTime
	Buffer<AmptekSDD123Histogram*> histograms_;

	/// Helper functions which populate request data based on the parameters passed:
	/// Fills the request with all data acquired after lastFetch
	void populateData(AMDSClientDataRequest* request, const QTime& lastFetch);
	/// Fills the request with count number of spectra after (and including) startTime
	void populateData(AMDSClientDataRequest* request, const QTime& startTime, int count);
	/// Fills the request with count number of spectra either side of the entry relativeCount ago
	void populateData(AMDSClientDataRequest* request, int relativeCount, int count);
	/// Fills the request with all data between (and including) startTime and endTime
	void populateData(AMDSClientDataRequest* request, const QTime& startTime, const QTime& endTime);
	/// Fills the request with countBefore entries before middleTime to countAfter entries after.
	void populateData(AMDSClientDataRequest* request, const QTime& middleTime, int countBefore, int countAfter);
	/// Helper function that returns the index in the buffer 1 before the given dwellTime, or at the point at
	/// which the given dwellTime would occur within the buffer, if an exact match is not found.
	int lowerBound(const QTime& dwellTime);
};

#endif // AMPTEKSDD123HISTOGRAM_H
