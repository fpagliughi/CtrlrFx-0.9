// OSTest.cpp
//
// Test of the RTOS functions and OS Factory from the CtrlrFramework
//
// PROJECT:
//		CtrlrFramework	(Test Suite)
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		January 2003
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/debug.h"

#include <stdlib.h>

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------
// Global Vars (Sync Objs)

// Uncomment the type of lockable object(s) to test
//#define _TEST_CLOCK_TIME
#define _TEST_MUTEX
#define _TEST_RECURSIVE_MUTEX
#define _TEST_SEM
#define _TEST_COND_VAR
#define _TEST_RDWR_LOCK
#define _TEST_EVENT
//#define _TEST_ATOMIC_COUNTER
//#define _TEST_MSG_QUEUE
//#define _TEST_MSG_SLOT
//#define _TEST_PERIODIC_THREAD
//#define _TEST_TIMER

#if defined(_TEST_CLOCK_TIME)
	#include "CtrlrFx/ClockTime.h"
#endif

#if defined(_TEST_MUTEX) || defined(_TEST_COND_VAR)
	Mutex mutex;
#endif

#if defined(_TEST_RECURSIVE_MUTEX)
	RecursiveMutex rmutex;
#endif

#if defined(_TEST_SEM)
	Semaphore sem;
#endif

#if defined(_TEST_COND_VAR)
	#include "CtrlrFx/ConditionVar.h"
	#include "CtrlrFx/ConditionData.h"
	
	ConditionVar condVar(mutex);
	bool condSignaled = false;

	ConditionData<int> condData(0);
#endif

#if defined(_TEST_RDWR_LOCK)
	#include "CtrlrFx/RdWrLock.h"
	RdWrLock rwLock;
#endif

#if defined(_TEST_EVENT)
	AutoResetEvent evt;
#endif

#if defined(_TEST_ATOMIC_COUNTER)
	#include "CtrlrFx/AtomicCounter.h"
#endif

#if defined(_TEST_MSG_QUEUE)
	#include "CtrlrFx/MsgQueue.h"
	const int	MSG_QUE_CAP = 5,
				N_MSG = 10;
	MsgQueue<int> que(MSG_QUE_CAP);
#endif

#if defined(_TEST_MSG_SLOT)
	#include "CtrlrFx/MsgSlot.h"
	MsgSlot<int> msgSlot;
#endif

#if defined(_TEST_TIMER)
	#include "CtrlrFx/Timer.h"
#endif

/////////////////////////////////////////////////////////////////////////////
//						Second (Test) Thread
/////////////////////////////////////////////////////////////////////////////

class TestThread : public Thread
{
public:
	TestThread(int prio);

protected:
	virtual int run();
};

// --------------------------------------------------------------------------

TestThread::TestThread(int prio)
						: Thread(prio, DFLT_STACK_SIZE, "TestThread")
{
	activate();
}

// --------------------------------------------------------------------------

int TestThread::run()
{
	DPRINTF("[TestThread] Thread Started.\n");

	int n = Thread::curr_priority();
	DPRINTF("\t[TestThread] Priority is: %d\n\n", n);


	#if defined(_TEST_MUTEX)
		DPRINTF("[TestThread] Waiting for *** Mutex ***\n");
		mutex.acquire();
		sleep(100);
		mutex.release();

		DPRINTF("[TestThread] Waiting for *** Guard ***\n");
		{
			Guard<Mutex> g(mutex);
			sleep(100);
		}
		DPRINTF("[TestThread] Mutex Tests Complete.\n\n");
	#endif

	#if defined(_TEST_RECURSIVE_MUTEX)
		DPRINTF("[TestThread] Waiting for *** Recursive Mutex ***\n");
		rmutex.acquire();
		sleep(100);
		rmutex.release();
		DPRINTF("[TestThread] Recursive Mutex Tests Complete.\n\n");
	#endif 

	#if defined(_TEST_SEM)
		if (sem) {
			DPRINTF("[TestThread] Waiting for *** Semaphore ***\n");
			if (!sem.acquire(sec(2.5))) {	//(!sem.acquire(2500)) {
				DPRINTF("\t[TestThread] Semaphore Timeout (expected)\n");
				sem.acquire();
				DPRINTF("\t[TestThread] Got Semaphore\n");
			}

			sleep(100);
			sem.release();
		}
	#endif

	#if defined(_TEST_COND_VAR)
		DPRINTF("[TestThread] Waiting for *** Condition Var ***\n");
		{
			Guard<ConditionVar> g(condVar);
			while (!condSignaled) {
				if (!condVar.wait(sec(2.0))) {
					DPRINTF("[ThreadTest] ConditionVar Timeout (expected 2x)\n");
				}
			}
		}
		DPRINTF("[TestThread] Waiting for *** Condition Data ***\n");
		condData.lock_wait(5);
		DPRINTF("[TestThread] Condition Variable Tests Complete\n\n");
	#endif
		
	#if defined(_TEST_RDWR_LOCK)
		DPRINTF("[TestThread] Waiting for *** Read Lock ***\n");
		rwLock.rd_lock();
		DPRINTF("[TestThread] Waiting for *** Read Lock ***\n");
		rwLock.rd_lock();
		DPRINTF("[TestThread] Waiting for *** Read Lock ***\n");
		rwLock.rd_lock();
		DPRINTF("[TestThread] R/W Lock Tests Complete\n\n");
	#endif
		
	#if defined(_TEST_EVENT)
		DPRINTF("[TestThread] Waiting for *** OSEvent ***\n");
		if (!evt.wait(2500))
			DPRINTF("\t[TestThread] Timeout (expected)\n");
		evt.wait();
		DPRINTF("[TestThread] OSEvent Tests Complete.\n\n");
	#endif


	#if defined(_TEST_MSG_QUEUE)
		DPRINTF("[TestThread] Sleeping 2.5 sec...\n");
		sleep(2500);

		DPRINTF("[TestThread] Filling Que...\n");

		for (int i=0; i<N_MSG; ++i) {
			int n = i+1;	//*2;
			DPRINTF("\t[TestThread] In: %2d\n", n);
			que.put(n);
			//sleep(500);
		}
	#endif

	#if defined(_TEST_MSG_SLOT)
		DPRINTF("[TestThread] Waiting for *** MsgSlot ***\n");
		int x = msgSlot.get();
		DPRINTF("\t[TestThread] Got: %d from the MsgSlot\n", x);
	#endif
	
	DPRINTF("[TestThread] Sleeping for 5 sec...\n");
	sleep(sec(5.0));
	DPRINTF("[TestThread] Done... Exiting.\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// 

#if defined(_TEST_PERIODIC_THREAD)

class PeriodicThread : public Thread
{
    Semaphore quitSem_;

	virtual int run() {
		const Duration period(msec(500));
		Time tm = Time::from_now(period);
		unsigned n = 0; 

		while (!quitSem_.wait_until(tm)) {
			tm += period;
			DPRINTF("[PeriodicThread] Ping  %u\n", ++n);
		}
		return 0;
	}

public:
	PeriodicThread(int prio) : Thread(prio, DFLT_STACK_SIZE, "PeriodicThread") {
		activate();
	}

	virtual void quit() {
		quitSem_.post();
	}
};

#endif

/////////////////////////////////////////////////////////////////////////////
//								Timer Client
/////////////////////////////////////////////////////////////////////////////

#if defined(_TEST_TIMER)

class TimerClient : virtual public ITimerClient
{
	AutoResetEvent ready_;

public:
	TimerClient() {}

	virtual void on_timer(ITimer &tmr) {
		ready_.signal();
	}

	void wait() {
		ready_.wait();
	}
};

#endif

// --------------------------------------------------------------------------
//								Main
// --------------------------------------------------------------------------

int App::main(int, char **)
{
	#if defined(_TRACE) && !defined(_TRACE_STDOUT)
		TRACE_OPEN("/dev/ser0");
	#endif

	DPRINTF("\fOSTest.exe  Test of the OS Components of the CtrlrFramework\n\n");
	
	int n = Thread::curr_priority();
	DPRINTF("[Main] The main startup priority is: %d\n", n);

	Thread::curr_priority(Thread::PRIORITY_BELOW_NORMAL);
	n = Thread::curr_priority();

	DPRINTF("[Main] The new, lower, priority is: %d\n\n", n);

	// ----- Realtime clock -----

	Time t = Time::now();
	DPRINTF("[Main] Real time tick is now:        %lu  (%d,%ld)\n", 
			(ulong) ((t.ticks()/1000) % 1000000000L), (int) t.tv_sec, (long) t.tv_nsec);
	t = Time::from_now(msec(2000));
	DPRINTF("[Main] Sleep until 2.5 sec from now: %lu  (%d,%ld)\n", 
			(ulong) ((t.ticks()/1000) % 1000000000L), (int) t.tv_sec, (long) t.tv_nsec);
	Thread::sleep_until(t);
	t = Time::now();
	DPRINTF("[Main] Real time tick is now:        %lu  (%d,%ld)\n\n",
			(ulong) ((t.ticks()/1000) % 1000000000L), (int) t.tv_sec, (long) t.tv_nsec);

	// ----- Monotonic clock -----

	MonotonicTime mt = MonotonicTime::now();
	DPRINTF("[Main] Monotonic time tick is now:   %lu  (%d,%ld)\n", 
			(ulong) ((mt.ticks()/1000) % 1000000000L), (int) mt.tv_sec, (long) mt.tv_nsec);
	mt = MonotonicTime::from_now(msec(2000));
	DPRINTF("[Main] Sleep until 2.5 sec from now: %lu  (%d,%ld)\n",
			(ulong) ((mt.ticks()/1000) % 1000000000L), (int) mt.tv_sec, (long) mt.tv_nsec);
	Thread::sleep_until(mt);
	mt = MonotonicTime::now();
	DPRINTF("[Main] Monotonic time tick is now:   %lu  (%d,%ld)\n\n", 
			(ulong) ((mt.ticks()/1000) % 1000000000L), (int) mt.tv_sec, (long) mt.tv_nsec);

	#if defined(_TEST_CLOCK_TIME)
		DPRINTF("[Main] ClockTimer Ticks Per Sec: %u\n", (unsigned) 
			  ClockDuration::TicksPerSecond());
		DPRINTF("[Main] Waiting 5 sec for the Clock Timer...\n");
		ClockTimer tm;
		while (tm.ElapsedMilliseconds() < 5000) {
			DPRINTF("[%6u]  %5d\n", (unsigned) clock(), (int) tm.ElapsedMilliseconds());
			Thread::sleep(100);
		}
	#endif

	#if defined(_TEST_MUTEX)
		DPRINTF("[Main] Locking the Mutex...\n");
		mutex.acquire();
	#endif

	#if defined(_TEST_RECURSIVE_MUTEX)
		DPRINTF("[Main] Locking the Recursive Mutex 3x...\n");
		rmutex.acquire();
		rmutex.acquire();
		rmutex.acquire();
	#endif

	#if defined(_TEST_RDWR_LOCK)
		DPRINTF("[Main] Acquiring the write lock...\n");
		rwLock.wr_lock();
	#endif

	DPRINTF("[Main] Creating & starting the test thread...\n");		
	TestThread testThread(Thread::PRIORITY_ABOVE_NORMAL);


	#if defined(_TEST_MUTEX)
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);

		DPRINTF("[Main] Releasing the Mutex...\n");
		mutex.release();
	#endif

	#if defined(_TEST_RECURSIVE_MUTEX)
		DPRINTF("[Main] Sleeping for 1 sec...\n");
		Thread::sleep(1000);
		DPRINTF("[Main] Releasing the Recursive Mutex...\n");
		rmutex.release();
		DPRINTF("[Main] Sleeping for 1 sec...\n");
		Thread::sleep(1000);
		DPRINTF("[Main] Releasing the Recursive Mutex...\n");
		rmutex.release();
		DPRINTF("[Main] Sleeping for 1 sec...\n");
		Thread::sleep(1000);
		DPRINTF("[Main] Releasing the Recursive Mutex...\n");
		rmutex.release();
	#endif

	#if defined(_TEST_SEM)
		if (!sem) {
			DPRINTF("[Main] !!! Error creating semaphore !!!\n");
		}
		else {
			DPRINTF("[Main] Sleeping for 5 sec...\n");
			Thread::sleep(5000);

			DPRINTF("[Main] Releasing the Semaphore...\n");
			sem.release();
		}
	#endif
	
	#if defined(_TEST_COND_VAR)
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);

		DPRINTF("[Main] Signaling the Condition Variable...\n");
		condVar.lock();
		condSignaled = true;
		condVar.signal();
		condVar.unlock();

		DPRINTF("[Main] Signaling the Condition Data...\n");
		for (int i=1; i<=7; ++i) {
			Thread::sleep(1000);
			DPRINTF("\t(%d)\n", i);
			condData.lock_signal(i);
		}
	#endif
		
	#if defined(_TEST_RDWR_LOCK)
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);

		DPRINTF("[Main] Releasing the Write lock...\n");
		rwLock.wr_unlock();
	#endif
		
	#if defined(_TEST_EVENT)
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);

		DPRINTF("[Main] Signaling the OSEvent...\n");
		evt.signal();
	#endif

	#if defined(_TEST_ATOMIC_COUNTER)
		AtomicCounter actr;
		DPRINTF("[Main] AtomicCounter: %d\n", int(actr));
	#endif	

	#if defined(_TEST_MSG_QUEUE)
		DPRINTF("[Main] Reading MsgQueue...\n");

		for (int i=0; i<N_MSG; ++i) {
			DPRINTF("\t[Main] Waiting on MsgQueue\n");
			n = que.get();
			DPRINTF("\t[Main] Out: (%2u)  %5d\n", (unsigned) que.size(), n);
		}
	#endif

	#if defined(_TEST_MSG_SLOT)
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);

		DPRINTF("[Main] Putting value '24' into message slot...\n");
		msgSlot.put(24);
	#endif
	
	DPRINTF("[Main] Waiting for the TestThread to terminate...\n");
	testThread.wait();

	#if defined(_TEST_PERIODIC_THREAD)
		DPRINTF("[Main] Creating periodic thread...\n");
		PeriodicThread perThread(Thread::PRIORITY_ABOVE_NORMAL);
		DPRINTF("[Main] Sleeping for 5 sec...\n");
		Thread::sleep(5000);
		DPRINTF("[Main] Shutting down periodic thread...\n");
		perThread.quit();
		perThread.wait();
	#endif

	#if defined(_TEST_TIMER)
		DPRINTF("[Main] Creating a timer (5 sec)...\n");
		TimerClient tc;
		Timer tmr(tc, sec(5.0), sec(1.0));
		for (int i=0; i<3; ++i) {
			DPRINTF("[Main] Waiting for timer...\n");
			tc.wait();
			DPRINTF("[Main] Timer done.\n");
		}

		DPRINTF("[Main] Creating a one-shot timer (5 sec)...\n");
		TimerClient tc1;
		OneShot tmr1(tc1, sec(5.0));
		DPRINTF("[Main] Waiting for one-shot timer...\n");
		tc1.wait();
		DPRINTF("[Main] One-shot timer done.\n");
	#endif

	DPRINTF("[Main] Done... Exiting.\n");

	Thread::sleep(250);
	return EXIT_SUCCESS;
}

