/// @file Thread.h
//
// Implementation of the OS for POSIX (pthreads) system.

#ifndef __OSFactoryPosix_h
#define __OSFactoryPosix_h

#include "CtrlrFx/Time.h"
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include "CtrlrFx/OSEvent.h"

#ifndef CTRLR_FX_THREAD_NAME_LEN
	#define CTRLR_FX_THREAD_NAME_LEN 32
#endif

namespace CtrlrFx {

typedef pthread_t thread_t;

/////////////////////////////////////////////////////////////////////////////
// 								Thread
/////////////////////////////////////////////////////////////////////////////

/**
 * An abstract base class for active objects.
 * To create a concrete class, this should be overriden with the concrete
 * class implementing the run() function. In addition, the new class often
 * needs to provide a different implementation of the quit() method to
 * signal run() when it's time to terminate.
 */
class Thread
{
	typedef void* (*ThreadFunc)(void*);

public:
	static const int	DFLT_STACK_SIZE			= 2048;

	static const int	PRIORITY_HIGHEST		= 99,
						PRIORITY_ABOVE_NORMAL	= 75,
						PRIORITY_NORMAL			= 50,
						PRIORITY_BELOW_NORMAL	= 25,
						PRIORITY_LOWEST			=  5,
						PRIORITY_IDLE			=  1,
		
						PRIORITY_INCREASE		= +1,
						PRIORITY_DECREASE		= -1;

	static const msec_t	INFINITE				= msec_t(-1),
						WAIT_FOREVER			= INFINITE;

	static const int 	DFLT_POLICY = SCHED_RR;

private:
	pthread_t			thread_;		///< Handle to the OS thread
	mutable int			prio_;			///< The thread's priority
	ManualResetEvent	evtDone_;		///< Gets signaled when the thread completes
	volatile bool		active_;		///< true if the thread was activated

protected:
	volatile bool quit_;	///< Flag gets set when someone wants the thread to exit

	/**
     * Creates a thread at the specified priority.
     * The thread is created with the default (os-dependent) stack size.
     * @param prio The thread's priority. This can be a os-independent
     *             constant like @em Thread::PRIORITY_NORMAL, or @em
     *             Thread::PRIORITY_ABOVE_NORMAL
     * @param stackSize (ignored in POSIX port)
     * @param name (ignored in POSIX implementation)
	 */
	explicit Thread(int prio, unsigned stackSize=0, const char *name=0);
	/**
	 * Virtual base destructor.
	 */
	virtual ~Thread() {}
	/**
     * The static c-style thread startup function.
     * This receives control from the OS when a new thread is started. It
     * assumes that the parameter, p, is a pointer to a thread object, and
     * calls it's run() method.
     * @param p Pointer to the thread object that should get control. The
     *          object's run() function is then called from this new thread
     *          context.
     * @return void*
	 */
	static void* startup(void *p);
	/**
	 * The thread function.
     * Derived classes should implement this to perform the functionality of
     * the thread.
     * @return Application-defined return value. Normally zero indicates
     *         a normal exit (success).
	 */
	virtual int run() =0;
	/**
     * The last function called before the OS thread is detached from this object.
     * The thread object can perform any final cleanup in this routine. A
     * thread object that was allocated on the heap can safely delete itself
     * at the end of this routine.
	 */
	virtual void close() {}

public:
	// ----- Static Functions: Apply to Current Thread -----

	/**
     * Puts the current thread to sleep for the specified (relative) time.
     * @param d The duration of time to sleep.
	 */
	static void sleep(const Duration& d);
	/**
     * Puts the current thread to sleep until the specified absolute time.
     * This works with a time based on any of the POSIX clocks, includeing
     * CLOCK_REALTIME (Time), or CLOCK_MONOTONIC (MonotonicTime).
     * @param CLOCK The POSIX clock on which the time is based.
     * @param t The time to wait until.
	 */
	template <clockid_t CLOCK>
	static void sleep_until(const TimeTmpl<CLOCK>& t) {
		::clock_nanosleep(CLOCK, TIMER_ABSTIME, (timespec*) &t, NULL);
	}
	/**
     * Gets the OS handle to the current thread.
     * @return The OS handle to the current thread.
	 */
	static thread_t curr_thread() { return (thread_t) ::pthread_self(); }
	/**
	 * Sets the priority of the current thread.
     * @param prio The thread's priority. This can be an os-independent
     *             constant like @em Thread::PRIORITY_NORMAL, or
     *             Thread::PRIORITY_ABOVE_NORMAL
     * @return bool @em true if the priority was changed, @em false if an
     *         error occured.
	 */
	static bool curr_priority(int prio);
	/**
     * Gets the priority of the current thread.
     * @return The priority of the current thread.
	 */
	static int curr_priority();

	// ----- Public Interface -----

    /**
     * Activates the thread by placing it into the scheduler's queue.
     * When created, the thread object is unknown to the operating system.
     * Activating it places it in the operating system's scheduler, and sets
     * it ready to run. If the thread is a higher priority than the caller,
     * the thread will start running immediately, and the call won't return
     * until the caller is rescheduled.
     * @return bool @em true if the thread is activated, @em false on an
     *         error.
	 */
	bool activate();
	/**
     * Wait for the thread to complete.
     * This is normally called after a pre-arranged signal has been sent to
     * the thread asking it to terminate gracefully. The call will block the
     * caller until this thread completes by exiting its run() function.
     * 
     * @note The request for this thread to quit is normally implemented by
     *       using or overriding the @ref quit() method.
	 */
	void wait();
	/**
	 * Time-bounded wait for the thread to complete.
     * @param d The maximum amount of time to wait for the thread to
     *          complete.
	 * 
	 * @return bool
	 */
	bool wait(const Duration& d);
	/**
	 * Sets the priority of the thread.
     * @param prio The thread's priority. This can be a os-independent
     *             constant like @em Thread::PRIORITY_NORMAL, or @em
     *             Thread::PRIORITY_ABOVE_NORMAL
     * @return bool @em true if the priority was changed, @em false if an
     *         error occured.
	 */
	bool priority(int prio);
	/**
     * Gets the priority of the current thread.
     * @return The priority of the current thread.
	 */
	int priority() const;
	/**
     * Sends a request to the thread to terminate itself gracefully.
     * This does not wait for the thread to complete. To do that, call the
     * @ref wait function.
     * @note This implementation simply sets the @em quit flag to true. It's
     *       up to the derived class to check this flag and terminate when
     *       it gets set, or to override this method and provide a more
     *       appropriate function. If the derived thread typically blocks on
     *       a queue or event awaiting input, the derived @em quit should
     *       knock the thread off the queue and have it exit as quickly as
     *       possible.
	 */
	virtual void quit() { quit_ = true; }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __posix_h

