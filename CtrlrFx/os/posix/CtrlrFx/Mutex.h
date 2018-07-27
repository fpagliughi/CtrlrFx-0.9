/// @file Mutex.h
/// CtrlrFx implementation of mutex wrapper class for POSIX targets.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved
///

#ifndef __CtrlrFx_Mutex_h
#define __CtrlrFx_Mutex_h

#include <pthread.h>
#include <errno.h>
#include <assert.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/**
 * A mutually exclusive thread synchronization object.
 * This wraps an underlying OS mutex object.
 */
class Mutex
{
	pthread_mutex_t mutex_;		///< Handle to the OS Mutex
	bool			own_;		///< Whether we own the OS handle
	int				err_;		///< The last error encountered

public:
	/// Construct a mutex.
	/// This creates a new mutex in the OS.
	Mutex();

	/// Constructs a mutex wrapper around the specified OS mutex handle.
	/// If we're given ownership of the handle, the OS mutex will be destroyed
	/// when this wrapper is destroyed.
	Mutex(const pthread_mutex_t& h, bool own=false);

	/// Constructs a weak reference to the specified mutex.
	Mutex(Mutex& mutex);

	/// Destroys the mutex.
	/// This destructs the C++ object. If the C++ object "owns" the 
	/// underlying OS mutex handle, then the OS mutex is destroyed as well.
	~Mutex();

	/// Gets the OS mutex handle.
	pthread_mutex_t& handle() { return mutex_; }

	/// Determines if the mutex is valid and usable.
	/// This makes sure there were no errors creating the underlying OS
	/// mutex, and that the C++ object is properly connected to an
	/// OS object.
	/// @return true if the object is valid and usable.
	bool is_valid() const { return err_ == 0; }

	/// Gets the last internal error for the mutex.
	/// In general this is only set on a failed construction.
	int error() const { return err_; }

	/// Determines if the mutex is valid and usable.
	operator void*() const { return (void*) is_valid(); }

	/// Determines if the mutex is invalid (unusable).
	bool operator!() const { return !is_valid(); }

	/// Locks the mutex.
	/// If another thread hold the mutex, this will block the calling thread
	/// until it gets possesion of the mutex.
	void lock() { ::pthread_mutex_lock(&mutex_); }

	/// Attempts to lock the mutex without blocking.
	/// If another thread holds the mutex this call will fail.
	/// @return
	/// @li @em true the mutex was acquired and is currently held.
	/// @li @em false the mutex was not acquired
	bool trylock() { return ::pthread_mutex_trylock(&mutex_) == 0; }

	/// Releases the lock on the mutex.
	void unlock() { ::pthread_mutex_unlock(&mutex_); }

	/// Locks the mutex.
	/// @sa lock
	void acquire() { lock(); }

	/// Attempts to lock the mutex without blocking.
	/// @sa trylock
	/// @return
	/// @li @em true the mutex was acquired and is currently held.
	/// @li @em false the mutex was not acquired
	bool tryacquire() { return trylock(); }

	/// Releases the lock on the mutex.
	/// @sa unlock
	void release() { unlock(); }
};

// --------------------------------------------------------------------------

inline Mutex::Mutex() : own_(true), err_(0)
{
	if (::pthread_mutex_init(&mutex_, NULL) < 0)
		err_ = errno;
	assert(err_ == 0);
}

// --------------------------------------------------------------------------

inline Mutex::Mutex(const pthread_mutex_t& h, bool own /*=false*/) 
			: mutex_(h), own_(own), err_(0)
{
}

// --------------------------------------------------------------------------

inline Mutex::Mutex(Mutex& m)
			: mutex_(m.handle()), own_(false), err_(m.error())
{
}

// --------------------------------------------------------------------------

inline Mutex::~Mutex()
{
	if (own_ && err_ == 0)
		::pthread_mutex_destroy(&mutex_);
}

/////////////////////////////////////////////////////////////////////////////

typedef Mutex FastMutex;

/////////////////////////////////////////////////////////////////////////////
// endnamespace CtrlrFx
};

#endif		// __CtrlrFx_Mutex_h

