// Timer.cpp
// Definition of eCos Timer object

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Timer.h"
//#include <assert.h>

using namespace CtrlrFx;

// --------------------------------------------------------------------------
//							Constructor(s)/Destructor
// --------------------------------------------------------------------------

Timer::Timer()
{
	create(0);
}

Timer::Timer(ITimerClient& client)
{
	create(&client);
}

Timer::Timer(ITimerClient& client, const Duration& period)
{
	create(&client);
	Time tm = Time::from_now(period);
	::cyg_alarm_initialize(handle_, tm.ticks(), period.ticks());
}

Timer::Timer(ITimerClient& client, const Duration& initDelay, const Duration& period)
{
	create(&client);
	Time tm = Time::from_now(initDelay);
	::cyg_alarm_initialize(handle_, tm.ticks(), period.ticks());
}

// --------------------------------------------------------------------------
// Creates the underlying OS timer object.

void Timer::create(ITimerClient* client)
{
	cyg_handle_t	hcounter,
					sys_clk = ::cyg_real_time_clock();

	::cyg_clock_to_counter(sys_clk, &hcounter);
	::cyg_alarm_create(hcounter, handler, (cyg_addrword_t) this,
						&handle_, &alarm_);
	client_ = client;
}

// --------------------------------------------------------------------------
//								Handlers
// --------------------------------------------------------------------------

// The static handler for the whole class. The address/pointer is the 
// Timer object that should handle the event. We call its on_timer() 
// function.

void Timer::handler(cyg_handle_t, cyg_addrword_t p)
{
	Timer* timer = reinterpret_cast<Timer*>(p);
	//assert(timer->client_ != 0);
	timer->client_->on_timer(*timer);
}

// --------------------------------------------------------------------------
//							ITimer Interface
// --------------------------------------------------------------------------

int Timer::register_client(ITimerClient& client)
{
	// TODO: This clobbers any previous client
	client_ = &client;
	return 1;
}

// --------------------------------------------------------------------------

void Timer::start()
{
	::cyg_alarm_enable(handle_);
}

// --------------------------------------------------------------------------

void Timer::start(const Duration& period)
{
	Time tm = Time::from_now(period);
	::cyg_alarm_initialize(handle_, tm.ticks(), period.ticks());
}

// --------------------------------------------------------------------------

void Timer::start(const Duration& initDelay, const Duration& period)
{
	Time tm = Time::from_now(initDelay);
	::cyg_alarm_initialize(handle_, tm.ticks(), period.ticks());
}

// --------------------------------------------------------------------------

void Timer::one_shot(const Duration& timeout)
{
	Time tm = Time::from_now(timeout);
	::cyg_alarm_initialize(handle_, tm.ticks(), 0);
}

// --------------------------------------------------------------------------

void Timer::stop()
{
	::cyg_alarm_disable(handle_);
}


