// Time.h
// CtrlrFx time classes for Win32

#ifndef __CtrlrFx_Time_h
#define __CtrlrFx_Time_h

#include <ctime>
#include "CtrlrFx/TimeUnits.h"

namespace CtrlrFx {

typedef int64_t	tick_t;

/////////////////////////////////////////////////////////////////////////////

class Duration
{
	tick_t ticks_;

	static const tick_t ONE_K = 1000,
						ONE_M = 1000000;


public:
	Duration(nsec ns) : ticks_(ns) {}
	Duration(usec us) : ticks_(ONE_K * us) {}
	Duration(msec ms) : ticks_(ONE_M * ms) {}
	Duration(sec s) : ticks_(tick_t(1.0e9 * s)) {}
	Duration(uint32_t ms) : ticks_(ONE_M * ms) {}
	Duration(int ms) : ticks_(ONE_M * ms) {}
	Duration(std::time_t sec, long nsec) : ticks_(tick_t(1.0e9 * sec + nsec)) {}
	Duration(const tick_t& ticks) : ticks_(ticks_) {}
	Duration(const Duration& d) : ticks_(d.ticks_) {}

	tick_t ticks() const { return ticks_; }

	nsec_t to_nsec() const { return nsec_t(ticks_); }
	usec_t to_usec() const { return usec_t(ticks_ / ONE_K); }
	msec_t to_msec() const { return msec_t(ticks_ / ONE_M); }
	sec_t  to_sec()  const { return  sec_t(ticks_ / 1.0e9); }
};

/////////////////////////////////////////////////////////////////////////////

class Time
{
	tick_t ticks_;

public:
	Time() : ticks_(0) {}
	Time(const FILETIME& ft);
	Time(const Time& t) : ticks_(t.ticks_) {}
	Time(tick_t t) : ticks_(t) {}

	static Time now();
	static Time from_now(const Duration& d);

	Time& operator=(const Time& t);
	Time& operator+=(const Duration& d);
	Time& operator-=(const Duration& d);

	Time operator+(const Duration& d);
	Time operator-(const Duration& d);

	Duration operator-(const Time& t);
};

// --------------------------------------------------------------------------

inline Time Time::now()
{
	Time t;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	::SystemTimeToFileTime(&sysTime, reinterpret_cast<FILETIME*>(&t.ticks_));

	return t;
}

inline Time Time::from_now(const Duration& d)
{
	int64_t ticks;

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
	::SystemTimeToFileTime(&sysTime, reinterpret_cast<FILETIME*>(&ticks));

	return Time(ticks + d.ticks());
}

inline Time& Time::operator=(const Time& t)
{
	ticks_ = t.ticks_;
	return *this;
}

inline Time& Time::operator+=(const Duration& d)
{
	ticks_ += d.ticks();
	return *this;
}

inline Time& Time::operator-=(const Duration& d)
{
	ticks_ -= d.ticks();
	return *this;
}

inline Time Time::operator+(const Duration& d)
{
	return Time(ticks_ + d.ticks());
}

inline Time Time::operator-(const Duration& d)
{
	return Time(ticks_ - d.ticks());
}

inline Duration Time::operator-(const Time& t)
{
	return Duration(ticks_ - t.ticks_);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Time_h

