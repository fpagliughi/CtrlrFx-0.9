/// @file ITimer.h
/// Base interfaces for OS Timer objects.

#ifndef __CtrlrFx_ITimer_h
#define __CtrlrFx_ITimer_h

#include "CtrlrFx/Time.h"

namespace CtrlrFx {

class ITimer;

/////////////////////////////////////////////////////////////////////////////
/// Interface for Timer Clients.
/// A class that implements this interface can attach itself to a timer and
/// receive timeout events from the timer.

class ITimerClient
{
public:
	/// Handler for a timer event.
	virtual void on_timer(ITimer& tmr) =0;

	/// Virtual destructor
	virtual ~ITimerClient() {}
};

/////////////////////////////////////////////////////////////////////////////

/// Interface for a timer object.

class ITimer
{
public:
	/// Registers a client with this timer.
	/// @param client The client to receive callbacks when the timer expires.
	virtual int register_client(ITimerClient& client) =0;

	/// Starts the timer running using the last delay and period that were set.
	virtual void start() =0;

	/// Starts a periodic timer running with the specified period.
	virtual void start(const Duration& period) =0;

	/// Starts a timer running with the specified 
	virtual void start(const Duration& initDelay, const Duration& period) =0;

	/// Starts the timer as a one-shot.
	/// This is equivalent to calling @ref start with an initial delay and a
	/// period of zero.
	virtual void one_shot(const Duration& timeout) =0;

	/// Stops the timer.
	/// The timer will not send any more events after this operation 
	/// completes, but clients that have input event queues could still have
	/// messages in their input queues from previous events.
	virtual void stop() =0;

	virtual ~ITimer() {}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_ITimer_h

