// OSEvent.h
// A set of OS-Events made from condition variables.

#ifndef __CtrlrFx_OSEvent_h
#define __CtrlrFx_OSEvent_h

#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/Guard.h"
#include "CtrlrFx/ConditionVar.h"
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class OSEvent
{
protected:
	typedef Guard<ConditionVar> MyGuard;

	ConditionVar	cond_;
	bool            manualReset_,
					signaled_;

	// Non-copyable
	OSEvent(const OSEvent&);
	OSEvent& operator=(const OSEvent&);

public:
	static const bool AUTO_RESET = false;
	static const bool MANUAL_RESET = true;

	OSEvent(bool manualReset, bool signaled=false);

	bool	is_valid() const { return cond_.is_valid(); }
	int		error() const { return cond_.error(); }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	wait();
	bool	trywait();
	void	signal();
	void	reset();

	bool	wait(msec_t ms)	{ return wait(Duration(msec(ms))); }
	bool	wait(usec us)	{ return wait(Duration(us)); }
	bool	wait(msec ms)	{ return wait(Duration(ms)); }
	bool	wait(sec s)		{ return wait(Duration(s)); }

	bool	wait(const Duration& d);
};

// --------------------------------------------------------------------------

inline OSEvent::OSEvent(bool manualReset, bool signaled /*=false*/)
			: manualReset_(manualReset), signaled_(signaled)
{
}

/////////////////////////////////////////////////////////////////////////////

class ManualResetEvent : public OSEvent
{
public:
	ManualResetEvent() : OSEvent(MANUAL_RESET) {}
	ManualResetEvent(bool signaled) : OSEvent(MANUAL_RESET, signaled) {}
};

/////////////////////////////////////////////////////////////////////////////

class AutoResetEvent : public OSEvent
{
public:
	AutoResetEvent() : OSEvent(AUTO_RESET) {}
	AutoResetEvent(bool signaled) : OSEvent(AUTO_RESET, signaled) {}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_OSEvent_h

