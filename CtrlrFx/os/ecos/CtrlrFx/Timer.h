/// @file Timer.h

#ifndef __CtrlrFx_Timer_h
#define __CtrlrFx_Timer_h

#include "CtrlrFx/StdPlatform.h"
#include "CtrlrFx/ITimer.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Timer : virtual public ITimer
{
protected:
	cyg_handle_t	handle_;	///< Handle to the OS timer object
	cyg_alarm		alarm_;		///< Memory for the timer object

	ITimerClient*	client_;	///< Timer client to receive timer events
	Duration		initDelay_,	///< The delay before the first event
					period_;	///< The time between subsequent, periodic, events.	

	/// The callback for the OS timer object.
	static void handler(cyg_handle_t halarm, cyg_addrword_t p);

	/// Creates the OS timer object.
	void create(ITimerClient* client);

public:
	/// Constructor
	Timer();

	/// Constructs a timer with the specified client
	Timer(ITimerClient& client);

	/// Constructs a timer and starts it running with the specified period.
	Timer(ITimerClient& client, const Duration& period);

	/// Starts a timer running with the specified 
	Timer(ITimerClient& client, const Duration& initDelay, const Duration& period);

	/// Destructor
	~Timer() { ::cyg_alarm_delete(handle_); }

	/// Registers a client with this timer.
	/// @param client The client to receive callbacks when the timer expires.
	virtual int register_client(ITimerClient& client);

	/// Starts the timer running using the last delay and period that were set.
	virtual void start();

	/// Starts a periodic timer running with the specified period.
	virtual void start(const Duration& period);

	/// Starts a timer running with the specified 
	virtual void start(const Duration& initDelay, const Duration& period);

	/// Starts the timer as a one-shot.
	/// This is equivalent to calling @ref start with an initial delay and a
	/// period of zero.
	virtual void one_shot(const Duration& timeout);

	/// Stops the timer.
	/// The timer will not send any more events after this operation 
	/// completes, but clients that have input event queues could still have
	/// messages in their input queues from previous events.
	virtual void stop();
};

////////////////////////////////////////////////////////////////////////////

/// Timer object that expires only once.

class OneShot : public Timer
{
	typedef Timer Base;

public:
	OneShot() {}

	OneShot(ITimerClient& client, const Duration& timeout)
		: Base(client, timeout, Duration(0)) {}

	virtual void start() { Base::start(); }

	/// Starts a periodic timer running with the specified period.
	virtual void start(const Duration& timeout) {
		Base::one_shot(timeout);
	}
};

////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Timer_h

