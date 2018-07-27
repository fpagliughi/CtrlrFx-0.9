// Event.h
// OSEvent definitions for Win32

#ifndef __CtrlrFx_OSEvent_h
#define __CtrlrFx_OSEvent_h

#include "CtrlrFx\StdWin32.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class OSEvent : public Win32WaitHandle
{
public:
	static const bool AUTO_RESET = false;
	static const bool MANUAL_RESET = true;

	OSEvent(bool manualReset, bool signaled=false);

	void signal() { ::SetEvent(handle()); }
	void reset() { ::ResetEvent(handle()); }
};

// --------------------------------------------------------------------------

inline OSEvent::OSEvent(bool manualReset, bool signaled /*=false*/)
{
	attach(::CreateEvent(NULL, manualReset, signaled, NULL));
}

/////////////////////////////////////////////////////////////////////////////

class ManualResetEvent : public OSEvent
{
public:
	ManualResetEvent() : OSEvent(MANUAL_RESET) {}
	ManualResetEvent(bool signaled) : OSEvent(MANUAL_RESET, signaled) {}
};

/////////////////////////////////////////////////////////////////////////////

class AutoResetEvent : public OSEvent
{
public:
	AutoResetEvent() : OSEvent(AUTO_RESET) {}
	AutoResetEvent(bool signaled) : OSEvent(AUTO_RESET, signaled) {}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_OSEvent_h

