/// @file ObjPool.h
/// Definition of a thread-safe pool of objects.
///
/// @author	Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// Copyright(c) 2005, SoRo Systems, Inc.
/// All Rights Reserved

#ifndef __CtrlrFx_ObjPool_h
#define __CtrlrFx_ObjPool_h

#include "CtrlrFx/MsgQueue.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A thread-safe container to keep an unordered pool of objects.
/// The pool keeps pointers to the actual objects. When a client requests
/// an object, with the @ref get method, the calling thread is blocked until
/// an object is available, and the address of the object is returned. When
///	the client is done with the object it returns it to the pool by calling
/// @em put with the address of the object.

template <typename T, typename LockType=Mutex> 
class ObjPool : public MsgQueue<T*, LockType>
{
	typedef MsgQueue<T*, LockType> Base;

	bool own_;		///< Whether we own the objects in the pool

	// Non-copyable
	ObjPool(const ObjPool&);
	ObjPool& operator=(const ObjPool&);

protected:
	/// Creates an empty pool.
	ObjPool() : own_(false) {}

	/// Frees the objects in the pool, if we own them.
	void dealloc();

public:
	/// Creates a pool with the specified number of items.
	/// The objects are created on the heap
	explicit ObjPool(size_t n);

	/// Creates a pool from an array of objects.
	ObjPool(T* arr, size_t n);

	~ObjPool() { dealloc(); }
};

// --------------------------------------------------------------------------

template <typename T, typename LockType>
ObjPool<T,LockType>::ObjPool(size_t n) : Base(n), own_(true)
{
	while (n--)
		Base::put(new T);
}

template <typename T, typename LockType>
ObjPool<T,LockType>::ObjPool(T* arr, size_t n) : Base(n), own_(false)
{
	while (n--)
		Base::put(arr++);
}


template <typename T, typename LockType>
void ObjPool<T,LockType>::dealloc()
{
	if (own_) {
		T* p;
		while (Base::tryget(&p))
			delete p;
		own_ = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Pool_h

