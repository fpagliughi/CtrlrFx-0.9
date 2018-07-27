// Thread.cpp
//
// Thread class definition for the eCos RTOS.
//
// PLATFORM:
//		eCos v2
//
// COMPILER:
//		GNU g++ (i386-elf-g++ v. 3.2.1)
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		CtrlrFramework
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		January 2004
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#include "CtrlrFx/os.h"
#include "CtrlrFx/Thread.h"
#include "CtrlrFx/Time.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//								Event
/////////////////////////////////////////////////////////////////////////////
/*
bool Event::Wait(uint32 ms)
{
	if (ms == 0)
		return (bool) ::cyg_flag_poll(&m_flag, FLAG, WAITMODE);

	if (ms == INFINITE) {
		::cyg_flag_wait(&m_flag, FLAG, WAITMODE);
		return true;
	}

	return (bool) ::cyg_flag_timed_wait(&m_flag, FLAG, WAITMODE, Timeout(ms));
}

/////////////////////////////////////////////////////////////////////////////
//							Event Group
/////////////////////////////////////////////////////////////////////////////

uint32 EventGroup::TimedWaitForAll(uint32 pattern, uint32 ms)
{
	if (ms == 0)
		return TryWaitForAll();

	if (ms == INFINITE)
		return WaitForAll();

	return (uint32) ::cyg_flag_timed_wait(&m_flag, pattern, 
											ALL_WAITMODE, Timeout(ms));
}

// --------------------------------------------------------------------------

uint32 EventGroup::TimedWaitForAny(uint32 pattern, uint32 ms)
{
	if (ms == 0)
		return TryWaitForAny();

	if (ms == INFINITE)
		return WaitForAny();

	return (uint32) ::cyg_flag_timed_wait(&m_flag, pattern, 
											ANY_WAITMODE, Timeout(ms));
}

/////////////////////////////////////////////////////////////////////////////
//								MsgQueue
/////////////////////////////////////////////////////////////////////////////

bool MsgQueue::Put(void *p, uint32 ms)
{
	if (ms == 0)
		return (bool) ::cyg_mbox_tryput(m_hBox, p);
	
	if (ms == INFINITE)
		return (bool) ::cyg_mbox_put(m_hBox, p);

	return (bool) ::cyg_mbox_timed_put(m_hBox, p, Timeout(ms));
}

void* MsgQueue::Get(uint32 ms)
{
	if (ms == 0)
		return ::cyg_mbox_tryget(m_hBox);
	
	if (ms == INFINITE)
		return ::cyg_mbox_get(m_hBox);

	return ::cyg_mbox_timed_get(m_hBox, Timeout(ms));
}
*/

/////////////////////////////////////////////////////////////////////////////
//								Thread
/////////////////////////////////////////////////////////////////////////////

Thread::Thread(int prio) : quit_(false)
{
	create(prio, DFLT_STACK_SIZE, "CtrlrFxThread");
}


Thread::Thread(int prio, unsigned stackSize) : quit_(false)
{
	create(prio, stackSize, "CtrlrFxThread");
}

Thread::Thread(int prio, unsigned stackSize, const char *name) : quit_(false)
{
	create(prio, stackSize, name);
}

Thread::~Thread()
{
	::cyg_thread_delete(handle_);
	delete[] stack_;
}

// --------------------------------------------------------------------------

void Thread::create(int priority, unsigned stackSize, const char *name)
{
	stack_ = new char[stackSize];

	::cyg_thread_create((cyg_addrword_t) priority, startup,
						(cyg_addrword_t) this, 
						const_cast<char*>(name), 
						(void*) stack_, stackSize, 
						&handle_, &thread_);
}

// --------------------------------------------------------------------------
//						Static Thread Functions
// --------------------------------------------------------------------------

void Thread::startup(CYG_ADDRESS p)
{
	Thread *pThread = reinterpret_cast<Thread*>(p);

	pThread->run();
	pThread->evtDone_.signal();
	pThread->close();
}

// --------------------------------------------------------------------------

void Thread::sleep(const Duration& d)
{
	::cyg_thread_delay(d.ticks());
}

// --------------------------------------------------------------------------
// Get/Set Current Priority

int Thread::curr_priority()
{
	return (int) ::cyg_thread_get_priority(curr_thread());
}


bool Thread::curr_priority(int prio)
{
	if (prio < PRIORITY_HIGHEST || prio > PRIORITY_LOWEST)
		return false;
	
	::cyg_thread_set_priority(curr_thread(), (cyg_priority_t) prio);
	return true;
}

// --------------------------------------------------------------------------
//							Thread Functions
// --------------------------------------------------------------------------

bool Thread::priority(int prio)
{
	if (prio < PRIORITY_HIGHEST || prio > PRIORITY_LOWEST)
		return false;
	
	::cyg_thread_set_priority(handle_, (cyg_priority_t) prio);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//							Application Startup
/////////////////////////////////////////////////////////////////////////////

// Here, we've chosen to override main() instead of cyg_user_start(). That
// way, main executes in a thread, after the sceduler has been started. This
// makes it compatible with most of the other OS's we're using.
//

int main(int argc, char *argv[])
{
	Thread::curr_priority(Thread::PRIORITY_NORMAL);
	return App::main(argc, argv);
}

