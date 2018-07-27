/// Sempahore.h
/// CtrlrFx implementation of Semaphores for POSIX targets.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved

#ifndef __CtrlrFx_Semaphore_h
#define __CtrlrFx_Semaphore_h

#include "CtrlrFx/Time.h"
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <errno.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper class arouund a counting semaphore.
 */
class Semaphore
{
protected:
	sem_t	sem_;		///< Handle to the OS Semaphore
	int		err_;		///< The last error encountered

public:
	/**
     * Construct a semaphore.
     * This creates an OS semaphore.
	 */
	Semaphore();
	/**
     * Construct a semaphore with the specified initial count.
     * This creates an OS semaphore.
     * @param cnt The initial count for the semaphore. This is the number of
     *            tokens it initially holds.
	 */
	explicit Semaphore(int cnt);
	/**
	 * Destroys the semaphore, including the underlying OS object.
	 */
	~Semaphore() { destroy(); }
	/**
     * (Re)creates the semaphore with the specified count.
     * This destroys the existing OS semaphore and creates a new one with
     * the specified initial count.
     * @param cnt The number of tokens the semaphore should be holding.
     * @return int @em Zero on success, @em <0 on failure.
	 */
	int create(int cnt);
	/**
     * Destroys the underlying OS semaphore.
     * This is not thread safe and should not be called if any other threads
     * may be holding the semaphore or may try to access it in the future.
	 */
	void destroy();
	/**
	 * Determines if the mutex is valid and usable.
     * This makes sure there were no errors creating the underlying OS
     * mutex, and that the C++ object is properly connected to an OS object.
     * @return @em true if the object is valid and usable.
	 */
	bool is_valid() const { return err_ == 0; }
	/**
	 * Gets the last internal error for the mutex.
	 * In general this is only set on a failed construction.
     * @return @em Zero for no errors, otherwise the error code.
	 */
	int error() const { return err_; }
	/**
     * Determines if the semaphore is valid and usable.
     * This can be called to detemine if the construction of the semaphore
     * succeeded.
     * @return Non-zero if the semaphore is valid.
	 */
	operator void*() const { return (void*) is_valid(); }
	/**
     * Determines if the semaphore is invalid (not usable).
     * This can be called to detemine if the construction of the semaphore
     * failed.
     * @return Non-zero if the semaphore is invalid.
	 */
	bool operator!() const { return !is_valid(); }
	/**
     * Wait for (and acquire) the semaphore.
     * This blocks until the semaphore is available and acquires it.
	 */
	void wait() { ::sem_wait(&sem_); }
	/**
     * Attempts to acquire the semaphore, but only waits a bounded amount of
     * time for it.
     * @param d The realtive amount of time to wait.
     * @return bool @em true if the semaphore was acquired and is currently
     *         held, @em false otherwise.
	 */
	bool wait(const Duration& d) {
		return wait_until(Time::from_now(d));
	}
	/**
     * Attempts to acquire the semaphore, but only waits until a specific
     * absolute time for it to become available.
     * @param t The time to wait until.
     * @return bool @em true if the semaphore was acquired and is currently
     *         held, @em false otherwise.
	 */
	bool wait_until(const Time& t) {
		return ::sem_timedwait(&sem_, &(const_cast<Time&>(t))) == 0;
	}
	/**
     * Attempts to acquire the semaphore without blobking.
     * @return bool @em true if the semaphore was acquired and is currently
     *         held, @em false otherwise.
	 */
	bool trywait() { return ::sem_trywait(&sem_) == 0; }
	/**
     * Posts (releases) the semaphore.
	 */
	void post() { ::sem_post(&sem_); }

	// ----- Acquire/Release Nomenclature -----

	void	acquire()			{ wait(); }
	bool	acquire(const Duration& d)	{ return wait(d); }
	bool	tryacquire()		{ return trywait(); }
	void	release()			{ post(); }

};

// --------------------------------------------------------------------------

inline Semaphore::Semaphore() : err_(0)
{
	if (::sem_init(&sem_, 0, 0) < 0)
		err_ = errno;
	assert(err_ == 0);
}

inline Semaphore::Semaphore(int cnt) : err_(0)
{
	if (::sem_init(&sem_, 0, cnt) < 0)
		err_ = errno;
	assert(err_ == 0);
}

inline int Semaphore::create(int cnt)
{
	destroy();
	err_ = (::sem_init(&sem_, 0, cnt) == 0) ? 0 : errno;
	assert(err_ == 0);
	return err_;
}

inline void Semaphore::destroy()
{
	if (err_ == 0) {
		::sem_destroy(&sem_);
		err_ = -1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// TODO: This isn't really a binary semaphore. 
// 			We need to define something complete

typedef Semaphore BinarySemaphore;

/////////////////////////////////////////////////////////////////////////////
// endnamespace CtrlrFx
};

#endif		// __CtrlrFx_Semaphore_h

