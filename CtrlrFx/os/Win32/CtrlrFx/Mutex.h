// Mutex.h
//
// Mutex definition for Win32

#ifndef __CtrlrFx_Mutex_h
#define __CtrlrFx_Mutex_h

#include "CtrlrFx\StdWin32.h"
#include "CtrlrFx\Win32WaitHandle.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Mutex 
{
	CRITICAL_SECTION cs_;

public:
	Mutex()  { ::InitializeCriticalSection(&cs_); }
	~Mutex() { ::DeleteCriticalSection(&cs_); }

	void lock()		{ ::EnterCriticalSection(&cs_); }
	void unlock()	{ ::LeaveCriticalSection(&cs_); }

	void acquire()	{ lock(); }
	void release()	{ unlock(); }

	#if _WIN32_WINNT >= 0x0500
		bool trylock()		{ return ::TryEnterCriticalSection(&cs_) != 0; }
		bool tryacquire()	{ return trylock(); }
	#endif
};

/////////////////////////////////////////////////////////////////////////////

class Win32Mutex : public Win32WaitHandle
{
public:
	Win32Mutex() { attach(::CreateMutex(NULL, false, NULL));  }

	void release() { ::ReleaseMutex(handle()); }
};


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Mutex_h

