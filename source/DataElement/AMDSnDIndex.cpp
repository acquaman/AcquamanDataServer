#include "DataElement/AMDSnDIndex.h"

#include <QStringList>

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

QString AMDSnDIndex::toString(const QString &separator) const {
	QStringList rv;
	for(int i=0; i<rank(); i++)
		rv << QString::number(at(i));
	return rv.join(separator);
}

