/// @file TimeUnits.h
/// Declaration of CtrlrFx time unit wrapper classes.
/// 
/// Time units are primarily used with the OS-specific time Duration class
/// to specify timeouts for synchronization object, I/O operations, and so
/// on.
///
/// @author Frank Pagliughi
/// @date November 2006

#ifndef __CtrlrFx_TimeUnits_h
#define __CtrlrFx_TimeUnits_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
// Time Unit Types

#if !defined(CFX_TIME_UNITS_DEFINED)
	typedef int64_t nsec_t;
	typedef int32_t usec_t;
	typedef int32_t msec_t;

	#if defined(CFX_NO_FLOAT)
		typedef int32_t sec_t;
		typedef int32_t minute_t;
		typedef int32_t hour_t;
		typedef int32_t day_t;
	#else
		typedef double sec_t;
		typedef double minute_t;
		typedef double hour_t;
		typedef double day_t;
	#endif

	#define CFX_TIME_UNITS_DEFINED
#endif

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of nanoseconds

class nsec
{
	nsec_t ns_;

public:
	nsec(nsec_t ns) : ns_(ns) {}
	operator nsec_t() const { return ns_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of microseconds.

class usec
{
	usec_t us_;

public:
	usec(usec_t us) : us_(us) {}
	operator usec_t() const { return us_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of milliseconds

class msec
{
	msec_t ms_;

public:
	msec(msec_t ms) : ms_(ms) {}
	operator msec_t() const { return ms_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of seconds
/// Seconds are normally represented as a double-precision floating point 
/// number, unless the library is expressly copiled to exclude floating 
/// point by defining @em CFX_NO_FLOAT.

class sec
{
	sec_t sec_;

public:
	sec(sec_t s) : sec_(s) {}
	operator sec_t() const { return sec_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of minutes
/// Minutes are normally represented as a double-precision floating point 
/// number, unless the library is expressly copiled to exclude floating 
/// point by defining @em CFX_NO_FLOAT.

class minutes
{
	minute_t m_;

public:
	minutes(minute_t m) : m_(m) {}
	operator minute_t() const { return m_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of hours.
/// Hours are normally represented as a double-precision floating point 
/// number, unless the library is expressly copiled to exclude floating 
/// point by defining @em CFX_NO_FLOAT.

class hours
{
	hour_t hr_;

public:
	hours(hour_t h) : hr_(h) {}
	operator hour_t() const { return hr_; }
};

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for time units of days.
/// Days are normally represented as a double-precision floating point 
/// number, unless the library is expressly copiled to exclude floating 
/// point by defining @em CFX_NO_FLOAT.

class days
{
	day_t d_;

public:
	days(day_t d) : d_(d) {}
	operator day_t() const { return d_; }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_TimeUnits_h

