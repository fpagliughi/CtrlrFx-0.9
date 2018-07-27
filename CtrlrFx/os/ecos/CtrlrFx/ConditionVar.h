// ConditionVar.h

#ifndef __CtrlrFx_ConditionVar_h
#define __CtrlrFx_ConditionVar_h

#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class ConditionVar
{
	cyg_cond_t	cond_;
	Mutex		mutex_;

public:
	ConditionVar();
	ConditionVar(Mutex& mutex);
	~ConditionVar();

	bool	is_valid() const { return true; }
	int		error() const { return 0; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void lock()			{ mutex_.lock(); }
	bool trylock()		{ return mutex_.trylock(); }
	void unlock()		{ mutex_.unlock(); }

	void acquire()		{ lock(); }
	bool tryacquire()	{ return trylock(); }
	void release()		{ unlock(); }

	void wait()			{ ::cyg_cond_wait(&cond_); }
	bool wait(const Duration& d);
	bool wait_until(const Time& t);

	void signal()		{ ::cyg_cond_signal(&cond_); }
	void broadcast()	{ ::cyg_cond_broadcast(&cond_); }
};

// --------------------------------------------------------------------------

inline ConditionVar::ConditionVar()
{
	::cyg_cond_init(&cond_, &mutex_.handle());
}

inline ConditionVar::ConditionVar(Mutex& mutex) : mutex_(mutex.handle())
{
	::cyg_cond_init(&cond_, &mutex_.handle());
}

inline ConditionVar::~ConditionVar()
{
	::cyg_cond_destroy(&cond_);
}

inline bool ConditionVar::wait(const Duration& d)
{
	Time t = Time::from_now(d);
	return ::cyg_cond_timed_wait(&cond_, t.ticks());
}

inline bool ConditionVar::wait_until(const Time& t)
{
	return ::cyg_cond_timed_wait(&cond_, t.ticks());
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_ConditionVar_h

