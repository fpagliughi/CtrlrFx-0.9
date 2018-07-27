/// @file SeqLock.h
/// A non-blocking sequence lock

#ifndef __CtrlrFx_SeqLock_h
#define __CtrlrFx_SeqLock_h

#include "CtrlrFx/AtomicCounter.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A specialized lock primitive.
/// A SeqLock is used to protect access to simple data types that are read 
/// often, but only updated occasionally by one writer.
/// @note With this implementation there can only be one writer thread.
/// The counter type must be atomically assignable on the target platform,
/// but the increment operation does not necessarily have to be atomic, 
/// since there should only be one writer thread attempting the write lock.

template <typename CtrType=int>
class SeqLock
{
	CtrType ctr_;

public:
	/// Start a write operation
	void write_lock() { ++ctr_; }

	/// Finish a write operation
	void write_unlock() { ++ctr_; }

	/// Acquire the write lock (acquire/release nomenclature).
	/// Reads don't affect the state of the object.
	void acquire() { write_lock(); }

	/// Release the write lock (acquire/release nomenclature).
	/// Reads don't affect the state of the object.
	void release() { write_unlock(); }

	/// Start a read operation
	int read_begin() const { return (int) ctr_; }

	/// Determines if the read operation failed and should be re-tried
	/// @return
	/// @li true if the read failed and should be re-tried
	/// @li false if the read succeeded
	bool read_retry(int ctr) const;
};

// --------------------------------------------------------------------------

template <typename CtrType>
bool SeqLock<CtrType>::read_retry(int ctr) const
{
	return ((ctr & 1) != 0) || (ctr != ctr_);
}

/////////////////////////////////////////////////////////////////////////////
/// A SeqLock used by an ISR to update a variable.
/// Since the ISR should execute uninterrupted, the counter is guaranteed atomic.
typedef SeqLock<int> IntrSeqLock;

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_SeqLock_h

