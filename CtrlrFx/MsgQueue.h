/// @file MsgQueue.h
/// Class definition of a thread-safe queue. 
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// @date March 2004
///
/// COPYRIGHT:
///		Copyright (c) 2004-2007, SoRo Systems, Inc.
///		All Rights Reserved.
///

#ifndef __CtrlrFx_MsgQueue_h
#define __CtrlrFx_MsgQueue_h

#include "CtrlrFx/os.h"
#include "CtrlrFx/Guard.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A class for passing messages between threads.
/// This is a thread-protected circular queue.
///
/// @par
/// Class defines a FIFO queue structure with thread synchronization.
///	Data is copied into and out of the queue by value (NOT by 
///	reference). Thus, this is normally used for small, built-in types.
///
/// @par
///	This is static queue for sharing data between threads. It's similar
///	in concept to the Message Queue in most RTOS's, but provides a 
///	consistent implementation across platforms using semaphores for
///	thread synchronization, and considerable flexibility in where the
///	memory for the buffer is obtained.
///
/// @par
///	The queue is static in the sense that it does not grow or shrink as
///	data is pushed or popped. If the queue is full, the calling thread is
///	blocked when it attempts to put an item. The size of the buffer can
///	be changed with set_buffer(), but any existing data is discarded and
///	a new, empty buffer is allocated. set_buffer() is also not thread safe
/// and can only be used when the queue is off-line.
///
/// @par
///	This version uses an underlying buffer provided by the user or 
///	allocated automatically off the heap. Use MtFixedQueue<T,N> for
///	a similar structure that contains the buffer internally, thus
///	placing it at the point of 

template<typename T, typename LockType=Mutex> class MsgQueue
{
	typedef Guard<LockType> MyGuard;

	mutable LockType lock_;	///< The lock for the collection

	volatile size_t	sz_;	///< The current # elements

	T			*buf_,		///< The underlying memory buffer

				*put_,		///< Next item to put
				*get_;		///< Next item to get

	const T		*end_;		///< Points to next mem loc after buffer

	bool		own_;		///< Whether we "own" the buf (and should delete)

	Semaphore	slotSem_,	///< Remaining, empty, slots
				dataSem_;	///< Availible data element

	T*		next(T* p);
	bool	do_put(const T& v);
	bool	do_get(T *p);

	// Non-copyable
	MsgQueue(const MsgQueue&);
	MsgQueue& operator=(MsgQueue&);

public:
	/// Creates an empty queue, with no memory store.
	/// The queue must be given a buffer with a call to resize() or set_buffer()
	/// before it can be used.
	MsgQueue();

	/// Creates a queue with the specified capacity.
	MsgQueue(size_t cap);

	/// Creates a queue and uses the array provided for the underlying memory.
	MsgQueue(T* arr, size_t cap, bool own=false);

	/// Destroys the queue and releases any memory that it owns.
	~MsgQueue();

	/// Resizes the queue to the new capacity.
	/// This reallocates the memory for the queue. If the underlying memory 
	/// is owned by the queue it is released and re-allocated. Any messages
	/// in the queue are lost.	
	/// @note *** This routine is not thread safe ***
	/// @par It should not be called on an object that is being used by the system.
	///
	/// @param cap The new capacity for the queue (i.e. the maximum number of
	///				items that it can hold.
	void resize(size_t cap);

	/// Sets the 
	/// This reallocates the memory for the queue. If the underlying memory 
	/// is owned by the queue it is released and re-allocated. Any messages
	/// in the queue are lost.
	/// @param cap The new capacity for the queue (i.e. the maximum number of
	///				items that it can hold.
	void set_buffer(T* buf, size_t cap, bool own=false);

	/// Destroys the queue and frees all the memory that it owns. 
	/// The object can be used again by calling set_buffer().
	///
	/// @note *** This routine is not thread safe ***
	/// @par
	/// It should not be called on a queue that is currently being used by the system.
	void destroy();

	/// Gets the number of items currently contained in the queue.
	size_t size() const;

	/// Gets the maximum number of items the queue can hold.
	size_t capacity() const	{ return end_ - buf_; }

	/// Determines if the queue is currently full
	bool full() const { return size() == 0;  }

	/// Determines if the queue is currently full
	bool empty() const { return remaining() == 0; }

	/// Gets the number of items in the queue.
	size_t available() const;

	/// Gets the number of empty slots remaining.
	size_t remaining() const;

	/// Releases a thread waiting on the queue
	void release() { put(T()); }

	/// Places an item into the queue.
	/// This will block if the queue is full and wait for a slot to open
	/// up to insert the message. It will not return until the message
	/// is delivered to the queue
	/// @param v the item to place in the queue
	void put(const T& v);

	/// Tries to place an item into the queue, and waits a bounded amount
	/// of time if the queue is currently full.
	/// This will attempt to place a message in the queue
	/// @param v The item to place in the queue.
	/// @param d The amount of time to wait.
	/// @return
	/// @li @em true if the item is successfully placed in the queue
	/// @li @em false if the buffer is full and a timeout occurs
	bool put(const T& v, const Duration& d);

	/// Attempts to place an item in the queue.
	/// This will try to place the item in the queue. If the queue is 
	/// currently full, the routine will fail immediately without blocking.
	/// @param v the value to place in the queue
	/// @return
	/// @li @em true if the item is successfully placed in the queue
	/// @li @em false if the buffer is full and the item was not inserted
	bool tryput(const T& v);

	/// Returns the next available item from the queue
	/// If the queue is empty this will block until another thread puts an 
	/// item in the queue.
	T get();

	/// Gets the next item from the queue
	void get(T *p);

	/// Tries to get an item from the queue, and waits a bounded amount
	/// of time if the queue is currently empty.
	/// This will attempt to get a message from the queue.
	/// @param p A pointer to the memory that will receive the object
	/// @param d The time to wait for an item if the queue is empty
	///
	/// @return
	/// @li @em true if the item is successfully retrieved from the queue
	/// @li @em false if the queue is empty and a timeout occured
	bool get(T *p, const Duration& d);

	/// Attempts to get an item in the queue.
	/// This will try to get an item from the queue. If the queue is 
	/// currently empty, the routine will fail immediately without blocking.
	/// @param p pointer to the memory that will get the item
	/// @return
	/// @li @em true if the item is successfully retrieved from the queue
	/// @li @em false if the queue is empty and no item was retrieved
	bool tryget(T *p);

	/// Gets the next item in the queue without removing it.
	/// Tries to get the next item in the queue without removing it. If the
	/// queue is currently empty it returns immediately without blocking.
	/// @return
	/// @li true if there was an item waiting.
	/// @li false if the queue was empty
	bool peek(T *p);
};

// --------------------------------------------------------------------------

template<typename T, typename LockType>
MsgQueue<T,LockType>::MsgQueue() : sz_(0), own_(false)
{
}

template<typename T, typename LockType>
MsgQueue<T,LockType>::MsgQueue(size_t cap) : own_(false)
{
	resize(cap);
}

template<typename T, typename LockType>
MsgQueue<T,LockType>::MsgQueue(T* arr, size_t cap, bool own /*=false*/) : own_(false)
{
	set_buffer(arr, cap, own);
}

// --------------------------------------------------------------------------

template<typename T, typename LockType>
MsgQueue<T,LockType>::~MsgQueue()
{
	destroy();
}

// --------------------------------------------------------------------------
// Allocates a buffer of the specified size. If capacity is set to zero, an
// owned buffer is released and the object is set back to a zero state
// (it's destroyed).
//
// *** This routine is not thread safe ***
//

template<typename T, typename LockType>
void MsgQueue<T,LockType>::resize(size_t cap)
{
	if (cap > 0)
		set_buffer(new T[cap], cap, true);
	else
		destroy();
}

// --------------------------------------------------------------------------
// *** This routine is not thread safe ***

template<typename T, typename LockType>
void MsgQueue<T,LockType>::set_buffer(T* buf, size_t cap, bool own /*=false*/)
{
	destroy();

	if (buf != 0 && cap != 0) {
		buf_ = put_ = get_ = buf;
		end_ = buf+cap;
		own_ = own;

		for (size_t i=0; i<cap; i++)
			slotSem_.post();
	}
}

// --------------------------------------------------------------------------
//							Protected Members
// --------------------------------------------------------------------------
//

template<typename T, typename LockType>
void MsgQueue<T,LockType>::destroy()
{
	if (own_) {
		delete[] buf_;
		own_ = false;
	}

	while (dataSem_.tryacquire());
	while (slotSem_.tryacquire());

	sz_ = 0;
	buf_ = put_ = get_ = 0;
	end_ = 0;
}

// --------------------------------------------------------------------------
// Returns the next position for the specified get or put pointer.

template<typename T, typename LockType>
inline T* MsgQueue<T,LockType>::next(T* p)
{
	if (++p == end_)
		p = buf_;
	return p;
}

// --------------------------------------------------------------------------
// Puts (copies) a value into the queue. Before coming here, the caller 
// must acquire a slot semaphore.

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::do_put(const T& v)
{
	MyGuard g(lock_);

	*put_ = v;
	put_ = next(put_);
	sz_++;

	g.release();
	dataSem_.post();
	return true;
}

// --------------------------------------------------------------------------
// Gets a value from the queue. Before coming here, the caller must 
// acquire a data semaphore.

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::do_get(T *p)
{
	MyGuard g(lock_);
	
	*p = *get_;
	get_ = next(get_);
	sz_--;

	g.release();
	slotSem_.post();
	return true;
}

// --------------------------------------------------------------------------
//								Public Interface
// --------------------------------------------------------------------------
	
template<typename T, typename LockType>
inline size_t MsgQueue<T,LockType>::size() const
{
	MyGuard g(lock_);
	size_t sz = sz_;
	return sz;
}

template<typename T, typename LockType>
inline size_t MsgQueue<T,LockType>::available() const
{
	MyGuard g(lock_);
	size_t n = sz_;
	return n;
}

template<typename T, typename LockType>
inline size_t MsgQueue<T,LockType>::remaining() const
{
	MyGuard g(lock_);
	size_t n = capacity() - sz_;
	return n;
}

// --------------------------------------------------------------------------
// Places a value into the queue. Blocks until a slot is available.

template<typename T, typename LockType>
void MsgQueue<T,LockType>::put(const T& v)
{
	slotSem_.acquire();
	do_put(v);
}

// --------------------------------------------------------------------------
// Timed put. Blocked until a slot becomes available or a time out occurs.

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::put(const T& v, const Duration& d)
{
	if (!slotSem_.acquire(d))
		return false;

	return do_put(v);
}

// --------------------------------------------------------------------------
// Attempt to put immediately. It will fail if there isn't a slot currently
// available. Guaranteed not to block.
//

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::tryput(const T& v)
{
	if (!slotSem_.tryacquire())
		return false;
 
	return do_put(v);
}

// --------------------------------------------------------------------------
// Removes the next item from the queue and returns it. Blocks until a value
// is available. Use caution w/ large user-defined types. Calls default & 
// copy constructor (2x?).

template<typename T, typename LockType>
T MsgQueue<T,LockType>::get()
{
	dataSem_.acquire();

	T v;
	do_get(&v);
	return v;
}

// --------------------------------------------------------------------------
// Removes the next available item and writes it to the address, p.
// Blocks until a value is available.

template<typename T, typename LockType>
void MsgQueue<T,LockType>::get(T *p)
{
	dataSem_.acquire();
	do_get(p);
}

// --------------------------------------------------------------------------
// Timed get. Tries to remove the next available item and write it to the
// address, p. Blocks unitl a data item is available or a timeout occurs.

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::get(T *p, const Duration& d)
{
	if (!dataSem_.acquire(d))
		return false;

	return do_get(p);
}

// --------------------------------------------------------------------------
// Attempts to get a value if there is one currently available. Guaranteed
// not to block.
//

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::tryget(T *p)
{
	if (!dataSem_.tryacquire())
		return false;

	return do_get(p);
}

// --------------------------------------------------------------------------

template<typename T, typename LockType>
bool MsgQueue<T,LockType>::peek(T *p)
{
	if (sz_ != 0) {
		MyGuard g(lock_);	// Do double check locking
		if (sz_ != 0) {
			*p = *get_;
			return true;
		}
	}
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_MsgQueue_h

