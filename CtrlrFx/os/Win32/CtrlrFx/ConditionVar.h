// ConditionVar.h
// Implementation of Condition Variables on Win32

#ifndef __CtrlrFx_ConditionVar_h
#define __CtrlrFx_ConditionVar_h

#include "CtrlrFx\StdWin32.h"
#include "CtrlrFx\Mutex.h"
#include "CtrlrFx\OSEvent.h"
#include "CtrlrFx\Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class ConditionVar
{
	enum { SIGNAL, BROADCAST, N_EVT };

	ManualResetEvent	broadcastEvt_;
	AutoResetEvent		signalEvt_;
	unsigned			nthread_;
	Mutex				&mutex_,
						local_mutex_;

public:
	ConditionVar();
	ConditionVar(Mutex& mutex);
	~ConditionVar();

	void lock()			{ mutex_.lock(); }
	bool trylock()		{ return mutex_.trylock(); }
	void unlock()		{ mutex_.unlock(); }

	void acquire()		{ lock(); }
	bool tryacquire()	{ return trylock(); }
	void release()		{ unlock(); }

	void wait();
	bool wait(const Duration& d);

	void signal();
	void broadcast();
};

// --------------------------------------------------------------------------

inline ConditionVar::ConditionVar() : nthread_(0), mutex_(local_mutex_)
{
}

inline ConditionVar::ConditionVar(Mutex& mutex) : nthread_(0), mutex_(mutex)
{
}

inline ConditionVar::~ConditionVar()
{
}

// --------------------------------------------------------------------------

inline void ConditionVar::wait()
{
	wait(INFINITE);
}

// --------------------------------------------------------------------------
// Wakes up a single waiting thread by signaling the auto-reset event.

inline void ConditionVar::signal()
{
	if (nthread_)
		signalEvt_.signal();
}

// --------------------------------------------------------------------------
// Broadcasts a signal to wake up all waiting threads. It does this by 
// signaling the manual reset event.

inline void ConditionVar::broadcast()
{
	if (nthread_)
		broadcastEvt_.signal();
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_ConditionVar_h
