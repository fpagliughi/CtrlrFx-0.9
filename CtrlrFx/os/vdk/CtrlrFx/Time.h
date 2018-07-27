/// @file Time.h
/// Header file for VDK time classes.

#ifndef __CtrlrFx_Time_h
#define __CtrlrFx_Time_h

#include "CtrlrFx/TimeUnits.h"
#include <time.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

#if !defined(USEC_PER_TICK_DEFINED)
	const uint32_t USEC_PER_TICK = 100;		// default 0.1 ms/tick
	#define USEC_PER_TICK_DEFINED
#endif

// Use the OS tick for our basic time increment.
typedef VDK::Ticks tick_t;

/////////////////////////////////////////////////////////////////////////////
//							TimeBase
/////////////////////////////////////////////////////////////////////////////

/**
 * Base for the time classes @ref Duration and @ref Time.
 * In the VDK port, this is a simple wrapper around the VDK::Ticks type.
 */
class TimeBase
{
protected:
	tick_t ticks_;

public:
	TimeBase() {}
	TimeBase(tick_t t) : ticks_(t) {}
	TimeBase(const TimeBase& t) : ticks_(t.ticks_) {}

	tick_t ticks() const { return ticks_; }
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
//							Relative Time
/////////////////////////////////////////////////////////////////////////////

/**
 * Relative time period.
 * The VDK OS uses a simple 32-bit counter for time, which is initialized at
 * system power-up. The default time period is 0.1 ms, though this can be
 * changed by the configuration editor, and set at compile time. At this
 * default rate, the clock will roll over every 4.9 days, so this is the
 * maximum amount of time that can bbe represented by a Duration.
 */
class Duration : public TimeBase
{
public:
	Duration(nsec ns) : TimeBase(ns / (USEC_PER_TICK * 1000)) {}
	Duration(usec us) : TimeBase(us / USEC_PER_TICK) {}
	Duration(msec ms) : TimeBase(1000 * ms / USEC_PER_TICK) {}
	Duration(msec_t ms) : TimeBase(1000 * ms / USEC_PER_TICK) {}
	Duration(sec s)  : TimeBase(1000000L * s / USEC_PER_TICK) {}

	Duration(time_t sec, long nsec) {
		long us = sec * 1000000 + nsec / 1000;
		ticks_ = us / USEC_PER_TICK;
	}

	Duration(const TimeBase& t) : TimeBase(t) {}
	Duration(const Duration& d) : TimeBase(d.ticks_) {}

	nsec_t to_nsec() const {
		return nsec_t(ticks_) * USEC_PER_TICK * 1000L;
	}

	usec_t to_usec() const {
		return usec_t(ticks_) * USEC_PER_TICK;
	}

	msec_t to_msec() const {
		return msec_t(ticks_) * USEC_PER_TICK / 1000;
	}

	sec_t  to_sec() const {
		return sec_t(ticks_) * USEC_PER_TICK / 1000000L;
	}

	Duration& operator=(const Duration& d) {
		ticks_ = d.ticks_;
		return *this;
	}


	#if defined(CFX_NO_FLOAT)
		Duration& operator*=(int scale);
		Duration& operator/=(int scale);
	#else	
		Duration& operator*=(double scale);
		Duration& operator/=(double scale);

		double total_usec() const { return double(ticks_) * USEC_PER_TICK; }
		double total_msec() const { return total_usec() / 1.0e3; }
		double total_sec()  const { return total_usec() / 1.0e6; }
	#endif	

	Duration& operator+=(const Duration& d) {
		ticks_ += d.ticks_;
		return *this;
	}

	Duration& operator-=(const Duration& d) {
		ticks_ -= d.ticks_;
		return *this;
	}
};

// --------------------------------------------------------------------------
// Duration math

#if defined(CFX_NO_FLOAT)
	inline Duration& Duration::operator*=(int scale) {
		ticks_ *= scale;
		return *this;
	}

	inline Duration& Duration::operator/=(int scale) {
		ticks_ /= scale;
		return *this;
	}
#else
	inline Duration& Duration::operator*=(double scale) {
		ticks_ = tick_t(ticks_ * scale + 0.5);
		return *this;
	}

	inline Duration& Duration::operator/=(double scale) {
		ticks_ = tick_t(ticks_ * scale + 0.5);
		return *this;
	}
#endif	// CFX_NO_FLOAT

// --------------------------------------------------------------------------

inline Duration operator+(const Duration& d1, const Duration& d2)
{
	return Duration(TimeBase(d1.ticks() + d2.ticks()));
}

inline Duration operator-(const Duration& d1, const Duration& d2)
{
	return Duration(TimeBase(d1.ticks() - d2.ticks()));
}

inline Duration operator*(const Duration& d, double scale)
{
	tick_t t = tick_t(d.ticks() * scale + 0.5);
	return Duration(TimeBase(t));
}

inline Duration operator*(double scale, const Duration& d)
{
	return operator*(d, scale);
}

inline Duration operator/(const Duration& d, double scale)
{
	tick_t t = tick_t(d.ticks() / scale + 0.5);
	return Duration(TimeBase(t));
}

inline Duration operator/(double scale, const Duration& d)
{
	return operator/(d, scale);
}

/////////////////////////////////////////////////////////////////////////////
//							Absolute Time	
/////////////////////////////////////////////////////////////////////////////

/**
 * Absolute time class.
 * The VDK OS uses a simple 32-bit counter for time, which is initialized at
 * system power-up. The default time period is 0.1 ms, though this can be
 * changed by the configuration editor, and set at compile time. At this
 * default rate, the clock will roll over every 4.9 days, so care must be
 * taken if using absolute time.
 */
class Time : public TimeBase
{
public:
	Time() {}
	Time(const TimeBase& t) : TimeBase(t) {}
	Time(const Time& t) : TimeBase(t.ticks_) {}

	static Time now() {
		return Time(VDK::GetUptime());
	}

	static Time from_now(const Duration& d) {
		return Time(VDK::GetUptime() + d.ticks());
	}

	Time& operator=(const Time& t) {
		ticks_ = t.ticks_;
		return *this;
	}

	Time& operator+=(const Duration& d) {
		ticks_ += d.ticks();
		return *this;
	}

	Time& operator-=(const Duration& d) {
		ticks_ -= d.ticks();
		return *this;
	}
};

// --------------------------------------------------------------------------

inline Time operator+(const Time& t, const Duration& d) {
	return Time(TimeBase(t.ticks() + d.ticks()));
}

inline Time operator+(const Duration& d, const Time& t) {
	return operator+(t, d);
}

inline Time operator-(const Time& t, const Duration& d) {
	return Time(TimeBase(t.ticks() - d.ticks()));
}

inline Duration operator-(const Time& t1, const Time& t2) {
	return Duration(TimeBase(t1.ticks() - t2.ticks()));
}

/////////////////////////////////////////////////////////////////////////////

inline void sleep(const Duration& d)
{
	VDK::Sleep(d.ticks());
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		//  __CtrlrFx_Time_h

