/// @file CircQueue.h
/// Definition of a teplatized circular queue (FIFO) class.
///
/// @author	Frank Pagliughi
///	@author SoRo Systems, Inc.
///
/// @date	March 2004
///

// COPYRIGHT:
//		Copyright (c) 2001, SoRo Systems, Inc.
//		All Rights Reserved.
//

#ifndef __CtrlrFx_CircQueue_h
#define __CtrlrFx_CircQueue_h

#include "CtrlrFx/xtypes.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
///	A traditional FIFO queue structure with buffer that does NOT grow
///	dynamically. This is useful for data going in or out of an ISR a byte
///	(or word) at a time. The buffer size is determined at run-time,
///	and the underlying memory can be provided by the calling application
///	or it can be allocated off the heap by the object.
///
///	Note that this class is not thread safe, but a few design decisions
///	were made for use in ISR-type situations. The "full" and "empty" flags
///	are implemented as a booleans, rather than comparing pointers, so that
///	a background task can loop on the desired flag without disabling and
///	enabling ISR's.
///
///	The get & put routines never block or wait. They simply fail if the 
///	requested operation can not be completed inmmediately.
///
/// @note This structure IS NOT thread-safe. For multi-thread support, 
/// 		see class @ref MsgQueue
///
/// 

template<typename T> class CircQueue
{
protected:
	T				*base_,		///< The buffer
					*put_,		///< Next slot for "put" 
					*get_;		///< Next item to "get"
	const T			*end_;		///< points to next mem loc after buffer

	bool			own_;		///< Whether we "own" the underlying memory.

	void dealloc();
	void copy_elem(T* dest, const T* src, size_t n);

	T* next(T* p) {
		if (++p == end_)
			p = base_;
		return p;
	}

	// Non-copyable
	CircQueue(const CircQueue&);
	CircQueue& operator=(const CircQueue&);

public:
	/// Construct the shell of a queue.
	/// The queue has no underlying memory and can not be used until it is 
	/// given memory by calling @ref resize or @ref set.
	CircQueue();

	/// Creates a queue with the specified capacity.
	/// @param cap The capacity, in number of items, that the queue can hold
	explicit CircQueue(size_t cap);

	/// Creates a queue using the provided memory.
	/// @param arr Memory array to hold data
	/// @param cap The number of elements the memory can contain.
	/// @param own Whether we are given ownership of the memory and should 
	/// 			delete it on destruction.
	CircQueue(T* arr, size_t cap, bool own=false);

	/// Destructor.
	~CircQueue();

	/// Gets a pointer to the underlying memory.
	T* c_array() { return base_; }

	/// Resize the queue.
	/// This deallocates the underlying memory, if we own it, and allocates
	/// new memory for the queue. It does not transfer the contents of the
	/// queue and is not meant as a way to dynamically "grow" the queue.
	/// After resizing, the queue is empty.
	/// @param cap The new capacity of the queue, in number of items.
	void resize(size_t cap);

	/// Resassigns the underlying memory to the aray provided.
	/// This deallocates the current memory, if we own it, and sets up to use 
	/// the array provided. After setting the new memory the queue is empty.
	/// @param arr Memory array to hold data
	/// @param cap The number of elements the memory can contain.
	/// @param own Whether we are given ownership of the memory and should 
	/// 			delete it on destruction.
	void set(T* arr, size_t cap, bool own=false);

	/// Deallocates the memory used by the queue.
	/// After it's destroyed, the queue can't be used unless it is given new
	/// memory by calling @ref resize or @ref set.
    void destroy();

	/// Gets the number of items currently in the queue.
	size_t size() const;

	/// Gets the max number of items the queue can hold.
	size_t capacity() const { return end_ - base_; }

	/// Determines if the queue is currently full.
	bool full() const { return next(put_) == get_; }

	/// Determines if the queue is currently empty.
	bool empty() const { return put_ == get_; }

	/// Gets the number of items currently in the queue.
	size_t available() const { return size(); }

	/// Gets the number of open slots remaining in the queue.
	/// This compensates for the single, unavailable slot used for pointer
	/// comparisons. With an empty queue, this will return capacity-1.
	size_t remaining() const { return capacity() - size() - 1; }

	/// Places an item into the queue.
	bool put(const T& v);

	/// Places an array of items into the queue.
	size_t put(const T buf[], size_t n);

	/// Retrieves the next item from the queue.
	T get();

	/// Attempts to retrieve the next item from the queue.
	bool get(T *p);

	/// Retrieves the next @em n items from the queue. 
	size_t get(T buf[], size_t n);

	// ----- For Compatibility w/ MsgQueue -----
 
	bool tryput(const T& v)	{ return put(v); }
	bool tryget(T *p)		{ return get(p); }
};


// --------------------------------------------------------------------------
//					Template Member Definitions
// --------------------------------------------------------------------------

template<typename T> inline CircQueue<T>::CircQueue() : own_(false)
{
	destroy();		// Resets the member vars
}

template<typename T> inline CircQueue<T>::CircQueue(size_t cap)
								: own_(false)
{
	resize(cap);
}

template <typename T> inline CircQueue<T>::CircQueue(T* buf, size_t cap,
													 bool own /*=false*/)
								: own_(false)						
{
	set(buf, cap, own);
}

template <typename T> inline CircQueue<T>::~CircQueue()
{
	if (own_)
		delete[] base_;
}

// --------------------------------------------------------------------------

template <typename T> void CircQueue<T>::dealloc()
{
	if (own_) {
		delete[] base_;
		base_ = 0;
		own_ = false;
	}
}

// --------------------------------------------------------------------------
// "Resizes" the internal buffer by throwing away the old one and allocating
// a new one. The object is set to an initial state. No data is coped to the
// new buffer.

template <typename T> void CircQueue<T>::resize(size_t cap)
{
	if (cap == 0)
		destroy();
	else
		set(new T[cap], cap, true);
}

// --------------------------------------------------------------------------
// Sets the internal buffer to the memory specified by 'buf' 

template<typename T> void CircQueue<T>::set(T *buf, size_t cap, 
													bool own /*=false*/)
{
	if (cap == 0) {
		destroy();
		return;
	}

	dealloc();

	base_ = put_ = get_ = buf;
	end_ = base_ + cap;
	own_ = own;

	#if defined(_DEBUG)
		memset(buf, 0, cap * sizeof(T));
	#endif
}

// --------------------------------------------------------------------------
// Destroys the object by freeing any dynamic memory we own and setting the
// object back to an initial state.
//

template<typename T> void CircQueue<T>::destroy()
{
	dealloc();
	
	base_ = put_ = get_ = 0;
    end_ = 0;
}

// --------------------------------------------------------------------------

template <typename T> size_t CircQueue<T>::size() const
{
	int sz = put_ - get_;
	if (sz < 0)
		sz += capacity();

	return (size_t) sz;
}

// --------------------------------------------------------------------------
// Copies @em n elements from the source to the destination.

template <typename T> 
void CircQueue<T>::copy_elem(T* dest, const T* src, size_t n)
{
	while (n--)
		*dest++ = *src++;
}

// --------------------------------------------------------------------------
// Attempts to place a single value into the queue. It will fail if the 
// queue is currently full.

template<typename T> bool CircQueue<T>::put(const T& v)
{
	if (full())
		return false;

	*put_ = v;
	put_ = next(put_);
	return true;
}

// --------------------------------------------------------------------------
// Attempts to write the contents of 'buf' into the queue. If there's not
// enough room, it will push as many elements as possible.

template<typename T> size_t CircQueue<T>::put(const T buf[], size_t n)
{
	n = min(n, remaining());

	size_t nwrap = size_t(end_ - put_);

	if (n < nwrap) {
		copy_elem(put_, buf, n);
		put_ += n;
	}
	else {
		size_t nrem = n - nwrap;

		copy_elem(put_, buf, nwrap);
		copy_elem(base_, buf+nwrap, nrem);

		put_ = base_ + nrem;		// Wraps put for nwrap == n, too
	}

	return n;
}

// --------------------------------------------------------------------------
// Removes a single item from the queue and returns it. If the queue is 
// empty returns a zero (in the type specified for queue elements).

template<typename T> T CircQueue<T>::get()
{
	if (empty())
		return T(0);

	T v = *get_;
	get_ = next(get_);
	return v;
}

// --------------------------------------------------------------------------
// Removes a single item from the queue and places it in the memory pointed
// to by 'p'  The routine fails if there is no data element currently 
// available.

template<typename T> bool CircQueue<T>::get(T *p)
{
	if (empty())
		return false;

	*p = *get_;
	get_ = next(get_);
	return true;
}

// --------------------------------------------------------------------------
// Removes up to 'n' items from the queue and places them into 'buf'
// If there are fewer then 'n' items currently available they will all be
// obtained.
//
// RETURNS:
//		The number of items removed from the queue (could be <= 'n')
//

template<typename T> size_t CircQueue<T>::get(T buf[], size_t n)
{
	if (empty())
		return 0;

	n = min(n, size());

	size_t nwrap = size_t(end_ - get_);

	if (n < nwrap) {
		copy_elem(buf, get_, n);
		T* p = get_ + n;
		get_ = p;
	}
	else {
		size_t nrem = n - nwrap;
		copy_elem(buf, get_, nwrap);
		copy_elem(buf+nwrap, base_, nrem);

		get_ = base_ + nrem;
	}

	return n;
}

// --------------------------------------------------------------------------
//						Optimizations for Byte Data
// --------------------------------------------------------------------------
// Attempt to write the contents of 'buf' into the queue. If there's not
// enough room, it will push as many elements as possible.

template <>
size_t CircQueue<byte>::put(const byte buf[], size_t n)
{
	n = min(n, remaining());

	size_t nwrap = size_t(end_ - put_);

	if (n < nwrap) {
		memcpy(put_, buf, n);
		T* p = put_ + n;
		put_ = p;
	}
	else if (n == nwrap) {
		memcpy(put_, buf, n);
		put_ = base_;
	}
	else {
		size_t nrem = n - nwrap;

		memcpy(put_, buf, nwrap);
		memcpy(base_, buf+nwrap, nrem);
		put_ = base_ + nrem;
	}

	return n;
}

// --------------------------------------------------------------------------
// Removes up to 'n' items from the queue and places them into 'buf'
// If there are fewer then 'n' items currently available they will all be
// obtained.

template <>
size_t CircQueue<byte>::get(byte buf[], size_t n)
{
	if (empty_)
		return 0;

	n = min(n, size());

	size_t nwrap = size_t(end_ - get_);

	if (n < nwrap) {
		memcpy(buf, get_, n);
		T* g = get_ + n;
		get_ = g;
	}
	else if (n == nwrap) {
		memcpy(buf, get_, n);
		get_ = base_;
	}
	else {
		size_t nrem = n - nwrap;

		memcpy(buf, get_, nwrap);
		memcpy(buf+nwrap, base_, nrem);
		get_ = base_ + nrem;
	}

	return n;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_CircQueue_h

