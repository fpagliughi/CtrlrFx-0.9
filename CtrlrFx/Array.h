/// @file Array.h
///
/// Definition of the CtrlrFx Array class.

#ifndef __CtrlrFx_Array_h
#define __CtrlrFx_Array_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// An Array is a collection of items contained in a contiguous memory space.
/// The Array is similar to the std::vector, except that it doesn't grow
/// dynamically. The Array knows its @em capacity, which is the number of 
/// items it can contain. It also keeps a current @em size which can be set
/// to any value up to the capacity. The size is used to communicate the 
/// number of elements in the array, or perhaps, the desired number for an
/// input operation.

template <typename T> class Array
{
protected:
	T		*base_,		///< Pointer to the underlying memory
			*lim_,		///< The current size limit
			*end_;		///< One past end of memory

	bool	own_;		///< Whether we own the memory

protected:
	/// Copies the array and all of its elements into this one.
	/// The size of the other array must be less than or equal to the capacity
	// of this one. This will not grow the array to accomidate the assignment.
	bool deep_copy(const Array& src);

	/// Copies @em n elements from the source  to the destination.
	/// This does an element-by element copy, which can be optimized for
	/// primitive types that copied via a bitwise memcpy.
	void copy_elem(T* dest, const T* src, size_t n);

public:
	Array();
	explicit Array(size_t cap);
	Array(T* arr, size_t cap, bool own=false);
	Array(T* arr, size_t sz, size_t cap, bool own=false);
	Array(const Array& arr);

	~Array();

	/// Gets the maximum number of elements the array can hold.
	/// This is the amount of space allocated for the array.
	size_t capacity() const { return end_ - base_; }

	/// Gets the current size.
	size_t size() const { return lim_ - base_; }

	/// Sets the current size of the array.
	/// This sets the size limit of the buffer. The new size must be less 
	/// than or equal to the current capacity. The contents of the internal
	/// buffer remain unaffected.
	Array& size(size_t sz) { lim_ = base_ + sz; return *this; }

	/// Read-only access to underlying memory
	const T* data() const { return base_; }

	/// Read/Write access to underlying memory
	T* c_array() { return base_; }

	/// Const indexer
	const T& operator[](size_t i) const { return base_[i]; }

	/// Variable indexer
	T& operator[](size_t i) { return base_[i]; }

	/// Const member access w/ size check
	const T& at(size_t i) const;

	/// Fills the array with the sepcified value.
	Array& fill(const T& v);
};

// --------------------------------------------------------------------------

template <typename T>
Array<T>::Array() : base_(0), lim_(0), end_(0), own_(false)
{
}

template <typename T>
Array<T>::Array(size_t cap) : base_(new T[cap]), lim_(base_+cap), 
								end_(base_+cap), own_(true)
{
}

template <typename T>
Array<T>::Array(T* arr, size_t cap, bool own /*=false*/)
			 : base_(arr), lim_(base_+cap), end_(base_+cap), own_(own)
{
}

template <typename T>
Array<T>::Array(T* arr, size_t sz, size_t cap, bool own /*=false*/)
			 : base_(arr), lim_(base_+sz), end_(base_+cap), own_(own)
{
}

template <typename T>
Array<T>::Array(const Array& src)
{
	alloc(src.capacity());
	deep_copy(src);
}

template <typename T>
Array<T>::~Array()
{
	if (own_)
		delete[] base_;
}

// --------------------------------------------------------------------------
//							Protected Members
// --------------------------------------------------------------------------

template <typename T> 
bool Array<T>::deep_copy(const Array& src)
{
	if (src.size() > this->capacity())
		// TODO: throw an exception
		return false;

	lim_ = base_ + src.size();
	copy_elem(base_, src.base_, src.size());
	return true;
}

template <typename T> 
void Array<T>::copy_elem(T* dest, const T* src, size_t n)
{
	while (n--)
		*dest++ = *src++;
}

// --------------------------------------------------------------------------
//								Public Interface
// --------------------------------------------------------------------------

template <typename T>
inline const T& Array<T>::at(size_t i) const
{
    return (i < size()) ? base_[i] : T(0);
}

// --------------------------------------------------------------------------

template <typename T>
Array<T>& Array<T>::fill(const T& v)
{
	T* pos = base_;
	while (pos < lim_)
		*pos++ = v;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Array specializations

template <> 
inline void Array<byte>::copy_elem(byte* dest, const byte* src, size_t n)
{
	memcpy(dest, src, n);
}

template <> 
inline void Array<short>::copy_elem(short* dest, const short* src, size_t n)
{
	memcpy(dest, src, n*sizeof(short));
}

template <> 
inline void Array<ushort>::copy_elem(ushort* dest, const ushort* src, size_t n)
{
	memcpy(dest, src, n*sizeof(ushort));
}

template <> 
inline void Array<int>::copy_elem(int* dest, const int* src, size_t n)
{
	memcpy(dest, src, n*sizeof(int));
}

template <> 
inline void Array<uint>::copy_elem(uint* dest, const uint* src, size_t n)
{
	memcpy(dest, src, n*sizeof(uint));
}

template <> 
inline void Array<long>::copy_elem(long* dest, const long* src, size_t n)
{
	memcpy(dest, src, n*sizeof(long));
}

template <> 
inline void Array<ulong>::copy_elem(ulong* dest, const ulong* src, size_t n)
{
	memcpy(dest, src, n*sizeof(ulong));
}

template <> 
inline void Array<float>::copy_elem(float* dest, const float* src, size_t n)
{
	memcpy(dest, src, n*sizeof(float));
}

template <> 
inline void Array<double>::copy_elem(double* dest, const double* src, size_t n)
{
	memcpy(dest, src, n*sizeof(double));
}

/////////////////////////////////////////////////////////////////////////////
/// Encapsultes an array object and the underlying memory that it requires.
/// This is a convenient way to use an array without allocating dynamic
/// memory off the heap. All memory is local to the block where the array
/// is declared.

template <typename T, size_t N>
class FixedArray : public Array<T>
{
	T arr_[N];

	/// Don't allow re-assignment of underlying memory
	/*
	Array<T>& resize(size_t cap);
	Array<T>& set(T* arr, size_t cap, bool own=false);
	Array<T>& assign(T* arr, size_t cap, size_t sz, bool own=false);
	*/

public:
	FixedArray() : Array<T>(arr_, N, false) {}
	FixedArray(const FixedArray& src);

	FixedArray& operator=(const FixedArray& rhs);
};


// --------------------------------------------------------------------------

template <typename T, size_t N>
inline FixedArray<T,N>::FixedArray(const FixedArray& src) : Array<T>(arr_, N)
{
	deep_copy(src);
}

template <typename T, size_t N>
inline FixedArray<T,N>& FixedArray<T,N>::operator=(const FixedArray& rhs)
{
	deep_copy(rhs);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Array_h


