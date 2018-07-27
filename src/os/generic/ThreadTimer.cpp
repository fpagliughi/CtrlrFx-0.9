// ThreadTimer.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/ThreadTimer.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------

ThreadTimer::ThreadTimer(int prio, ITimerClient& client) 
							: Thread(prio), client_(&client), run_(false)
{
	activate();
}

// --------------------------------------------------------------------------

int ThreadTimer::register_client(ITimerClient& client)
{
	MyGuard g(cond_);
	client_ = &client;
	return 1;
}

// --------------------------------------------------------------------------
// Low-level call to start the timer running by setting the predicate and
// signaling the condition variable. It's assumed that the caller is holding
// the object lock.

void ThreadTimer::do_start()
{
	if (!run_) {
		run_ = true;
		cond_.signal();
	}
}

// --------------------------------------------------------------------------
// Starts the timer using the current (previously set) delay and period 
// values.

void ThreadTimer::start()
{
	MyGuard g(cond_);
	do_start();
}

// --------------------------------------------------------------------------

void ThreadTimer::start(const Duration& period)
{
	MyGuard g(cond_);
	initDelay_ = period_ = period;
	do_start();
}

// --------------------------------------------------------------------------

void ThreadTimer::start(const Duration& initDelay, const Duration& period)
{
	MyGuard g(cond_);
	initDelay_ = initDelay;
	period_ = period;
	do_start();
}

// --------------------------------------------------------------------------

void ThreadTimer::stop()
{
	MyGuard g(cond_);
	run_ = false;
	cond_.signal();
}

// --------------------------------------------------------------------------

void ThreadTimer::one_shot(const Duration& timeout)
{
	MyGuard g(cond_);
	if (!run_) {
		initDelay_ = timeout;
		period_ = Duration(0);
		run_ = true;
		cond_.signal();
	}
	// TODO: Else error?
}

// --------------------------------------------------------------------------
// Request the underlying thread to stop running.

void ThreadTimer::quit()
{
	MyGuard g(cond_);
	quit_ = true;
	run_ = false;
	cond_.signal();
}

// --------------------------------------------------------------------------
// Shuts down the timer thread and waits for it to close.

void ThreadTimer::destroy()
{
	quit();
	wait();
}

// --------------------------------------------------------------------------
// Thread function runs the timer.

int ThreadTimer::run()
{
	MyGuard g(cond_);

	while (!quit_) {
		while (!run_ && !quit_)
			cond_.wait();

		if (quit_)
			break;

		Time t = Time::now();

		if (initDelay_ != Duration(0))
			cond_.wait_until(t += initDelay_);

		while (run_ && !quit_) {
			if (client_)
				client_->on_timer(*this);

			if (period_ == Duration(0))
				run_ = false;
			else
				cond_.wait_until(t += period_);
		}
	}

	return 0;
}

