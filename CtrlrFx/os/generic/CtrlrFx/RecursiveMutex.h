/// @file RecursiveMutex.h
/// 
/// @author Frank Pagliughi
/// @date June 2005

#ifndef __CtrlrFx_RecursiveMutex_h
#define __CtrlrFx_RecursiveMutex_h

#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/ConditionVar.h"
#include "CtrlrFx/Thread.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class RecursiveMutex
{
	ConditionVar	cond_;		///< Signaled whenever a release occurs
	thread_t		owner_;		///< The thread that currently owns the mutex
	int				level_;		///< The nesting level

public:
	RecursiveMutex() : level_(0) {}
	~RecursiveMutex() {}

	void	lock();
	bool	trylock();
	void	unlock();

	void	acquire()		{ lock(); }
	bool	tryacquire()	{ return trylock(); }
	void	release()		{ unlock(); }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_RecursiveMutex_h

