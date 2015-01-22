#include "AMDSnDIndex.h"

#include <QStringList>

#include "AMDSLikely.h"

// Create an invalid index (rank of 0)
AMDSnDIndex::AMDSnDIndex() {
	rank_ = 0;
	extras_ = 0;
}

// Create an index for 1D data
AMDSnDIndex::AMDSnDIndex(quint32 row) {
	rank_ = 1;
	extras_ = 0;
	dims_[0] = row;
}

// Create an index for 2D data
AMDSnDIndex::AMDSnDIndex(quint32 row, quint32 col) {
	rank_ = 2;
	extras_ = 0;
	dims_[0] = row;
	dims_[1] = col;
}
// Create an index for 3D data
AMDSnDIndex::AMDSnDIndex(quint32 row, quint32 col, quint32 slice) {
	rank_ = 3;
	extras_ = 0;
	dims_[0] = row;
	dims_[1] = col;
	dims_[2] = slice;
}

// Create an index for 4D data
AMDSnDIndex::AMDSnDIndex(quint32 i, quint32 j, quint32 k, quint32 l) {
	rank_ = 4;
	extras_ = 0;
	dims_[0] = i;
	dims_[1] = j;
	dims_[2] = k;
	dims_[3] = l;
}

// Create an index for 5D data
AMDSnDIndex::AMDSnDIndex(quint32 i, quint32 j, quint32 k, quint32 l, quint32 m) {
	rank_ = 5;
	extras_ = 0;
	dims_[0] = i;
	dims_[1] = j;
	dims_[2] = k;
	dims_[3] = l;
	dims_[4] = m;
}

AMDSnDIndex::AMDSnDIndex(quint8 rank, InitializeFlag initialize, quint32 initialValue)  {
	rank_ = rank;
	if(initialize == DoInit) {
		if(rank_ > 8)
			extras_ = new QVector<quint32>(rank_, initialValue);
		else
			extras_ = 0;

		if(initialValue == 0)
			memset(dims_, 0, 8*sizeof(quint32));
		else
			for(int mu=0;mu<8;++mu)
				dims_[mu] = initialValue;
	}

	else {	// do not init
		if(rank_ > 8)
			extras_ = new QVector<quint32>(rank_);
		else
			extras_ = 0;
	}
}

AMDSnDIndex::AMDSnDIndex(const AMDSnDIndex &other) {
	rank_ = other.rank_;
	memcpy(dims_, other.dims_, 8*sizeof(quint32));

	if(AMDS_UNLIKELY(rank_ > 8))
		extras_ = new QVector<quint32>(*(other.extras_));
	else
		extras_ = 0;
}

AMDSnDIndex::~AMDSnDIndex(){}

AMDSnDIndex& AMDSnDIndex::operator=(const AMDSnDIndex& other) {
	if(AMDS_UNLIKELY(other.rank_ > 8)) {
		if(rank_ > 8)	// already have an extras_ assign from other's.
			*extras_ = *(other.extras_);
		else	// need to make an extras_
			extras_ = new QVector<quint32>(*(other.extras_));
	}

	// need to get rid of our extras_ ?
	if(AMDS_UNLIKELY(rank_ > 8 && other.rank_ <=8)) {
		delete extras_;
		extras_ = 0;
	}

	rank_ = other.rank();
	memcpy(dims_, other.dims_, 8*sizeof(quint32));

	return *this;
}

quint32 AMDSnDIndex::at(int dimIndex) const {
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
	for(quint8 mu=rank_-1; mu>=0; --mu)
		rv *= at(mu);
	return rv;
}

quint32 AMDSnDIndex::totalPointsTo(const AMDSnDIndex& toIndex) const {
	quint32 rv = 1;
	for(quint8 mu = rank_-1; mu >= 0; --mu)
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

AMDSnDIndex AMDSnDIndex::fromFlatIndexInArrayOfSize(const AMDSnDIndex& fullSize, quint32 flatIndex) {
	quint8 rank = fullSize.rank_;
	AMDSnDIndex rv(rank, AMDSnDIndex::DoNotInit);

	switch(rank) {
	case 0:
		break;
	case 1:
		rv[0] = flatIndex;
		break;
	case 2:
		rv[0] = flatIndex / fullSize.j();
		rv[1] = flatIndex % fullSize.j();
		break;
	case 3: {
		quint32 multiplier = fullSize.j()*fullSize.k();
		rv[0] = flatIndex / multiplier;
		flatIndex = flatIndex % multiplier;
		rv[1] = flatIndex / fullSize.k();
		rv[2] = flatIndex % fullSize.k();
		break; }

	case 4: {
		quint32 multiplier = fullSize.j()*fullSize.k()*fullSize.l();
		rv[0] = flatIndex / multiplier;
		flatIndex = flatIndex % multiplier;

		multiplier = fullSize.k()*fullSize.l();
		rv[1] = flatIndex / multiplier;
		flatIndex = flatIndex % multiplier;

		multiplier = fullSize.l();
		rv[2] = flatIndex / multiplier;
		rv[3] = flatIndex % multiplier;
		break; }

	default: {
		for(quint8 mu=0; mu<rank; ++mu) {
			quint32 multiplier = 1;
			for(quint8 nu=mu+1; nu<rank; ++nu)
				multiplier *= fullSize.at(nu);
			rv[mu] = flatIndex / multiplier;
			flatIndex = flatIndex % multiplier;
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

QString AMDSnDIndex::toString(const QString &separator) const {
	QStringList rv;
	for(int i=0; i<rank(); i++)
		rv << QString::number(at(i));
	return rv.join(separator);
}

