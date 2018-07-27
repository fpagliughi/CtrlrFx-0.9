/// @file Guard.h
/// Definition of an empty locking construct.

#ifndef __CtrlrFx_Guard_h
#define __CtrlrFx_Guard_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A guard is used to manage any lockable resource that exports 
/// acquire/release semantics.
///
/// On creation the guard locks the resource, then automatically releases
/// the lock when it (the guard) is destroyed. This helps insure that locks
/// are released when the guard goes out of scope. This is particularly
/// important in the presence of exceptions and/or multiple return paths
/// in the block.
///
/// @note
/// Although used to protedct an underlying thread sync object, the guard
/// object itself is not thread safe. The lock object is shared between
/// threads. The guard is not. It is normally used in a single function or
/// code block, and automatically releases the lock when the function returns.

template <typename Lock> class Guard
{
	Lock&	lock_;	///< The lockable object we're guarding
	bool 	own_;	///< Whether we currently own the lock

public:
	/// Constructs the guard and acquires the lock.
	Guard(Lock& lock) : lock_(lock), own_(true) {
		lock_.acquire();
	}

	/// Constructs the guard and acquires the lock, if not already owned
	Guard(Lock& lock, bool own) : lock_(lock), own_(own) {
		acquire();
	}

	/// Destroys the guard and releases the lock, if held.
	~Guard() {
		if (own_)
			lock_.release();
	}

	/// Acquires the lock.
	/// This has no effect of the lock is already held.
	void acquire() {
		if (!own_) {
			lock_.acquire();
			own_ = true;
		}
	}

	/// Releases the lock.
	/// This has no effect of the lock is not currently held.
	void release() {
		if (own_) {
			lock_.release();
			own_ = false;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_NullLock_h

