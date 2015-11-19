#ifndef AMDSNDINDEX_H
#define AMDSNDINDEX_H

#include <QVector>
#include <QString>

class AMDSnDIndex
{
public:
	/// Create an invalid index (rank of 0)
	AMDSnDIndex();
	/// Create an index for 1D data
	AMDSnDIndex(quint32 row);
	/// Create an index for 2D data
	AMDSnDIndex(quint32 row, quint32 col);
	/// Create an index for 3D data
	AMDSnDIndex(quint32 row, quint32 col, quint32 slice);
	/// Create an index for 4D data
	AMDSnDIndex(quint32 i, quint32 j, quint32 k, quint32 l);
	/// Create an index for 5D data
	AMDSnDIndex(quint32 i, quint32 j, quint32 k, quint32 l, quint32 m);

	/// Destructor.
	virtual ~AMDSnDIndex();

	/// Flag to indicate whether the dimension values should be initialized in the following constructor.
	enum InitializeFlag { DoNotInit, DoInit };
	/// Optimized for performance: Construct an index with the initial size of \c rank.  If \c initialize is AMDSnDIndex::Init, all the values are initialized to \c initialValue.
	AMDSnDIndex(quint8 rank, InitializeFlag initialize, quint32 initialValue = 0);


	/// Copy constructor
	AMDSnDIndex(const AMDSnDIndex& other);
	/// Assignment operator
	AMDSnDIndex& operator=(const AMDSnDIndex& other);

	// Constant access functions
	///////////////////////////////////

	/// Returns the rank (# of dimensions that this index has available)
	inline quint8 rank() const { return rank_; }

	/// Is the index valid? Invalid indexes have a rank of 0.
	inline bool isValid() const { return rank_ != 0; }

	/// Return the index for 1st independent axis of the multi-dimensional data set
	inline quint32 i() const { return dims_[0]; }
	/// Return the index for 2nd independent axis of the multi-dimensional data set
	inline quint32 j() const { return dims_[1]; }
	/// Return the index for 3rd independent axis of the multi-dimensional data set
	inline quint32 k() const { return dims_[2]; }
	/// Return the index for 4th independent axis of the multi-dimensional data set
	inline quint32 l() const { return dims_[3]; }

	inline quint32 m() const { return dims_[4]; }
	inline quint32 n() const { return dims_[5]; }
	inline quint32 o() const { return dims_[6]; }
	inline quint32 p() const { return dims_[7]; }


	/// Index-based read-only access:
	inline quint32 at(quint8 dimIndex) const;

	/// Lots of C math / data handling libraries often need a plain array of quint32s to use as multi-dimension indexes. The pointer returned by this function is only valid while the AMDSnDIndex still exists.
	inline const quint32* constData() const;

	// Modification functions
	////////////////////////////////

	/// Index-based modify access:
	inline quint32& operator[](quint8 dimIndex);

	/// Change the size (rank) of the index
	inline void setRank(quint8 newRank);

	/// Set all the dimensions of the index to \c value
	inline void setAll(quint32 value);

	/// Append all the indices from \c another AMDSnDIndex to this one.
	inline void append(const AMDSnDIndex& another);

	// Comparison and other functions
	/////////////////////////////////////

	/// Check to see if another AMDSnDIndex has the same dimensions as this one
	inline bool dimensionsMatch(const AMDSnDIndex& other);

	/// Check to see if another AMDSnDIndex refers to valid index within this one.
	/*! \warning Assumes that \c other is the same rank as us. */
	inline bool inBounds(const AMDSnDIndex& other);

	/// Check to see if this index is valid within the bounds of a multi-dimensional array \c fullSize
	/*! \warning Assumes that \c fullSize is the same rank as us. */
	inline bool validInArrayOfSize(const AMDSnDIndex& fullSize) const;

	/// Comparison operator
	inline bool operator==(const AMDSnDIndex& other) const;

	/// Inequality operator
	inline bool operator!=(const AMDSnDIndex& other) const;

	/// Calculate the product of all the dimensions. (If the AMDSnDIndex represents the size of a multidimensional array, this calculates the total number of elements in that array.)
	inline quint32 product() const;

	/// Calculate the total number of points in the space of the data block between this index and \c toIndex (inclusive).  For example, if this index is (2,2) and \c toIndex = (3,3), the block includes (2,2),(2,3),(3,2),(3,3) and this function would return 4.
	/*! \note Assumes that \c toIndex has the same rank as us, and also that all of its corresponding values are larger than ours. */
	inline quint32 totalPointsTo(const AMDSnDIndex& toIndex) const;

	/// Calculate the location where this index would end up in a flattened (1D) array, where \c fullSize gives the total size of the multi-dimensional array.  (This assumes row-major order, where the first index varies the slowest.)  For example, for a 4-dimensional AMDSnDIndex(3,4,5,600) and an array that has dimensions \c fullSize = (5,10,100,1000), the flat index is 3*10*100*1000 + 4*100*1000 + 5*1000 + 600.
	/*! \note Assumes that \c fullSize has the same rank() as us.*/
	inline quint32 flatIndexInArrayOfSize(const AMDSnDIndex& fullSize) const;

	/// Returns a multidimensional index corresponding to the \c flatIndex in a 1D array , where the multidimensional array has size \c fullSize.    Like flatIndexInArrayOfSize(), this assumes row-major order where the last index varies the fastest.
	static AMDSnDIndex fromFlatIndexInArrayOfSize(const AMDSnDIndex& fullSize, quint32 flatIndex);


	// Overloaded operators: increasing or decreasing an index dimension-by-dimension
	/////////////////////////////

	/// Adds \c other to this index, dimension by dimension.
	/*! \warning Assumes that \c other is the same rank as us. */
	inline AMDSnDIndex& operator+=(const AMDSnDIndex& other);

	/// Returns an index that is the dimension-by-dimension sum of us and \c other.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMDSnDIndex operator+(const AMDSnDIndex& other) const;

	/// Subtracts \c other from this index, dimension by dimension.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMDSnDIndex& operator-=(const AMDSnDIndex& other);

	/// Returns an index that is the dimension-by-dimension subtraction of \c other from this index.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMDSnDIndex operator-(const AMDSnDIndex& other) const;

	/// Adds \c offset to every dimension in this index.
	inline AMDSnDIndex& operator+=(quint32 offset);

	/// Returns an index created by adding \c offset to every dimension of this index.
	inline AMDSnDIndex operator+(quint32 offset) const;

	/// Subtracts \c offset from every dimension in this index.
	inline AMDSnDIndex& operator-=(quint32 offset);

	/// Returns an index created by subtracting \c offset from every dimension of this index.
	inline AMDSnDIndex operator-(quint32 offset) const;


	// Misc.
	//////////////////////////

	/// Print out dimensions: ex: "3 x 7 x 256"
	QString toString(const QString& separator = QString(" x ")) const;

protected:
	/// For ranks less than 9, dimension values are stored in this static array.
	quint32 dims_[8];
	/// The number of dimensions.
	quint8 rank_;
	/// For ranks larger than 9, the remaining dimension values are stored in extras_, but at indexes 8 and higher. This allows a fast implementation of constData(). Note that dims_ is always the authoritative storage location for the first 8 indexes.
	QVector<quint32>* extras_;

};

#include <QMetaType>
Q_DECLARE_METATYPE(AMDSnDIndex)

#include "DataElement/AMDSLikely.h"

quint32 AMDSnDIndex::at(quint8 dimIndex) const {
	if(AMDS_LIKELY(dimIndex < 8))
		return dims_[dimIndex];
	else
		return extras_->at(dimIndex);
}

const quint32* AMDSnDIndex::constData() const {
	if(AMDS_LIKELY(rank_ <= 8))
		return dims_;
	else {
		// copy first 8 elements from dims_ into extras_, before we return extras_. [Most of the time, the first 8 elements of extras_ are ignored/garbage.]
		memcpy(extras_->data(), dims_, 8*sizeof(quint32));
		return extras_->constData();
	}
}

quint32& AMDSnDIndex::operator[](quint8 dimIndex) {
	if(AMDS_LIKELY(dimIndex < 8))
		return dims_[dimIndex];
	else
		return extras_->operator[](dimIndex);
}

void AMDSnDIndex::setRank(quint8 newRank) {
	rank_ = newRank;
	if(rank_ < 9) {
		if(extras_) {
			delete extras_;
			extras_ = 0;
		}
	}
	else {	// rank > 8. need extra space
		if(!extras_)
			extras_ = new QVector<quint32>(rank_);
		else
			extras_->resize(rank_);
	}
}

void AMDSnDIndex::setAll(quint32 value) {
	for(int mu=0; mu<rank_; ++mu)
		operator[](mu) = value;
}

void AMDSnDIndex::append(const AMDSnDIndex& another) {
	quint8 originalRank = rank_;
	quint8 combinedRank = rank_+another.rank_;
	setRank(combinedRank);
	for(quint8 mu=originalRank; mu<combinedRank; mu++)
		operator[](mu) = another.at(mu-originalRank);
}

bool AMDSnDIndex::dimensionsMatch(const AMDSnDIndex& other){
	if( rank_ != other.rank_ )
		return false;
	return true;
}

bool AMDSnDIndex::inBounds(const AMDSnDIndex& other){
	for(quint8 x=0; x < rank_; x++)
		if( other.at(x) >= at(x) )
			return false;
	return true;
}

bool AMDSnDIndex::validInArrayOfSize(const AMDSnDIndex& fullSize) const {
	for(quint8 mu=0; mu<rank_; ++mu)
		if(at(mu) >= fullSize.at(mu))
			return false;
	return true;
}

bool AMDSnDIndex::operator==(const AMDSnDIndex& other) const {
	if(other.rank_ != rank_)
		return false;
	for(quint8 i=0; i<rank_; i++) {
		if(other.at(i) != at(i))
			return false;
	}
	return true;
}

bool AMDSnDIndex::operator!=(const AMDSnDIndex& other) const {
	return !(*this == other);
}

quint32 AMDSnDIndex::product() const {
	quint32 rv = 1;
	for(qint8 mu=rank_-1; mu>=0; --mu)
		rv *= at(mu);
	return rv;
}

quint32 AMDSnDIndex::totalPointsTo(const AMDSnDIndex& toIndex) const {
	quint32 rv = 1;
	for(qint8 mu = rank_-1; mu >= 0; --mu)
		rv *= (toIndex.at(mu) - at(mu) + 1);
	return rv;
}

quint32 AMDSnDIndex::flatIndexInArrayOfSize(const AMDSnDIndex& fullSize) const {
	quint32 rv;

	switch(rank_) {
	case 0:
		rv =  0; break;
	case 1:
		rv = i();
		break;
	case 2:
		rv = i()*fullSize.at(1)
				+ j();
		break;
	case 3: {
		rv = i()*fullSize.at(1)*fullSize.at(2)
				+ j()*fullSize.at(2)
				+ k();
		break; }

	case 4: {
		rv = i()*fullSize.at(1)*fullSize.at(2)*fullSize.at(3)
				+ j()*fullSize.at(2)*fullSize.at(3)
				+ k()*fullSize.at(3)
				+ l();
		break; }

	default: {
		rv = 0;
		for(quint8 mu=0; mu<rank_; ++mu) {
			quint32 multiplier = 1;
			for(quint8 nu=mu+1; nu<rank_; ++nu)
				multiplier *= fullSize.at(nu);
			rv += at(mu)*multiplier;
		}
		break; }
	}

	return rv;
}


AMDSnDIndex& AMDSnDIndex::operator+=(const AMDSnDIndex& other) {
	for(quint8 mu=0; mu<rank_; ++mu)
		(*this)[mu] += other.at(mu);
	return *this;
}

AMDSnDIndex AMDSnDIndex::operator+(const AMDSnDIndex& other) const {
	AMDSnDIndex rv(*this);
	rv += other;
	return rv;
}

AMDSnDIndex& AMDSnDIndex::operator-=(const AMDSnDIndex& other) {
	for(quint8 mu=0; mu<rank_; ++mu)
		(*this)[mu] -= other.at(mu);
	return *this;
}

AMDSnDIndex AMDSnDIndex::operator-(const AMDSnDIndex& other) const {
	AMDSnDIndex rv(*this);
	rv -= other;
	return rv;
}

AMDSnDIndex& AMDSnDIndex::operator+=(quint32 offset) {
	for(quint8 mu=0; mu<rank_; ++mu)
		(*this)[mu] += offset;
	return *this;
}

AMDSnDIndex AMDSnDIndex::operator+(quint32 offset) const {
	AMDSnDIndex rv(*this);
	rv += offset;
	return rv;
}

AMDSnDIndex& AMDSnDIndex::operator-=(quint32 offset) {
	for(quint8 mu=0; mu<rank_; ++mu)
		(*this)[mu] -= offset;
	return *this;
}

AMDSnDIndex AMDSnDIndex::operator-(quint32 offset) const {
	AMDSnDIndex rv(*this);
	rv -= offset;
	return rv;
}

#endif // AMDSNDINDEX_H
