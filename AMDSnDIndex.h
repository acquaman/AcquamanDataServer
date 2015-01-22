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
	inline quint32 at(int dimIndex) const;

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


#endif // AMDSNDINDEX_H
