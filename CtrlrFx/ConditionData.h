/// @file ConditionData.h
/// A template for guarding and monitoring data with a condition variable.
///
/// @author Frank Pagliughi
/// @date February 2005
///

#ifndef __CtrlrFx_ConditionData_h
#define __CtrlrFx_ConditionData_h

#include "CtrlrFx/ConditionVar.h"
#include "CtrlrFx/Guard.h"
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class ConditionData : public ConditionVar
{
	T val_;	///< The data value to manipulate & monitor

	typedef Guard< ConditionData<T> > MyGuard;

	// Non-copyable
	ConditionData(const ConditionData&);
	ConditionData& operator=(const ConditionData&);

public:
	/// Create a new conditional data object with a default value
	ConditionData() {}

	/// Create a new conditional data object that uses the specified Mutex
	ConditionData(Mutex& mutex) : ConditionVar(mutex) {}

	/// Create a new conditional data object with the specified value
	ConditionData(const T& val) : val_(val) {}

	/// Create a new conditional data object with the specified value
	ConditionData(const T& val, Mutex& mutex) : ConditionVar(mutex), val_(val) {}

	/// Gets the current value
	T value() const { return val_; }

	/// Implicit conversion to value type.
	operator T() const { return val_; }

	using ConditionVar::wait;

	/// Wait (block) until the value matches the one specified.
	void wait(const T& val);

	/// Locks the object, then waits until the value matches the one specified.
	void lock_wait(const T& val);

	/// Wait (block) until the value is different from the one specified.
	void wait_not(const T& val);

	/// Locks the object, then waits until the value is different than the one specified.
	void lock_wait_not(const T& val);

	/// Wait (block) until the value matches the one specified or a timeout 
	/// occurs.
	/// @return
	/// @li true if the value matched
	/// @li false if a timeout occured
	bool wait(const T& val, const Duration& d);

	/// Locks the object and waits until the value matches the one specified or a timeout 
	/// occurs.
	/// @return
	/// @li true if the value matched
	/// @li false if a timeout occured
	bool lock_wait(const T& val, const Duration& d);

	/// Wait (block) until the value is different than the one specified or a
	/// timeout occurs.
	/// @return
	/// @li true if the value matched
	/// @li false if a timeout occured
	bool wait_not(const T& val, const Duration& d);

	/// Locks the object then waits until the value is different than the one specified or a
	/// timeout occurs.
	/// @return
	/// @li true if the value matched
	/// @li false if a timeout occured
	bool lock_wait_not(const T& val, const Duration& d);

	using ConditionVar::signal;

	/// Sets the value and signals the condition variable
	void signal(const T& val);

	/// Locks the object, sets the value, and signals.
	void lock_signal(const T& val);

	using ConditionVar::broadcast;

	/// Sets the value and broadcasts the condition variable
	void broadcast(const T& val);

	/// Locks the object, sets the value, and broadcasts
	void lock_broadcast(const T& val);

	/// Wait (block) until the value matches the matchVal, then atomically
	/// signals the new value.
	void wait_and_signal(const T& matchVal, const T& newVal);

	/// Wait (block) until the value matches the matchVal, then atomically
	/// broadcasts the new value.
	void wait_and_broadcast(const T& matchVal, const T& newVal);

	/// Wait (block) until the value is different than the matchVal, then 
	/// signals the new value.
	void wait_not_and_signal(const T& matchVal, const T& newVal);

	/// Wait (block) until the value is different than the matchVal, then
	/// broadcasts the new value.
	void wait_not_and_broadcast(const T& matchVal, const T& newVal);
};

// --------------------------------------------------------------------------

template <typename T>
void ConditionData<T>::wait(const T& val)
{
	while (val_ != val)
		wait();
}

template <typename T>
inline void ConditionData<T>::lock_wait(const T& val)
{
	MyGuard g(*this);
	wait(val);
}

template <typename T>
void ConditionData<T>::wait_not(const T& val)
{
	while (val_ == val)
		wait();
}

template <typename T>
inline void ConditionData<T>::lock_wait_not(const T& val)
{
	MyGuard g(*this);
	wait_not(val);
}


template <typename T>
bool ConditionData<T>::wait(const T& val, const Duration& d)
{
	while (val_ != val) {
		if (!wait(d))
			return false;
	}
	return true;
}

template <typename T>
inline bool ConditionData<T>::lock_wait(const T& val, const Duration& d)
{
	MyGuard g(*this);
	return wait(val, d);
}


template <typename T>
bool ConditionData<T>::wait_not(const T& val, const Duration& d)
{
	while (val_ == val) {
		if (!wait(d))
			return false;
	}
	return true;
}


template <typename T>
inline bool ConditionData<T>::lock_wait_not(const T& val, const Duration& d)
{
	MyGuard g(*this);
	return wait_not(val, d);
}


template <typename T>
inline void ConditionData<T>::signal(const T& val)
{
	val_ = val;
	signal();
}

template <typename T>
inline void ConditionData<T>::lock_signal(const T& val)
{
	Guard<ConditionVar> g(*this);
	signal(val);
}


template <typename T>
inline void ConditionData<T>::broadcast(const T& val)
{
	val_ = val;
	broadcast();
}

template <typename T>
inline void ConditionData<T>::lock_broadcast(const T& val)
{
	Guard<ConditionVar> g(*this);
	broadcast(val);
}

template <typename T>
void ConditionData<T>::wait_and_signal(const T& matchVal, const T& newVal)
{
	while (val_ != matchVal)
		wait();
	val_ = newVal;
	signal();
}


template <typename T>
void ConditionData<T>::wait_and_broadcast(const T& matchVal, const T& newVal)
{
	while (val_ != matchVal)
		wait();
	val_ = newVal;
	broadcast();
}


template <typename T>
void ConditionData<T>::wait_not_and_signal(const T& matchVal, const T& newVal)
{
	while (val_ == matchVal)
		wait();
	val_ = newVal;
	signal();
}


template <typename T>
void ConditionData<T>::wait_not_and_broadcast(const T& matchVal, const T& newVal)
{
	while (val_ == matchVal)
		wait();
	val_ = newVal;
	broadcast();
}


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif 	// __CtrlrFx_ConditionData_h

