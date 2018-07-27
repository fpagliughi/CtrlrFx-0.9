// OSEvent.cpp

#include "CtrlrFx/os.h"
#include "CtrlrFx/OSEvent.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//								OSEvent
/////////////////////////////////////////////////////////////////////////////

void OSEvent::wait()
{
	MyGuard g(cond_);
	while (!signaled_)
		cond_.wait();

	if (!manualReset_)
		signaled_ = false;
}

// --------------------------------------------------------------------------

bool OSEvent::wait(const Duration& d)
{
	MyGuard g(cond_);

	while (!signaled_) {
		if (!cond_.wait(d))
			return false;
	}
	if (!manualReset_)
		signaled_ = false;

	return true;
}

// --------------------------------------------------------------------------

bool OSEvent::trywait()
{
	if (!cond_.trylock())
		return false;

	bool ret = signaled_;
	signaled_ = false;
	cond_.unlock();
	return ret;
}

// --------------------------------------------------------------------------

void OSEvent::signal()
{
	MyGuard g(cond_);
	signaled_ = true;

	if (manualReset_)
		cond_.broadcast();
	else
		cond_.signal();
}

// --------------------------------------------------------------------------

void OSEvent::reset()
{
	MyGuard g(cond_);
	signaled_ = false;
}

