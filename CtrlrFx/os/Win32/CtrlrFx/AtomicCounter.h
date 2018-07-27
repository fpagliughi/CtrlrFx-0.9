// AtomicCounter.h
//

#ifndef __CtrlrFx_AtomicCounter_h
#define __CtrlrFx_AtomicCounter_h

#include "CtrlrFx\StdWin32.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class AtomicCounter
{
	volatile long val_;

public:
	AtomicCounter() : val_(0L) {}
	AtomicCounter(int val) : val_(val) {}
	AtomicCounter(const AtomicCounter& ctr) : val_(ctr.val_) {}

	operator int() const { return (int) val_; }

	int operator++()	{ return ::InterlockedIncrement(&val_);		}
	int operator++(int)	{ return ::InterlockedIncrement(&val_) - 1;	}
	int operator--()	{ return ::InterlockedDecrement(&val_);		}
	int operator--(int)	{ return ::InterlockedDecrement(&val_) + 1;	}

	AtomicCounter& operator=(int val);
	AtomicCounter& operator=(const AtomicCounter& ctr);

	AtomicCounter& operator+=(int val);
	AtomicCounter& operator+=(const AtomicCounter& ctr);
};

// --------------------------------------------------------------------------

inline AtomicCounter& AtomicCounter::operator=(int val)
{
	::InterlockedExchange(&val_, val); 
	return *this;
}

inline AtomicCounter& AtomicCounter::operator=(const AtomicCounter& ctr)
{
	::InterlockedExchange(&val_, ctr.val_); 
	return *this;
}


inline AtomicCounter& AtomicCounter::operator+=(int val)
{
	::InterlockedExchangeAdd(&val_, val); 
	return *this;
}

inline AtomicCounter& AtomicCounter::operator+=(const AtomicCounter& ctr)
{
	::InterlockedExchangeAdd(&val_, ctr.val_); 
	return *this;
}


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_AtomicCounter_h

