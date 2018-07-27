/// @file ThreadPool.h
///
/// Generic pool of threads.
///

#ifndef __CtrlrFx_ThreadPool_h
#define __CtrlrFx_ThreadPool_h

#include "CtrlrFx/os.h"
#include "CtrlrFx/ObjPool.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

template <typename ThreadType>
class ThreadPool : public ObjPool<ThreadType, Mutex>
{
public:
	/// Creates a pool with the specified number of threads.
	/// The thread objects are created on the heap.
	explicit ThreadPool(size_t n) : ObjPool<ThreadType, Mutex>(n) {}

	/// Creates a pool from the array of objects.
	ThreadPool(ThreadType arr[], size_t n)  : ObjPool<ThreadType, Mutex>(arr, n) {}
};

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
};

#endif		// __CtrlrFx_ThreadPool_h

