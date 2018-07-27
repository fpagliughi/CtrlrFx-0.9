/// @file Buffer.h
/// Buffer definitions.

#ifndef __CtrlrFx_Buffer_h
#define __CtrlrFx_Buffer_h

#include <cstring>
#include "CtrlrFx/xtypes.h"

/// Namespace for the Performance Controller Framework
namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A memory management class for optimized I/O operations.

template <typename T> class Buffer
{
private:
	T		*base_,		///< pointer to the underlying memory
			*pos_,		///< The current position
			*lim_,		///< The current size limit
			*end_,		///< One past end of memory
			*mark_;		///< The current data mark

	bool	own_;		///< Whether we own the memory (& should delete it)

protected:
	/// Allocates the requested amount of memory and claims ownership.
	/// @param cap The capacity (in number of items) to allocate
	void alloc(size_t cap);

	/// Deallocates the underlying memory if we own it.
	/// If we own the buffer, this frees it, and sets the base to null.
	void dealloc();

	/// Makes a deep copy of the buffer, including data, position, mark, & limit
	bool deep_copy(const Buffer& rhs);

	/// Copies the data elements
	void copy_elem(T* dest, const T* src, size_t n);

public:
	// ----- Constructor(s)/Destructor -----

	/// Creates an uninitialized buffer.
	/// The buffer does not have any underlying memory area, and must be 
	/// assigned one before it can be used.
	Buffer();

	/// Creates a buffer with the specified capacity.
	/// The buffer allocates a memory area from the heap, and returns it to
	/// the heap when the object is destroyed.
	/// @param cap The capacity of the buffer
	explicit Buffer(size_t cap);

	/// Creates a buffer that operates on the specified memory array.
	/// @param arr The memory for the buffer
	/// @param cap The capacity of the buffer
	/// @param own whether the buffer should take ownership of the underlying 
	///				memory. If so, the array will be deleted when the buffer
	///				is destroyed, so the array @em must have been allocated by
	///				new[].
	Buffer(T* arr, size_t cap, bool own=false);

	/// Creates a buffer that operates on the specified memory array, and 
	/// assumes that it already has some data in it.
	/// @param arr The memory for the buffer
	/// @param pos The starting position
	/// @param cap The capacity of the buffer
	/// @param own whether the buffer should take ownership of the underlying 
	///				memory. If so, the array will be deleted when the buffer
	///				is destroyed, so the array @em must have been allocated by
	///				new[].
	Buffer(T* arr, size_t pos, size_t cap, bool own=false);
	
	/// Creates a new buffer as a deep copy of the specified one.
	Buffer(const Buffer& buf);

	/// The destructor.
	/// This will delete the underlying memory if we own it.
	~Buffer() { dealloc(); }

	// ----- Buffer Manipulation -----

	/// Changes the capacity of the buffer by reallocating memory
	void resize(size_t cap);

	/// Assigns the array as the underlying memory store and assumes that it's empty.
	///
	/// Reconfigures the buffer object to use the array memory, then resets 
	/// the buffer to an empty state.
	/// @param arr pointer to the memory to use
	/// @param cap the number of elements the array can hold
	/// @param own whether the buffer should take ownership of the underlying 
	///				memory. If so, the array will be deleted when the buffer
	///				is destroyed, so the array @em must have been allocated by
	///				new[].
	/// @return Reference to this buffer
	void set(T* arr, size_t cap, bool own=false);

	/// Assigns the array as the underlying memory store and assumes that it 
	/// already contains valid data.
	///
	/// Reconfigures the buffer object to use the array memory, then sets the
	/// buffer position for the specified size.
	/// @param arr pointer to the memory to use
	/// @param pos the starting position (the number of elements currently in
	///				the array)
	/// @param cap the maximum number of elements the array can hold
	/// @param own whether the buffer should take ownership of the underlying 
	///				memory. If so, the array will be deleted when the buffer
	///				is destroyed, so the array @em must have been allocated by
	///				new[].
	/// @return Reference to this buffer
	void assign(T* arr, size_t pos, size_t cap, bool own=false);

	/// Destroys the buffer.
	/// This causes the buffer to release the underlying memory.
	/// If the buffer @em owns the memory, it deletes it, otherwise it just 
	/// sets itself to an uninitialized state. The buffer object can only
	/// be reused if new memory is assigned to it.
	/// @par
	/// The buffer will will be left with a zero capacity.
	void destroy();

	/// Assigment does a deep copy.
	Buffer& operator=(const Buffer& rhs);

	// ----- Queries -----

	/// Gets the size of a single data item
	size_t item_size() const { return sizeof(T); }
	
	/// Gets the maximum number of items that the buffer can hold.
	size_t capacity() const { return end_ - base_; }

	/// Gets the number of bytes this buffer can hold
	size_t byte_capacity() const { return capacity() * sizeof(T); }

	/// Gets the current size
	size_t size() const { return lim_ - base_; }

	/// Sets the size and rewinds the buffer
	Buffer& size(size_t sz) { lim_ = base_ + sz; return *this; } 

	/// Gets the current position.
	/// The position is the number of items that have been read (read mode)
	/// or written (write mode) to the buffer.
	/// @return The position as an offset from the base.
	size_t position() const { return pos_ - base_; }

	/// Gets a pointer to the current position.
	/// This points to the next position where an item should be placed (write
	/// mode) or the next item to be removed (read mode).
	/// @return Pointer to the current position.
	T* position_ptr() const { return pos_; }

	/// Sets the position
	Buffer&	position(size_t pos) { pos_ = base_ + pos; return *this; }

	/// Increments the current position.
	/// This moves the position pointer forwad by a single item.
	Buffer& incr_position() { pos_++; return *this; }

	/// Increments the current position by the specified amount.
	/// This moves the position pointer. If the amount is negative, the 
	/// position is moved back in the buffer.
	/// @param n The number of item positions to move the position. This can 
	///				be negative to move the position backward.
	Buffer& incr_position(int n) { pos_ += n; return *this; }

	/// Gets the current mark
	size_t get_mark() const { return mark_ - base_; }

	/// Gets the number of available items in the buffer for reading or writing.
	/// @return the number of items between the current position and the limit
	size_t available() const { return lim_ - pos_; }

	/// Determines if a single get or put will succeed
	bool has_available() const { return lim_ != pos_; }

	/// Determines if the buffer is full (in write mode)
	bool full() const { return lim_ == pos_; }

	/// Determines if the buffer is full (in read mode)
	bool empty() const { return lim_ == pos_; }

	/// Read-only access to underlying memory
	const T* data() const { return base_; }

	/// Intrinsic conversion operator for read-only access to underlying memory
	//operator const T*() const { return base_; }

	/// Read/Write access to underlying memory
	T* c_array() { return base_; }

	/// Const indexer
	const T& operator[](size_t i) const { return base_[i]; }

	/// Variable indexer
	T& operator[](size_t i) { return base_[i]; }

	/// Fills the remaining portion of the buffer with the specified element
	Buffer& fill(const T& v);

	/// Sets the mark to the current position
	Buffer& mark() { mark_ = pos_; return *this; }

	/// Sets the position to the current mark
	Buffer& reset() { pos_ = mark_; return *this; }

	/// Sets the position & mark to the begining of the buffer
	Buffer& rewind() { pos_ = mark_ = base_; return *this; }

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	Buffer& clear();

	/// Sets the position to the start of the buffer and sets the size.
	/// This essentially empties the buffer and places it into write mode.
	Buffer& clear(size_t sz);

	/// Switches the buffer from write mode to read mode.
	Buffer& flip();

	/// Reserves a number of elements at the end of the buffer
	Buffer& reserve(size_t n) { lim_ = end_ - n; return *this; }

	/// Moves the data to the start of the buffer
	Buffer& compact();
	
	/// Increments the position until it falls on the specified boundary
	Buffer& align(int n);

	// ----- Data I/O -----

	/// Place the data item into the buffer
	/// @todo: Check the limits
	bool put(T v) { *pos_++ = v; return true; }

	/// Copies the array into the buffer
	size_t put(const T arr[], size_t n);

	/// Gets the next item from the buffer
	/// @todo: Check the limits
	T get() { return *pos_++; }

	/// Gets the next item from the buffer.
	/// This checks if there is any data in the buffer, and if so, removes 
	/// the next item and copies it to @em p.
	bool get(T* p);

	/// Copies a block of data from the buffer
	size_t get(T arr[], size_t n);
};

// --------------------------------------------------------------------------

template <typename T>
Buffer<T>::Buffer() : base_(0), pos_(0), lim_(0), end_(0), 
						mark_(0), own_(false)
{
}

template <typename T>
Buffer<T>::Buffer(size_t cap)
{
	base_ = pos_ = mark_ = new T[cap];
	end_ = lim_ = base_ + cap;
	own_ = true;
}

template <typename T>
Buffer<T>::Buffer(T* arr, size_t cap, bool own /*=false*/)
{
	base_ = pos_ = mark_ = arr;
	end_ = lim_ = base_ + cap;
	own_ = own;
}

template <typename T>
Buffer<T>::Buffer(T* arr, size_t sz, size_t cap, bool own /*=false*/)
{
	base_ = arr;
	pos_ = mark_ = arr + sz;	// TODO: make sure sz <= cap
	end_ = lim_ = base_ + cap;
	own_ = own;
}

template <typename T>
Buffer<T>::Buffer(const Buffer& rhs)
{
	alloc(rhs.capacity());
	deep_copy(rhs);
}

// --------------------------------------------------------------------------

template <typename T> 
void Buffer<T>::alloc(size_t cap)
{
	base_ = pos_ = mark_ = new T[cap];
	end_ = lim_ = base_ + cap;
	own_ = true;
}

template <typename T> 
void Buffer<T>::dealloc()
{
	if (own_) {
		delete[] base_;
		base_ = 0;
		own_ = false;
	}
}

template <typename T> 
bool Buffer<T>::deep_copy(const Buffer& rhs)
{
	if (rhs.size() > this->capacity())
		// TODO: throw an exception
		return false;

	pos_	= base_ + rhs.position();
	mark_	= base_ + rhs.get_mark();
	lim_	= base_ + rhs.size();

	copy_elem(base_, rhs.base_, rhs.size());
	return true;
}

template <typename T> 
void Buffer<T>::copy_elem(T* dest, const T* src, size_t n)
{
	while (n--)
		*dest++ = *src++;
}

// --------------------------------------------------------------------------
//						Underlying Memory Manipulation
// --------------------------------------------------------------------------

template <typename T>
void Buffer<T>::resize(size_t cap)
{
	dealloc();
	alloc(cap);
}

template <typename T>
void Buffer<T>::assign(T* arr, size_t sz, size_t cap, bool own /*=false*/)
{
	dealloc();
	base_ = arr;
	pos_ = mark_ = arr + sz;		// TODO: Make sure sz <= cap
	end_ = lim_ = base_ + cap;
	own_ = own;
}

template <typename T> 
inline void Buffer<T>::destroy()
{
	dealloc();
	lim_ = end_ = pos_ = mark_ = 0;
}

template <typename T>
Buffer<T>& Buffer<T>::operator=(const Buffer<T>& rhs)
{
	dealloc();
	alloc(rhs.capacity());
	deep_copy(rhs);
	return *this;
}

// --------------------------------------------------------------------------

template <typename T>
Buffer<T>& Buffer<T>::fill(const T& v)
{
	while (pos_ < lim_)
		*pos_++ = v;
	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::clear()
{
	pos_ = mark_ = base_;
	lim_ = end_;
	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::clear(size_t sz)
{
	if (sz > capacity())
		sz = capacity();

	pos_ = mark_ = base_;
	lim_ = base_ + sz;
	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::flip()
{
	lim_ = pos_;
	pos_ = mark_ = base_;
	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::compact()
{
	size_t n = lim_ - pos_;

	for (size_t i=0; i<n; ++i)
		base_[i] = pos_[i];

	lim_ -= pos_ - base_;
	pos_ = base_;

	return *this;
}

template <typename T>
Buffer<T>& Buffer<T>::align(int n)
{
	int rem = int(pos_ - base_) % n;		
	
	if (rem != 0) {
		T* p = pos_ + (n - rem);
		if (p < end_)
			pos_ = p;
	}
	return *this;
}

// --------------------------------------------------------------------------

template <typename T>
size_t Buffer<T>::put(const T arr[], size_t n)
{
	if (pos_ + n > lim_)
		return 0;

	copy_elem(pos_, arr, n);
	pos_ += n;
	return n;
}

template <typename T>
bool Buffer<T>::get(T* p)
{
	if (pos_ == lim_)
		return false;

	*p = *pos_++;
	return true;
}

template <typename T>
size_t Buffer<T>::get(T arr[], size_t n)
{
	if (pos_ + n > lim_)
		return 0;

	copy_elem(arr, pos_, n);
	pos_ += n;
	return n;
}

/////////////////////////////////////////////////////////////////////////////
// Byte Buffer

/// Template specialization for Buffer<byte>
typedef Buffer<byte> ByteBuffer;

template <> 
inline void ByteBuffer::copy_elem(byte* dest, const byte* src, size_t n)
{
	std::memcpy(dest, src, n);
}

template<>
inline size_t ByteBuffer::byte_capacity() const
{ 
	return capacity();
}

/////////////////////////////////////////////////////////////////////////////
// Char Buffer

/// Template specialization for Buffer<char>
typedef Buffer<char> CharBuffer;

template <> 
inline void CharBuffer::copy_elem(char* dest, const char* src, size_t n)
{
	std::memcpy(dest, src, n);
}

template<>
inline size_t CharBuffer::byte_capacity() const
{ 
	return capacity();
}

/////////////////////////////////////////////////////////////////////////////
// Primitive specializations

typedef Buffer<short> ShortBuffer;

template <> 
inline void ShortBuffer::copy_elem(short* dest, const short* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(short));
}


/// Template specialization for Buffer<ushort>
typedef Buffer<ushort> UShortBuffer;

template <> 
inline void UShortBuffer::copy_elem(ushort* dest, const ushort* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(ushort));
}

/// Template specialization for Buffer<int>
typedef Buffer<int> IntBuffer;

template <> 
inline void IntBuffer::copy_elem(int* dest, const int* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(int));
}

/// Template specialization for Buffer<uint>
typedef Buffer<uint> UIntBuffer;

template <> 
inline void UIntBuffer::copy_elem(uint* dest, const uint* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(uint));
}

/// Template specialization for Buffer<long>
typedef Buffer<long> LongBuffer;

template <> 
inline void LongBuffer::copy_elem(long* dest, const long* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(long));
}

/// Template specialization for Buffer<ulong>
typedef Buffer<ulong> ULongBuffer;

template <> 
inline void ULongBuffer::copy_elem(ulong* dest, const ulong* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(ulong));
}


/// Template specialization for Buffer<float>
typedef Buffer<float> FloatBuffer;

template <> 
inline void FloatBuffer::copy_elem(float* dest, const float* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(float));
}


/// Template specialization for Buffer<double>
typedef Buffer<double> DoubleBuffer;

template <> 
inline void DoubleBuffer::copy_elem(double* dest, const double* src, size_t n)
{
	std::memcpy(dest, src, n*sizeof(double));
}


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Buffer_h

