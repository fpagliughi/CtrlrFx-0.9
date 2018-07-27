// Semaphore.cpp

#include "CtrlrFx/StdPlatform.h"
#include "CtrlrFx/Semaphore.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//								Semaphore
/////////////////////////////////////////////////////////////////////////////

bool Semaphore::wait(const Duration& d)
{
	Time t = Time::from_now(d);
	return ::cyg_semaphore_timed_wait(&sem_, t.ticks());
}

bool Semaphore::wait_until(const Time& t)
{
	return ::cyg_semaphore_timed_wait(&sem_, t.ticks());
}

/////////////////////////////////////////////////////////////////////////////
//							Binary Semaphore
/////////////////////////////////////////////////////////////////////////////

bool BinarySemaphore::wait(const Duration& d)
{
	if (d.ticks() == 0)
		return sem_.trywait();

	// Note: No timed wait for binary semaphores

	sem_.wait();
	return true;
}

