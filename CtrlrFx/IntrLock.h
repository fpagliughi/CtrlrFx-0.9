/// @file IntrLock.h
/// Mutex-style lock object for systems without an RTOS.

#ifndef __CtrlrFx_IntrLock_h
#define __CtrlrFx_IntrLock_h

#include "CtrlrFx/Interrupt.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A mutex-style lock that uses a processor's global interrup enable/disable.

class GlobalIntrLock
{
public:
	void acquire() 		{ Interrupt::push(); }
	bool tryacquire()	{ Interrupt::push(); return true; }
	void release()		{ Interrupt::pop();  }
};

/////////////////////////////////////////////////////////////////////////////
/// A mutex-style lock that masks and unmasks a single interrupt

class IntrLock
{
	int irq_;	///< The interrupt number

public:
	IntrLock(int irq) : irq_(irq) {}

	void acquire() { Interrupt::mask(irq_); }
	void release() { Interrupt::unmask(irq_);  }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_IntrLock_h

