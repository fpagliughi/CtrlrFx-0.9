/// @file ConditionVar.h
///
/// CtrlrFx implementation of a condition variable for POSIX targets.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved

#ifndef __CtrlrFx_ConditionVar_h
#define __CtrlrFx_ConditionVar_h

#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/**
 * A condition variable synchronization object.
 */
class ConditionVar
{
	int				err_;		///< The last error encountered
	pthread_cond_t	cond_;		///< The OS condition variable handle
	Mutex			mutex_;		///< Mutex to guard the condition

	// Non-copyable
	ConditionVar(const ConditionVar&);
	ConditionVar& operator=(const ConditionVar&);

public:
	/// Constructs a condition variable that uses its internal mutex.
	ConditionVar();

	/// Constrcts a condition variable to use the mutex provided.
	ConditionVar(Mutex& mutex);

	/// Destroys the condition variable.
	~ConditionVar() {
		if (err_ == 0)
			::pthread_cond_destroy(&cond_);
	}

	/// Determines if the condition variable is valid and usable.
	/// This makes sure there were no errors creating the OS condition 
	/// variable, and that the C++ object is properly connected to an
	/// OS object.
	/// @return true if the object is valid and usable.
	bool is_valid() const { return err_ == 0; }

	/// Gets the last internal error for the condition variable.
	int error() const { return err_; }

	/// Determines if the condition variable is valid and usable.
	operator void*() const { return (void*) is_valid(); }

	/// Determines if the condition variable is invalid (unusable).
	bool operator!() const { return !is_valid(); }

	/// Locks the condition variable.
	/// This locks the mutex that is associated with the condition variable.
	/// The variable must locked to perform most operations, like examining
	/// the predicate, waiting, and signaling.
	void lock() { mutex_.lock(); }

	/// Attempts to lock the condition variable without blocking.
	/// This will attempt to lock the condition variable's associated mutex.
	/// If the mutex is currently locked this will fail immediately without
	/// blocking.
	/// @return 
	/// @li true if the lock was obtained
	/// @li false if the lock was not obtained
	bool trylock() { return mutex_.trylock(); }

	/// Unlocks the condition variable.
	/// This releases the lock on the associated mutex.
	void unlock() { mutex_.unlock(); }

	/// Locks the condition variable.
	/// @sa lock
	void acquire() { lock(); }

	/// Attempts to lock the condition variable without blocking.
	/// @sa trylock
	/// @return 
	/// @li true if the lock was obtained
	/// @li false if the lock was not obtained
	bool tryacquire() { return trylock(); }

	/// Unlocks the condition variable.
	/// @sa unlock
	void release() { unlock(); }

	/// Waits for the condition variable to become signaled.
	/// This puts the calling thread to sleep until another thread signals
	/// the condition variable.
	void wait() {
		::pthread_cond_wait(&cond_, &mutex_.handle());
	}

	/// Timed wait for the condition variable to be signaled.
	/// This puts the calling thread to sleep until another thread signals
	/// the condition variable. If the variable isn't signaled in the 
	/// specified amount of time, a timeout occurs.
	/// @return 
	/// @li true If the condition variable was signaled.
	/// @li false If a timeout occured.
	bool wait(const Duration& d) {
		return wait_until(Time::from_now(d));
	}

	/// Timed wait for the condition variable to be signaled, using an 
	/// absolute time.
	/// This puts the calling thread to sleep until another thread signals
	/// the condition variable. If the variable isn't signaled by the 
	/// specified time, a timeout occurs.
	/// @return 
	/// @li true If the condition variable was signaled.
	/// @li false If a timeout occured.
	bool wait_until(const Time& t) {
		return ::pthread_cond_timedwait(&cond_, &mutex_.handle(),
								&(const_cast<Time&>(t))) == 0;
	}

	/// Signals the conditional variable.
	/// This wakes a single thread that is waiting on the condition variable.
	void signal() {
		::pthread_cond_signal(&cond_);
	}
	/// Broadcasts a signal to the condition variable.
	/// This wakes @em all threads that are waiting on the condition variable.
	void broadcast() {
		::pthread_cond_broadcast(&cond_);
	}
};

// --------------------------------------------------------------------------

inline ConditionVar::ConditionVar()
{
	if ((err_ = mutex_.error()) == 0 &&
			::pthread_cond_init(&cond_, NULL) < 0)
		err_ = errno;
	assert(err_ == 0);
}

inline ConditionVar::ConditionVar(Mutex& mutex) : err_(0), mutex_(mutex.handle())
{
	if (::pthread_cond_init(&cond_, NULL) < 0)
		err_ = errno;
	assert(err_ == 0);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_ConditionVar_h

