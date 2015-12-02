#ifndef AMDSBUFFER_H
#define AMDSBUFFER_H

#include <QVector>
#include <QReadWriteLock>

/**
  * Templated class which provides automatic AMDSBuffering of classes of type T. Allows for constant time additions to the
  * end, automatic removal of elements at the front once maxSize has been reached, sorting maintained if elements are
  * added at the end in sorted order, constant time random access operators. Note: Interface for AMDSBuffer differs on
  * partially specialized T*. This class represents the generic form of the AMDSBuffer (where T is a value type), for the
  * version where T is a pointer, see below.
  */
template<typename T>
class AMDSBuffer
{
public:
	/// Creates a AMDSBuffer to store maxSize number of elements
	AMDSBuffer(int maxSize) :
		elements_(maxSize, 0),
		maxSize_(maxSize),
		nextPosition_(0),
		totalElementsAdded_(0)
	{
	}

	/// Copy Constructor
	AMDSBuffer(const AMDSBuffer& other) :
		elements_(other.elements_),
		maxSize_(other.maxSize_),
		nextPosition_(other.nextPosition_),
		totalElementsAdded_(other.totalElementsAdded_)
	{
	}

	/// Operator [] which returns non const reference to item at index
	T& operator[](int index)
	{
		QReadLocker readLock(&lock);
		return elements_[convertIndex(index)];
	}

	/// Operator [] which returns const reference to item at index
	const T& operator[](int index) const
	{
		QReadLocker readLock(&lock);
		return const_cast<T&>(elements_[convertIndex(index)]);
	}

	/// Returns const reference to item at index
	const T& at(int index) const
	{
		QReadLocker readLock(&lock);
		return (*this)[index];
	}

	/// The maximum number of elements to be stored in the AMDSBuffer. Once this limit is reached
	/// the AMDSBuffer will automatically pop the oldest items off the front
	int maxSize() const {
		return maxSize_;
	}

	/// Adds the provided value to the back of the AMDSBuffer.
	void append(const T& value){
		QWriteLocker writeLock(&lock);
		elements_[nextPosition_] = value;

		totalElementsAdded_++;
		nextPosition_ = (nextPosition_ +1) % maxSize_;
	}

	/// Removes all items from the AMDSBuffer and, if it owns them, frees their memory
	void clear() {
		QWriteLocker writeLock(&lock);
		totalElementsAdded_ = 0;
		nextPosition_ = 0;
		elements_.clear();
	}

	/// The number of items currently stored in the AMDSBuffer.
	int count() const {
		QReadLocker readLock(&lock);
		return totalElementsAdded_ < maxSize_ ? totalElementsAdded_ : maxSize_;
	}

	/// If the AMDSBuffer currently has no items stored in it
	bool isEmpty() const
	{
		QReadLocker readLock(&lock);
		return totalElementsAdded_ == 0;
	}

protected:

private:
	int convertIndex(int indexIn){
		if(totalElementsAdded_ <= maxSize_)
			return indexIn;
		else
			return (indexIn + nextPosition_) % maxSize_;
	}

private:
	QVector<T> elements_;
	int maxSize_;
	int nextPosition_;
	int totalElementsAdded_;
	mutable QReadWriteLock lock;
};

/**
  * Templated class which provides automatic AMDSBuffering of classes of type T. Allows for constant time additions to the
  * end, automatic removal of elements at the front once maxSize has been reached, sorting maintained if elements are
  * added at the end in sorted order, constant time random access operators. Note: Interface for AMDSBuffer differs on
  * partially specialized T*. This class represents the partially specialized form (where T is T*). In this case the
  * append method is altered to return a pointer to the element removed (so that resouce freeing can be performed by
  * the caller).
  */
template<typename T>
class AMDSBuffer<T*>
{
public:
	/// Creates a AMDSBuffer to store maxSize number of elements
	AMDSBuffer(int maxSize) :
		elements_(maxSize, 0),
		maxSize_(maxSize),
		nextPosition_(0),
		totalElementsAdded_(0)
	{
	}

	/// Copy Constructor
	AMDSBuffer(const AMDSBuffer& other) :
		elements_(other.elements_),
		maxSize_(other.maxSize_),
		nextPosition_(other.nextPosition_),
		totalElementsAdded_(other.totalElementsAdded_)
	{
	}

	/// Operator [] which returns non const reference to item at index
	T* operator[](int index)
	{
		QReadLocker readLock(&lock);
		return elements_[convertIndex(index)];
	}

	/// Operator [] which returns const reference to item at index
	const T* operator[](int index) const
	{
		QReadLocker readLock(&lock);
		return const_cast<T*>(elements_[convertIndex(index)]);
	}

	/// Returns const reference to item at index
	const T* at(int index) const
	{
		QReadLocker readLock(&lock);
		return (*this)[convertIndex(index)];
	}

	void setElementNull(int index) {
		QWriteLocker writeLock(&lock);

		elements_[convertIndex(index)] = 0;
	}

	/// The maximum number of elements to be stored in the AMDSBuffer. Once this limit is reached
	/// the AMDSBuffer will automatically pop the oldest items off the front
	int maxSize() const {
		return maxSize_;
	}

	/// Adds the provided value to the back of the AMDSBuffer, if the AMDSBuffer removes an item to
	/// make way for the new item, then a pointer to the removed item is returned.
	T* append(T* value){
		QWriteLocker writeLock(&lock);
		T* returnElement = 0;

		if(totalElementsAdded_ >= maxSize_)
			returnElement = elements_[nextPosition_];

		elements_[nextPosition_] = value;
		totalElementsAdded_++;
		nextPosition_ = (nextPosition_ +1) % maxSize_;

		return returnElement;
	}

	/// Removes all items from the AMDSBuffer and, if it owns them, frees their memory
	void clear() {
		QWriteLocker writeLock(&lock);

		totalElementsAdded_ = 0;
		nextPosition_ = 0;
		elements_.clear();
	}

	/// The number of items currently stored in the AMDSBuffer.
	int count() const {
		QReadLocker readLock(&lock);
		return totalElementsAdded_ < maxSize_ ? totalElementsAdded_ : maxSize_;
	}

	/// If the AMDSBuffer currently has no items stored in it
	bool isEmpty() const
	{
		QReadLocker readLock(&lock);
		return totalElementsAdded_ == 0;
	}

protected:

private:
	int convertIndex(int indexIn){
		if(totalElementsAdded_ <= maxSize_)
			return indexIn;
		else
			return (indexIn + nextPosition_) % maxSize_;
	}

private:
	QVector<T*> elements_;
	int maxSize_;
	int nextPosition_;
	int totalElementsAdded_;
	mutable QReadWriteLock lock;
};
#endif // AMDSBUFFER_H
