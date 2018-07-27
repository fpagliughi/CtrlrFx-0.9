// AtomicCounter.h
// Generic form of an atomic integer counter.

#ifndef __CtrlrFx_AtomicCounter_h
#define __CtrlrFx_AtomicCounter_h

#include "CtrlrFx/os.h"
#include "CtrlrFx/Mutex.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

template<typename T>
class AtomicCounter
{
	typedef Guard<Mutex> MyGuard;

	volatile T		cnt_;
	mutable Mutex	lock_;

public:
	AtomicCounter() : cnt_(T(0)) {}
	AtomicCounter(T cnt) : cnt_(cnt) {}
	AtomicCounter(const AtomicCounter& src) : cnt_(src.cnt_) {}

	AtomicCounter& operator=(T cnt);
	AtomicCounter& operator=(const AtomicCounter& rhs);

	AtomicCounter& operator+=(T amt);
	operator T() const;

	int operator++();
	int operator++(int);
	int operator--();
	int operator--(int);
};

// --------------------------------------------------------------------------

template<typename T>
inline AtomicCounter<T>& AtomicCounter<T>::operator=(T cnt)
{
	MyGuard g(lock_);
	cnt_ = cnt;
	return *this;
}

template<typename T>
inline AtomicCounter<T>& AtomicCounter<T>::operator=(const AtomicCounter& rhs)
{
	MyGuard g(lock_);
	cnt_ = rhs.cnt_;
	return *this;
}

template<typename T>
inline AtomicCounter<T>& AtomicCounter<T>::operator+=(T amt)
{
	MyGuard g(lock_);
	cnt_ += amt;
	return *this;
}

template<typename T>
inline AtomicCounter<T>::operator T() const
{
	MyGuard g(lock_);
	volatile int cnt = cnt_;
	return cnt;
}

#if !defined(_CTRLR_FX_NONATOMIC_INT)
template <>
inline AtomicCounter<int>::operator int() const
{
	return cnt_;
}
#endif

template<typename T>
inline int AtomicCounter<T>::operator++()
{
	MyGuard g(lock_);
	volatile int cnt = ++cnt_;
	return cnt;
}

template<typename T>
inline int AtomicCounter<T>::operator++(int)
{
	MyGuard g(lock_);
	volatile int cnt = cnt_++;
	return cnt;
}

template<typename T>
inline int AtomicCounter<T>::operator--()
{
	MyGuard g(lock_);
	volatile int cnt = --cnt_;
	return cnt;
}

template<typename T>
inline int AtomicCounter<T>::operator--(int)
{
	MyGuard g(lock_);
	volatile int cnt = cnt_--;
	return cnt;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_AtomicCounter_h

