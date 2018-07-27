// Time.h
// CtrlrFx time classes for eCos

#ifndef __CtrlrFx_Time_h
#define __CtrlrFx_Time_h

#include "CtrlrFx/StdPlatform.h"
#include "CtrlrFx/TimeUnits.h"
#include <ctime>

namespace CtrlrFx {

typedef cyg_tick_count_t	tick_t;

/////////////////////////////////////////////////////////////////////////////
/// An eCos clock object.

class Clock
{
protected:
	cyg_resolution_t	res_;
	cyg_handle_t		handle_;
	cyg_clock			clk_;

	void create(uint32_t dividend, uint32_t divisor);
	Clock() {}

public:
	Clock(uint32_t dividend, uint32_t divisor);
	Clock(uint32_t res);

	~Clock();

	cyg_handle_t handle() { return handle_; }
	operator cyg_handle_t() { return handle_; }

	nsec_t ticks_to_ns(tick_t ticks);
	tick_t ns_to_ticks(nsec_t ns);
};

// --------------------------------------------------------------------------

inline Clock::Clock(uint32_t dividend, uint32_t divisor)
{
	create(dividend, divisor);
}

inline Clock::Clock(uint32_t res)
{
	create(res, 1);
}

inline Clock::~Clock()
{
	::cyg_clock_delete(handle_);
}

inline nsec_t Clock::ticks_to_ns(tick_t ticks)
{
	return nsec_t(ticks * res_.dividend / res_.divisor);
}

inline tick_t Clock::ns_to_ticks(nsec_t ns)
{
	return tick_t(ns * res_.divisor / res_.dividend);
}

/////////////////////////////////////////////////////////////////////////////
/// The eCos System Clock
/// Utility class to convert between physical time and clock ticks. It also
/// knows the handle to the underlying system clock and can query for the
/// current system time. This is a fully static class definition to wrap 
/// the eCos System clock.

class SysClock
{
public:
	SysClock();

	static tick_t now();
	static nsec_t ticks_to_ns(tick_t ticks);
	static tick_t ns_to_ticks(nsec_t ns);

	static cyg_handle_t handle() { return ::cyg_real_time_clock(); }
};

// --------------------------------------------------------------------------

inline tick_t SysClock::now()
{
	return tick_t(::cyg_current_time());
}

inline nsec_t SysClock::ticks_to_ns(tick_t ticks)
{
	return ticks * CYGNUM_HAL_RTC_NUMERATOR / CYGNUM_HAL_RTC_DENOMINATOR;
}

inline tick_t SysClock::ns_to_ticks(nsec_t ns)
{
	return ns * CYGNUM_HAL_RTC_DENOMINATOR / CYGNUM_HAL_RTC_NUMERATOR;
}

/////////////////////////////////////////////////////////////////////////////
/// Base class for the absolute and relative times.
/// In eCos, the basic time quantum is the system clock tick. Constants in
/// the platform's HAL tell the number of nanoseconds per clock tick.

class TimeBase
{
protected:
	tick_t ticks_;	///< The time as number of system clock ticks (not ns)

	/// Convenience function to convert nanoseconds to ticks.
	tick_t to_ticks(nsec_t ns) { return SysClock::ns_to_ticks(ns); }

	static const tick_t ONE_T = 1000,
						ONE_M = ONE_T * 1000,
						ONE_B = ONE_M * 1000;

public:
	TimeBase() : ticks_(0) {}
	TimeBase(tick_t ticks) : ticks_(ticks) {}
	TimeBase(const TimeBase& tb) : ticks_(tb.ticks_) {}

	/// Get the number of system clock ticks in this time.
	tick_t ticks() const { return ticks_; }

	/// Get the number of nanoseconds in this time.
	nsec_t ns() const { return SysClock::ticks_to_ns(ticks_); }
};

// --------------------------------------------------------------------------

inline bool operator==(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() == t2.ticks();
}

inline bool operator!=(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() != t2.ticks();
}

inline bool operator<(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() < t2.ticks();
}

inline bool operator<=(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() <= t2.ticks();
}

inline bool operator>(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() > t2.ticks();
}

inline bool operator>=(const TimeBase& t1, const TimeBase& t2)
{
	return t1.ticks() >= t2.ticks();
}

/////////////////////////////////////////////////////////////////////////////
/// A relative amount of time.

class Duration : public TimeBase
{
public:
	Duration() {}
	Duration(nsec ns) : TimeBase(to_ticks(ns)) {}
	Duration(usec us) : TimeBase(to_ticks(ONE_T * us)) {}
	Duration(msec ms) : TimeBase(to_ticks(ONE_M * ms)) {}
	Duration(sec s)   : TimeBase(to_ticks(nsec_t(ONE_B * s))) {}
	Duration(int ms)  : TimeBase(to_ticks(ONE_M * ms)) {}
	Duration(uint32_t ms) : TimeBase(to_ticks(ONE_M * ms)) {}
	Duration(std::time_t sec, long nsec) : TimeBase(to_ticks(tick_t(ONE_B * sec + nsec))) {}
	Duration(const tick_t& ticks) : TimeBase(ticks_) {}
	Duration(const TimeBase& tb) : TimeBase(tb) {}
	Duration(const Duration& d) : TimeBase(d) {}

	nsec_t to_nsec() const { return ns(); }
	usec_t to_usec() const { return usec_t(ns() / ONE_T); }
	msec_t to_msec() const { return msec_t(ns() / ONE_M); }
	sec_t  to_sec()  const { return  sec_t(ns() / ONE_B); }

	/// Adds a duration to this one.
	Duration& operator+=(const Duration& d);

	/// Subtracts a duration from this one.
	Duration& operator-=(const Duration& d);

	#if !defined(CFX_NO_FLOAT)
	/// Multiplies this duration by a scalar.
	Duration& operator*=(double scale);

	/// Divides this duration by a scalar.
	Duration& operator/=(double scale);
	#endif
};

// --------------------------------------------------------------------------

inline Duration& Duration::operator+=(const Duration& d)
{
	ticks_ += d.ticks_;
	return *this;
}

inline Duration& Duration::operator-=(const Duration& d)
{
	ticks_ -= d.ticks_;
	return *this;
}

#if !defined(CFX_NO_FLOAT)

inline Duration& Duration::operator*=(double scale)
{
	ticks_ = tick_t(ticks_ * scale);
	return *this;
}

inline Duration& Duration::operator/=(double scale)
{
	ticks_ = tick_t(ticks_ / scale);
	return *this;
}

/// Multiply a duration by a scalar.
inline Duration operator*(const Duration& d, double scale)
{
	tick_t tk = tick_t(d.ticks() * scale);
	return Duration(nsec(tk));
}

/// Multiply a duration by a scalar.
inline Duration operator*(double scale, const Duration& d)
{
	return operator*(d, scale);
}

#endif		// CFX_NO_FLOAT

/////////////////////////////////////////////////////////////////////////////
/// An absolute time.

class Time : public TimeBase
{
public:
	Time() {}
	Time(const TimeBase& tb) : TimeBase(tb) {}
	Time(const Time& t) : TimeBase(t) {}
	Time(tick_t t) : TimeBase(t) {}

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
	return Time(SysClock::now());
}

inline Time Time::from_now(const Duration& d)
{
	return Time(SysClock::now() + d.ticks());
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

