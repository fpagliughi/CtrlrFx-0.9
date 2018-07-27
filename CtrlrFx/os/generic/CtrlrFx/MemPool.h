/// @file MemPool.h
///
///

#ifndef __CtrlrFx_MemPool_h
#define __CtrlrFx_MemPool_h

#include "CtrlrFx/MsgQueue.h"
#include "CtrlrFx/NullLock.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A memory pool.

template <typename T, typename LockType=Mutex> class MemPool
{
	T*						arr_;	///< The underlying memory, if we own it
	MsgQueue<T*, LockType>	que_;	///< The queue of available ptrs

	void dealloc();

public:
	/// Creates a memory pool with memory obtained from the heap
	/// @param n The number of blocks to allocate
	/// @param sz Te size of each block
	MemPool(size_t n, size_t sz);

	/// Creates a memory pool with the array that is provided
	MemPool(void *arr, size_t n, size_t sz, bool own=false);

	/// Destructor frees any owned memory
	~MemPool() { dealloc(); }

    /// Assigns the memory to the pool
	void assign(void *arr, size_t n, size_t sz, bool own=false);

	/// Gets a memory array from the pool
	T* get() { return que_.get(); }

	/// Tries to get a block from the pool, and waits a bounded amount
	/// of time if the pool is currently empty.
	bool get(T *p, const Duration& d);

	/// Non-blocking attempt to gets a memory array from the pool
	bool tryget(T* p) { return que_.tryget(p); }

	/// Returns a memory array to the pool.
	void put(T* p) { que_.put(p); }
};

// --------------------------------------------------------------------------

template <typename T, typename LockType>
MemPool<T,LockType>::MemPool(size_t n, size_t sz) : que_(n)
{
	arr_ = new T[n * sz];
	for (size_t i=0; i<n; ++i)
		que_.put(&arr_[i*sz]);
}


template <typename T, typename LockType>
MemPool<T,LockType>::MemPool(void *arr, size_t n, size_t sz, bool own /*=false*/) : arr_(0), que_(n)
{
	// TODO: The message queue should be allocated from the provided memory
	assign(arr, n, sz, own);
}

template <typename T, typename LockType>
void MemPool<T,LockType>::dealloc()
{
	delete[] arr_;
}

template <typename T, typename LockType>
void MemPool<T, LockType>::assign(void *arr, size_t n, size_t sz, bool own /*=false*/)
{
	dealloc();

	arr_ = (own) ? arr : 0;
	for (size_t i=0; i<n; ++i)
		que_.put(&arr_[i*sz]);
}

// --------------------------------------------------------------------------

/// A memory pool of bytes.
typedef MemPool<byte> BytePool;

/// An unprotected (single threaded) pool of bytes.
typedef MemPool<byte, NullLock> StBytePool;

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_MemPool_h

