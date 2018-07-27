/// @file Win32.h
/// Implementation of the OS Thread and synchronization primitives for Win32.

//
// PLATFORM:
//		Win32
//
// AUTHOR:
//		Frank Pagliughi
//
// DATE:
//		May 2001
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2001, SoRo Systems, Inc
//		All Rights Reserved
//

#ifndef __CtrlrFx_Win32_h
#define __CtrlrFx_Win32_h

#if !defined(WIN32)
	#error "This is a Win32 implementation file: requires WIN32 defined"
#endif

#include <climits>
#include "CtrlrFx\StdWin32.h"
#include "CtrlrFx\Win32WaitHandle.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Thread : public Win32WaitHandle
{
public:
	typedef unsigned (__stdcall *ThreadFunc)(void *);

	static const unsigned DFLT_STACK_SIZE = 4096;

	static const int	PRIORITY_HIGHEST		= THREAD_PRIORITY_HIGHEST,
						PRIORITY_ABOVE_NORMAL	= THREAD_PRIORITY_ABOVE_NORMAL,
						PRIORITY_NORMAL			= THREAD_PRIORITY_NORMAL,
						PRIORITY_BELOW_NORMAL	= THREAD_PRIORITY_BELOW_NORMAL,
						PRIORITY_LOWEST			= THREAD_PRIORITY_LOWEST,
						PRIORITY_IDLE			= THREAD_PRIORITY_IDLE;

	static const int	PRIORITY_INCREASE		= +1,
						PRIORITY_DECREASE		= -1;

protected:
	volatile bool quit_;

	Thread(int prio);
	Thread(int prio, unsigned stackSize);
	Thread(int prio, unsigned stackSize, const char *name);

	static int __stdcall startup(void *p);
	void create(unsigned stackSize);

	virtual void close() {}
	virtual int  run() =0;

public:
	// ----- Static Functions: Apply to Current Thread -----

	static void		sleep(const Duration& d);

	static bool		curr_priority(int prio);
	static int		curr_priority();

	// ----- Public Interface -----

	bool	activate();

	bool	priority(int prio);
	int		priority() const;

	virtual void quit() { quit_ = true; }
};

// --------------------------------------------------------------------------

inline void Thread::sleep(const Duration& d)
{
	::Sleep(d.to_msec());
}

inline bool Thread::curr_priority(int prio)
{
	return ::SetThreadPriority(::GetCurrentThread(), prio) != 0;
}

inline int Thread::curr_priority()
{
	return ::GetThreadPriority(::GetCurrentThread());
}

// --------------------------------------------------------------------------

inline bool Thread::activate()
{
	return ::ResumeThread(handle()) != -1;
}

// --------------------------------------------------------------------------

inline bool Thread::priority(int prio)
{
	return ::SetThreadPriority(handle(), prio) != 0;
}

inline int Thread::priority() const
{
	return ::GetThreadPriority(handle());
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		//  __OSFactoryWin32_h

