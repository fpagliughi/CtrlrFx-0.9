/// @file BufPool.h
//
// Class to handle a pool of data buffers. This is normally used in a
// high throughput system, where two or more buffers are needed to keep
// the high-priority hardware I/O thread with an available buffer to
// fill or empty, while a lower priority thread works on the data. The
// underlying queue is fully thread safe for passing buffers around.
//
// DESCRIPTION:
//		Buffer queues present an efficient way to pass data between
//		threads of an application, especially when data is streaming
//		in or out of an application at a high rate. This is common in
//		a data acquisition & processing application.
//
//		The I/O thread obtains a buffer from the queue via a reference
//		(i.e. no copying of individual data items), and then is free to fill
//		or empty the buffer without worry about thread sync for the items.
//		Only when the buffer is full (or empty) does the thread pass it on,
//		and thus syncs via the underlying mutex.
//
//		The underlying queue holds void pointers, regardless of the type of
//		buffers. This is an attempt at reducing template code bloat, since
//		we're wrapping all the entry/exit points anyway. We can do the 
//		casting in Put() & Get().
//
//		In a real I/O system the buffers are passed between multiple queues.
//		Each thread has its own queue, of size N, to wait on, but there are
//		a total of only N buffers. For example, consider an application that
//		has two threads: one fills the buffers and the other empties them.
//		You need two queues. One holds empty buffers and the other holds
//		filled buffers. At the start of the application N buffers are created
//		and are all placed in the "empty buffer" queue. The "full buffer"
//		queue gets none. The source thread gets a buffer from the "empty"
//		queue, fills it, places it in the "full" queue, then gets the next
//		buffer from the "empty" queue. The sink thread blocks on the "full
//		buffer" queue, gets a buffer, empties it, then returns it to the
//		"empty queue."
//
//		The MtAcqBufferQueue<T,N> is a self-contained object for handling 
//		this multi-queue scenario.

//
// PROJECT:
//		Performance Controller Framework
//
// PLATFORM:
//		
//
// AUTHOR:
//		Frank Pagliughi
//		SoRo Systems, Inc.
//
// DATE:
//		May, 2001
//
// COPYRIGHT:
//		Copyright (c) 2001, SoRo Systems, Inc.
//		All Rights Reserved.
//
// SOURCE SAFE:
//		$History:$
//

#ifndef __CtrlrFx_BufPool_h
#define __CtrlrFx_BufPool_h

#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/Buffer.h"
#include "CtrlrFx/ObjPool.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A template for thread-safe collections of objects

template<typename T, typename LockType=Mutex> class BufPool
								: public ObjPool<Buffer<T>, LockType>
{
	typedef ObjPool<Buffer<T>, LockType> Base;

public:
	BufPool() {}
	BufPool(size_t n, size_t sz);

	/// Creates a buffer queue with the specified capacity (n) and
	/// allocates that many buffers off the heap and adds them to the queue. Each
	/// buffer has room for nElem. Thus, the queue starts off full.
	void resize(size_t n, size_t sz);

	void resize_buffers(size_t sz);

	/// "Releases" a thread waiting on the queue. 
	/// This normally places a null reference in the pool to release a 
	/// waiting thread.
	void release();
};

// --------------------------------------------------------------------------

template<typename T, typename LockType>
BufPool<T,LockType>::BufPool(size_t n, size_t sz) : Base(n)
{
	resize_buffers(sz);
}

// --------------------------------------------------------------------------

template<typename T, typename LockType>
void BufPool<T,LockType>::resize(size_t n, size_t sz)
{
	Base::resize(n);
	resize_buffers(sz);
}

template<typename T, typename LockType>
void BufPool<T,LockType>::resize_buffers(size_t sz)
{
	// FIXME: This is a bad implementation 
	size_t n = Base::capacity();

	for (size_t i=0; i<n; ++i) {
		Buffer<T>* buf = Base::get();
		buf->resize(sz);
		Base::put(buf);
	}
}

// --------------------------------------------------------------------------

template<typename T, typename LockType>
inline void BufPool<T,LockType>::release()
{
	Base::tryput(0);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_BufPool_h

