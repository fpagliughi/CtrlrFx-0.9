/// @file ThreadTimer.h
/// Definition of the @ref ThreadTimer class.

#ifndef __CtrlrFx_ThreadTimer_h
#define __CtrlrFx_ThreadTimer_h

#include "CtrlrFx/Thread.h"
#include "CtrlrFx/ConditionVar.h"
#include "CtrlrFx/Time.h"
#include "CtrlrFx/ITimer.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// A timer object that is guaranteed to run in thread context.

class ThreadTimer : public ITimer, public Thread
{
	static const int DFLT_PRIO = PRIORITY_ABOVE_NORMAL + PRIORITY_INCREASE;

	typedef Guard<ConditionVar> MyGuard;

	ITimerClient*	client_;
	Duration		initDelay_,
					period_;
	ConditionVar	cond_;
	volatile bool	run_;

	void do_start();
	virtual int run();

public:
	ThreadTimer(int prio, ITimerClient& client);
	//ThreadTimer(const Duration& initDelay, const Duration& period);
	//ThreadTimer(const Duration& period);
	~ThreadTimer() { destroy(); }

	void destroy();
	virtual void quit();

	// ----- ITimer Interface -----

	virtual int register_client(ITimerClient& client);

	virtual void start();
	virtual void start(const Duration& period);
	virtual void start(const Duration& initDelay, const Duration& period);

	virtual void stop();

	virtual void one_shot(const Duration& timeout);
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_ThreadTimer_h


