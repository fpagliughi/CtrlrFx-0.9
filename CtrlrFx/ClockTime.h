/// @file ClockTime.h
// 

#ifndef __CtrlrFx_ClockTime_h
#define __CtrlrFx_ClockTime_h

#include <time.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A relative time duration that uses the clock() function from the CRT
/// This is a convenient time

class ClockDuration
{
	typedef long tick_t;

	tick_t ticks_;

public:
	ClockDuration(tick_t ticks) : ticks_(ticks) {}

	tick_t Ticks() const { return ticks_; }

	static uint32 ticks_per_sec() { return CLOCKS_PER_SEC; }

	//static ClockDuration FromMicroseconds(uint32 us);
	static ClockDuration from_ms(uint32_t ms);
	static ClockDuration from_sec(uint32_t s);

	//long us() const { return 1000000L * ticks_ / TicksPerSecond(); }
	long ms() const { return 1000L * ticks_ / ticks_per_sec(); }
	long sec() const { return ticks_ / ticks_per_sec(); }
};

// --------------------------------------------------------------------------

/*
ClockDuration ClockDuration::FromMicroseconds(uint32 us)
{
	return ClockDuration(us * TicksPerSecond() / 1000000L);
}
*/

ClockDuration ClockDuration::from_ms(uint32_t ms)
{
	return ClockDuration(ms * ticks_per_sec() / 1000);
}

ClockDuration ClockDuration::from_sec(uint32_t s)
{
	return ClockDuration(s * ticks_per_sec());
}

/////////////////////////////////////////////////////////////////////////////

class ClockTime
{
	typedef long tick_t;

	tick_t ticks_;

public:
	ClockTime() : ticks_(0) {}
	explicit ClockTime(tick_t ticks) : ticks_(ticks) {}

	static ClockTime now() { return ClockTime((tick_t) clock()); }

	ClockDuration operator-(const ClockTime& rhs);
	ClockTime operator+(const ClockDuration& d);
};

// --------------------------------------------------------------------------

ClockDuration ClockTime::operator-(const ClockTime& rhs)
{
	return ClockDuration(ticks_ - rhs.ticks_);
}

ClockTime ClockTime::operator+(const ClockDuration& d)
{
	return ClockTime(ticks_ + d.Ticks());
}

/////////////////////////////////////////////////////////////////////////////

class ClockTimer
{
	ClockTime time_;

public:
	ClockTimer() : time_(ClockTime::now()) {}

	void reset() { time_ = ClockTime::now(); }

	long elapsed_ms() const;
	long elapsed_sec() const;
};

// --------------------------------------------------------------------------

long ClockTimer::elapsed_ms() const
{
	return (ClockTime::now() - time_).ms();
}

long ClockTimer::elapsed_sec() const
{
	return (ClockTime::now() - time_).sec();
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_ClockTime_h

