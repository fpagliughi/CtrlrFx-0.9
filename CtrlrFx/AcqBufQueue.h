/// @file AcqBufQueue.h
/// A self-contained multi-queue structure for passing acquisition buffers
/// between multiple threads.
///
/// @author	Frank Pagliughi
///	@author	SoRo Systems, Inc.
///	@author	www.sorosys.com
///
/// @date	March 2004

// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_AcqBufQueue_h
#define __CtrlrFx_AcqBufQueue_h

#include "CtrlrFx/MsgQueue.h"
#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/Buffer.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A container for set of thread-safe buffer queues for applications that
/// pass buffers between multiple threads.
/// This container is like an array of buffer queues, with the first one
/// acting as a pool of empty buffers. When created, the zero'th array
/// contains buffers that can be used by an acquisition thread that then
/// passes them to other threads by putting the buffer in one of the 
/// upper array slots.
/// A typical use is to have a series of threads that process data in
/// stages. Each stage waits for a buffer in one slot, processes the buffer,
/// then places it into the next slot of the array. In this setup, the first
/// slot is usually filled with empty buffers before the threads are started.
/// The last thread in the pipeline empties the buffers and places the empty
/// buffers back in the first slot.

template<typename T, typename LockType=Mutex> class AcqBufQueue
{
public:
	typedef MsgQueue<Buffer<T>*,LockType> QueType;

private:
	size_t	nque_;
	QueType	*que_;

	void dealloc();

	// Non-copyable
	AcqBufQueue(const AcqBufQueue&);
	AcqBufQueue& operator=(const AcqBufQueue&);


public:
	/// Creates an empty buffer queue collection.
	AcqBufQueue();

	/// Creates a colection of buffer queues, but doesn't add any buffers.
	/// @param nque the number of queues
	/// @param que_sz the size of each queue (the number of buffers each 
	///			can hold)
	AcqBufQueue(size_t nque, size_t que_sz);

	/// Creates a colection of buffer queues, and fills the first queue with
	/// empty buffers allocated off the heap.
	/// @param nque the number of queues
	/// @param que_sz the size of each queue (the number of buffers each 
	///			can hold)
	/// @param buf_sz the size of each buffer created
	AcqBufQueue(size_t nque, size_t que_sz, size_t buf_sz);

	/// Resizes the queue collection
	/// @param nque the number of queues
	/// @param que_sz the size of each queue (the number of buffers each 
	///			can hold)
	void resize(size_t nque, size_t que_sz);

	/// Creates a colection of buffer queues, and fills the first queue with
	/// empty buffers allocated off the heap.
	/// @param nque the number of queues
	/// @param que_sz the size of each queue (the number of buffers each 
	///			can hold)
	/// @param buf_sz the size of each buffer created
	void resize(size_t nque, size_t que_sz, size_t buf_sz);

	/// Queue indexer
	/// @return Gets a reference to the i'th queue
	QueType& operator[] (int i) { return que_[i]; }

	/// Clears out the collection and empties all the buffers.
	/// This removes all the buffers from upper queues, clears the buffers,
	/// and places them into the first queue.
	void clear();

	/// Attempts to release the threads waiting on the queues.
	void release();
};

// --------------------------------------------------------------------------

template<typename T, typename LockType>
AcqBufQueue<T,LockType>::AcqBufQueue() : que_(0)
{
}

template<typename T, typename LockType>
AcqBufQueue<T,LockType>::AcqBufQueue(size_t nque, size_t que_sz) : que_(0)
{
	resize(nque, que_sz);
}

template<typename T, typename LockType>
AcqBufQueue<T,LockType>::AcqBufQueue(size_t nque, size_t que_sz, size_t buf_sz)
							: que_(0)
{
	resize(nque, que_sz, buf_sz);
}

// --------------------------------------------------------------------------

template <typename T, typename LockType>
void AcqBufQueue<T,LockType>::dealloc()
{
	delete[] que_;
	que_ = 0;
}

// --------------------------------------------------------------------------
// Creates empty queues.

template<typename T, typename LockType>
void AcqBufQueue<T,LockType>::resize(size_t nque, size_t que_sz)
{
	dealloc();
	que_ = new QueType[nque_ = nque];

	for (size_t i=0; i<nque; ++i)
		que_[i].resize(que_sz);
}

// --------------------------------------------------------------------------
// Creates queues of the specified, nBuf, size, and fills the zero'th queue
// with buffers containing 'nElem" elements. The remaining queues are empty.

template<typename T, typename LockType>
void AcqBufQueue<T,LockType>::resize(size_t nque, size_t nbuf, size_t sz)
{
	resize(nque, nbuf);

	for (size_t i=0; i<nbuf; ++i)
		que_[0].put(new Buffer<T>(sz));
}

// --------------------------------------------------------------------------
// Clears out the structure by removing all the buffers from all of the upper
// queues and placing them into the zero'th queue.
//

template<typename T, typename LockType>
void AcqBufQueue<T,LockType>::clear()
{
	for (size_t i=nque_-1; i>0; --i) {
		Buffer<T> *p;
		
		while (que_[i].try_get(&p)) {
			p->clear();
			que_[0].put(p);
		}
	}
}

// --------------------------------------------------------------------------

template<typename T, typename LockType>
void AcqBufQueue<T,LockType>::release()
{
	for (size_t i=nque_-1; i>=0; --i)
		que_[i].release();
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __AcqBufQueue_h

