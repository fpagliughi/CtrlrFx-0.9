/// @file Time.h
/// Posix time classes.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.

// --------------------------------------------------------------------------
// This file is part of CtrlrFx, The Controller Framework.
// Copyright (C) 2001-2005 Frank Pagliughi
// Copyright (C) 2005-2007 SoRo Systems, Inc
//
// CtrlrFx is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// CtrlrFx is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with CtrlrFx; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.

#ifndef __CtrlrFx_Time_h
#define __CtrlrFx_Time_h

#include "CtrlrFx/TimeUnits.h"
#include <time.h>
#include <sys/time.h>

void normalize_timespec(timespec* tm);

namespace CtrlrFx {

/// Type for whole numbers of ticks.
typedef int64_t tick_t;

/////////////////////////////////////////////////////////////////////////////
//							TimeBase
/////////////////////////////////////////////////////////////////////////////

/// Base for the POSIX time classes @ref Duration and @ref Time.
/// This has methods to manipulate the underlying timespec structure.

class TimeBase : public timespec
{
public:
	/**
     * Constructs a timebase.
     * The time is initialized to zero.
	 */
	TimeBase() {
		tv_sec = 0;
		tv_nsec = 0;
	}
	/**
     * Constructs a time with the specified number of seconds and
     * nanoseconds.
     * @param sec The number of seconds in the time.
     * @param nsec The number of nanoseconds in the time.
	 */
	TimeBase(time_t sec, long nsec) {
		tv_sec = sec;
		tv_nsec = nsec;
	}
	/**
     * Constructs a time from a Unix timespec structure.
     * @param tm A Unix timespec structure.
	 */
	TimeBase(const timespec& tm) : timespec(tm) {}
	/**
     * Constructs a time from a Unix timeval structure.
     * @param tv A Unix timeval structure.
	 */
	TimeBase(const timeval& tv) {
		tv_sec = tv.tv_sec;
		tv_nsec = tv.tv_usec * 1000;
	}
	/**
	 * The number of nanoseconds per second.
	 */
	static const long NS_PER_SEC = 1000000000L;
	/**
	 * Normalize the time to a unique value.
     * If the @em nsec component of the timespec is greater then 1.0e9 (1
     * sec), then the excess will be moved into the @em sec component, and
     * the nanoseconds will be adjusted accordingly. This insures that the
     * @em nsec value is always less than 1 sec, and thus the time value is
     *     unique and can be used in a comparison, etc.
	 * @return TimeBase&
	 */
	TimeBase& normalize() {
		normalize_timespec(this);
		return *this;
	}
	/**
	 * Gets the time as a whole number of "ticks"
     * Returns the time as converted to a single whole number, as a 64-bit
     * number of nanoseconds since the epoch. This value is useful for
     * comparison and scalar math, but care must be taken. Unlike most other
     * operating systems, the value isn't incremented by one on each clock
     * tick, but rather by some amount determined by the system clock
     * resolution.
     * @return tick_t The time value in the number of nanoseconds since the
     *         epoch.
	 */
	tick_t ticks() const {
		return tick_t(tv_sec) * NS_PER_SEC + tv_nsec; 
	}
	/**
     * Returns the time as Linux/Unix 'timeval' struct.
     * @return The time as Linux/Unix 'timeval' struct.
	 */
	timeval  to_timeval() const {
		timeval tv;
		tv.tv_sec = tv_sec;
		tv.tv_usec = tv_nsec / 1000;
		return tv;
	}
};

// --------------------------------------------------------------------------
//							Global comparators
// --------------------------------------------------------------------------

inline bool operator==(const TimeBase& t1, const TimeBase& t2)
{
	return t1.tv_sec == t2.tv_sec && t1.tv_nsec == t2.tv_nsec;
}

inline bool operator!=(const TimeBase& t1, const TimeBase& t2)
{
	return t1.tv_sec != t2.tv_sec || t1.tv_nsec != t2.tv_nsec;
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

/// A relative amount of time.
/// This indicates an amount of time, and is generally the difference 
/// between two absolute times. Often it is used to specify an amount of time
/// relative to the current one, like 'wait 5 milliseconds'.

class Duration : public TimeBase
{
	void from_sec(sec_t s);

public:
	/**
     * Constructs
	 */
	Duration() {}

	/**
     * Constructs a duration from a number of nanoseconds.
     * @param ns The number of nanoseconds in the duration.
	 */
	Duration(nsec ns) {
		tv_sec  = ns / NS_PER_SEC;
		tv_nsec = ns % NS_PER_SEC;
	}
	/**
     * Constructs a duration from a number of microseconds.
     * @param ns The number of microseconds in the duration.
	 */
	Duration(usec us) {
		tv_sec  =  us / 1000000;
		tv_nsec = (us % 1000000) * 1000L;
	}
	/**
     * Constructs a duration from a number of milliseconds.
     * @param ns The number of milliseconds in the duration.
	 */
	Duration(msec ms) {
		tv_sec  =  ms / 1000;
		tv_nsec = (ms % 1000) * 1000000L;
	}
	/**
     * Constructs a duration from a number of seconds.
     * @param ns The number of seconds in the duration.
	 */
	Duration(sec s) {
		from_sec(s);
	}
	/**
     * Constructs a duration from a number of minutes.
     * @param m The number of minutes in the duration.
	 */
	Duration(minutes m) {
		from_sec(60*m);
	}
	/**
     * Constructs a duration from a number of hours.
     * @param h The number of hours in the duration.
	 */
	Duration(hours h) {
		from_sec(h*3600);
	}
	/**
     * Constructs a duration from a number of days.
     * @param d The number of days in the duration.
	 */
	Duration(days d) {
		from_sec(86400*d);
	}
	/**
     * Constructs a duration from a number of milliseconds.
     * @param ns The number of milliseconds in the duration.
	 */
	Duration(uint32_t ms) {
		tv_sec  =  ms / 1000;
		tv_nsec = (ms % 1000) * 1000000L;
	}
	/**
     * Constructs a duration from the discrete number of seconds and
     * nanoseconds.
     * @param sec The number of seconds in the duration.
     * @param nsec The number of nanoseconds in the duration.
	 */
	Duration(time_t sec, long nsec) : TimeBase(sec, nsec) {}
	/**
     * Constructs a duration from a Unix timespec object.
     * @param ts The Unix timespec object.
	 */
	explicit Duration(const timespec& ts) : TimeBase(ts) {}
	/**
     * Constructs a duration from a Unix timeval object.
     * @param tv The Unix timespec object.
	 */
	explicit Duration(const timeval& tv) : TimeBase(tv) {}
	/**
     * Copy constructor.
     * @param d The duration to copy.
	 */
	Duration(const Duration& d) : TimeBase(d) {}

	/**
	 * Normalize to produce a unique time.
	 * @see TimeBase::normalize
     * @return A reference to this object.
	 */
	Duration& normalize() {
		normalize_timespec(this);
		return *this;
	}

	/// Gets the Duration in nanoseconds.
	nsec_t to_nsec() const {
		return nsec_t(tv_sec) * NS_PER_SEC + tv_nsec;
	}

	/// Gets the Duration in microseconds.
	usec_t to_usec() const {
		return usec_t(tv_sec) * 1000000 + tv_nsec / 1000;
	}

	/// Gets the Duration in milliseconds.
	msec_t to_msec() const {
		return msec_t(tv_sec) * 1000 + tv_nsec / 1000000L;
	}

	/// Gets the Duration in seconds.
	sec_t to_sec() const {
		return sec_t(tv_sec) + sec_t(tv_nsec) / NS_PER_SEC;
	}

	/// Gets the Duration in minutes.
	minute_t to_minutes() const {
		return to_sec()/60;
	}

	/// Gets the Duration in hours.
	hour_t to_hours() const {
		return to_sec()/3600;
	}

	/// Gets the Duration in days.
	day_t to_days() const {
		return to_sec()/86400;
	}

	/// Assignment operator.
	Duration& operator=(const Duration& d) {
		tv_sec  = d.tv_sec;
		tv_nsec = d.tv_nsec;
		return *this;
	}

	/// Adds a duration to this one.
	Duration& operator+=(const Duration& d) {
		tv_sec  += d.tv_sec;
		tv_nsec += d.tv_nsec;
		return normalize();
	}

	/// Subtracts a duration from this one.
	Duration& operator-=(const Duration& d) {
		tv_sec  -= d.tv_sec;
		tv_nsec -= d.tv_nsec;
		return normalize();
	}

	/// Multiplies this duration by a scalar.
	Duration& operator*=(double scale) {
		tick_t tk = tick_t(ticks() * scale);
		tv_sec  = tk / NS_PER_SEC;
		tv_nsec = tk % NS_PER_SEC;
		return *this;
	}

	/// Divides this duration by a scalar.
	Duration& operator/=(double scale) {
		tick_t tk = tick_t(ticks() / scale);
		tv_sec  = tk / NS_PER_SEC;
		tv_nsec = tk % NS_PER_SEC;
		return *this;
	}
};

// --------------------------------------------------------------------------

inline void Duration::from_sec(sec_t s)
{
	#if defined(CFX_NO_FLOAT)
		tv_sec  = time_t(s);
		tv_nsec = 0;
	#else
		tv_sec  = time_t(long(s));
		tv_nsec = long((s - tv_sec) * NS_PER_SEC);
	#endif
}

// --------------------------------------------------------------------------

/// Adds two Duration values together.
inline Duration operator+(const Duration& d1, const Duration& d2)
{
	Duration dr(d1.tv_sec+d2.tv_sec, d1.tv_nsec+d2.tv_nsec);
	return dr.normalize();
}

/// Subtract one Duration from another.
inline Duration operator-(const Duration& d1, const Duration& d2)
{
	Duration dr(d1.tv_sec-d2.tv_sec, d1.tv_nsec-d2.tv_nsec);
	return dr.normalize();
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

/// Divide a duration by a scalar.
inline Duration operator/(const Duration& d, double scale)
{
	tick_t tk = tick_t(d.ticks() / scale);
	return Duration(nsec(tk));
}

/////////////////////////////////////////////////////////////////////////////
/// 						Absolute Time
/////////////////////////////////////////////////////////////////////////////

template <clockid_t CLOCK>
class TimeTmpl : public TimeBase
{
public:
	TimeTmpl() {}
	TimeTmpl(time_t sec, long nsec) : TimeBase(sec,nsec) {}
	explicit TimeTmpl(const timespec& tm) : TimeBase(tm) {}
	TimeTmpl(const TimeTmpl<CLOCK>& t) : TimeBase(t) {}

	/**
     * Gets the reference clock used for this time.
     * This could be any of the POSIX clocks, such as CLOCK_REALTIME,
     * CLOCK_MONOTONIC, and so on.
     * @return The id of the clock used by this time.
	 */
	static clockid_t clock_id() { return CLOCK; }

	/// Normalize to produce a unique time.
	/// @see TimeBase::normalize
	TimeTmpl<CLOCK>& normalize() {
		normalize_timespec(this);
		return *this;
	}

	/// Gets the current time from the system clock.
	static TimeTmpl<CLOCK> now() {
		TimeTmpl<CLOCK> tm;
		::clock_gettime(CLOCK, &tm);
		return tm;
	}

	/// Gets an absolute time relative to the current time.
	static TimeTmpl<CLOCK> from_now(const Duration& d);

	/// Assignment from a @em timespec
	TimeTmpl<CLOCK>& operator=(const timespec& tm) {
		static_cast<timespec&>(*this) = tm;
		normalize_timespec(this);
		return *this;
	}

	/// Assignment
	TimeTmpl<CLOCK>& operator=(const TimeTmpl<CLOCK>& t) {
		static_cast<timespec&>(*this) = timespec(t);
		return *this;
	}

	/// Adds a duration to this time to produce a new absolute time.
	/// Offets this time into the future.
	TimeTmpl<CLOCK>& operator+=(const Duration& d);

	/// Subtracts a duration from this time to produce a new absolute time.
	/// Offets this time into the past.
	TimeTmpl<CLOCK>& operator-=(const Duration& d);
};

// --------------------------------------------------------------------------

template <clockid_t CLOCK>
TimeTmpl<CLOCK> TimeTmpl<CLOCK>::from_now(const Duration& d)
{
	TimeTmpl<CLOCK> tm;

	::clock_gettime(CLOCK, &tm);
	tm.tv_sec  += d.tv_sec;
	tm.tv_nsec += d.tv_nsec; 
	return tm.normalize();
}

template <clockid_t CLOCK>
inline TimeTmpl<CLOCK>& TimeTmpl<CLOCK>::operator+=(const Duration& d)
{
	tv_sec  += d.tv_sec;
	tv_nsec += d.tv_nsec;
	return normalize();
}

template <clockid_t CLOCK>
inline TimeTmpl<CLOCK>& TimeTmpl<CLOCK>::operator-=(const Duration& d)
{
	tv_sec  -= d.tv_sec;
	tv_nsec -= d.tv_nsec;
	return normalize();
}

// --------------------------------------------------------------------------

template <clockid_t CLOCK>
inline TimeTmpl<CLOCK> operator+(const TimeTmpl<CLOCK>& t, const Duration& d)
{
	TimeTmpl<CLOCK> tm(t.tv_sec+d.tv_sec, t.tv_nsec+d.tv_nsec);
	return tm.normalize();
}

template <clockid_t CLOCK>
inline TimeTmpl<CLOCK> operator+(const Duration& d, const TimeTmpl<CLOCK>& t)
{
	return operator+(t,d);
}

template <clockid_t CLOCK>
inline TimeTmpl<CLOCK> operator-(const TimeTmpl<CLOCK>& t, const Duration& d)
{
	TimeTmpl<CLOCK> tm(t.tv_sec-d.tv_sec, t.tv_nsec-d.tv_nsec);
	return tm.normalize();
}

template <clockid_t CLOCK>
inline Duration operator-(const TimeTmpl<CLOCK>& t1, const TimeTmpl<CLOCK>& t2)
{
    Duration d(t1.tv_sec-t2.tv_sec, t1.tv_nsec-t2.tv_nsec);
	return d.normalize();
}

/////////////////////////////////////////////////////////////////////////////

typedef TimeTmpl<CLOCK_REALTIME>  Time;

#if defined(PLATFORM_LINUX)
	typedef TimeTmpl<CLOCK_MONOTONIC> MonotonicTime;
#endif

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		//  __CtrlrFx_Time_h

