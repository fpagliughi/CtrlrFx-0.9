// RecursiveMutex.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/RecursiveMutex.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------
// Lock the mutex:
//		- If no one owns it, grab it.
//		- If we own it, just increment the level
//		- If someone else owns it, wait for it

void RecursiveMutex::lock()
{
	thread_t curr = Thread::curr_thread();

	Guard<ConditionVar> g(cond_);

	if (level_ == 0)
		owner_ = curr;
	else if (owner_ != curr) {
		while (level_ > 0)
			cond_.wait();
		owner_ = curr;
	}
	++level_;
}

// --------------------------------------------------------------------------
// Try to lock the mutex, but fail immediately if someone else owns it:
//		- If no one owns it, grab it.
//		- If we own it, just increment the level
//		- If someone else owns it, fail

bool RecursiveMutex::trylock()
{
	thread_t curr = Thread::curr_thread();

	if (!cond_.trylock())
		return false;

	Guard<ConditionVar> g(cond_, true);

	if (level_ == 0)
		owner_ = curr;
	else if (owner_ != curr)
		return false;

	++level_;
	return true;
}

// --------------------------------------------------------------------------
// Release the mutex - or at least one level of it.
// If this thread really owns the mutex, decrement the level. If it goes to
// zero, signal any waiting threads that the mutex is available.

void RecursiveMutex::unlock()
{
	Guard<ConditionVar> g(cond_);

	if (level_ > 0 && owner_ == Thread::curr_thread()) {
		if (--level_ == 0)
			cond_.signal();
	}
}



