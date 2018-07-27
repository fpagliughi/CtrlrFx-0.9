// Thread.cpp
// CtrlrFx thread definitions for the POSIX targets.

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>

#if !defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
	#error "No POSIX thread priority scheduling"
#endif

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//								Thread
/////////////////////////////////////////////////////////////////////////////

Thread::Thread(int prio, unsigned, const char*)
				: prio_(prio), active_(false), quit_(false)
{
}

// --------------------------------------------------------------------------
//								Static Functions
// --------------------------------------------------------------------------

void* Thread::startup(void* p)
{
	Thread* thread = static_cast<Thread*>(p);

	int n = thread->run();
	thread->evtDone_.signal();
	thread->close();

	return reinterpret_cast<void*>(n);
}

// --------------------------------------------------------------------------
// Blocks the thread for the specified duration.

void Thread::sleep(const Duration& d)
{
	::nanosleep(&(const_cast<Duration&>(d)), NULL);
}

// --------------------------------------------------------------------------
// Sets the priority of the current thread.

bool Thread::curr_priority(int prio)
{
	struct sched_param param;
	int policy;

	pthread_t self = ::pthread_self();

	::pthread_getschedparam(self, &policy, &param);
	param.sched_priority = prio;
	return ::pthread_setschedparam(self, 1, &param) == 0;
}

// --------------------------------------------------------------------------
// Gets the priority of the current thread.

int Thread::curr_priority()
{
	struct sched_param param;
	int policy;

	::pthread_getschedparam(::pthread_self(), &policy, &param);
	return param.sched_priority;
}

// --------------------------------------------------------------------------
//							Public Interface
// --------------------------------------------------------------------------

// Starts the thread running. It attampts to set the real-time priority
// as requested in the constructor, but if it fails, it does so quietly.

bool Thread::activate()
{
	// ----- Set the priority before creating thread -----

	pthread_attr_t attr;
	struct sched_param param;

	::pthread_attr_init(&attr);

	assert(::pthread_attr_getschedparam(&attr, &param) == 0);
	param.sched_priority = prio_;

	::pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);

	::pthread_attr_setschedpolicy(&attr, DFLT_POLICY);
	::pthread_attr_setschedparam(&attr, &param);
	::pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	
	// ----- Start the thread -----

	active_ = ::pthread_create(&thread_, &attr, startup, this) == 0;
	::pthread_attr_destroy(&attr);
	
	if (!active_) {
		active_ = ::pthread_create(&thread_, 0, startup, this) == 0;
		//TODO: Trace out non-realtime?
	}

	assert(active_);
	return active_;
}

// --------------------------------------------------------------------------
// Sets the priority of the thread.

bool Thread::priority(int prio)
{
	struct sched_param param;
	int policy;

	::pthread_getschedparam(thread_, &policy, &param);
	param.sched_priority = prio_ = prio;

	return ::pthread_setschedparam(thread_, policy, &param) == 0;
}

// --------------------------------------------------------------------------
// Get the priority of the thread.

int Thread::priority() const
{
	struct sched_param param;
	int policy;

	::pthread_getschedparam(thread_, &policy, &param);
	return prio_ = param.sched_priority;
}

// --------------------------------------------------------------------------
// Waits for the thread to complete

void Thread::wait()
{
	if (active_) {
		::pthread_join(thread_, NULL);
		active_ = false;
	}
}

// --------------------------------------------------------------------------
// Waits a bounded time for the thread to complete

bool Thread::wait(const Duration& d)
{
	if (!active_)
		return true;

	if (evtDone_.wait(d)) {
		::pthread_join(thread_, NULL);
		active_ = false;
		return true;
	}
	return false;
}


