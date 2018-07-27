// PeriodicThread.cpp
//
// This is a Controller Framework example program demonstrating how to create
// threads that run periodically.
// 
// This example only works on platforms that have a wait_until(absolute time)
// method in their semaphores. Note that the threads are scheduled to run 
// periodically, but may jitter due to scheduling issues. Better timing is
// achieved by using Timer objects that execute in interrupt or other high
// priority context.
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/debug.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
// This is the periodic thread. It derives from the CtrlrFx thread class
// and overrides the run method to implement the periodic behavior.

class PeriodicThread : public Thread
{
	Duration	period_;
	Semaphore	quitSem_;

	virtual int run();

public:
	// We construct the thread and start it running by calling activate()
	PeriodicThread(const Duration& per, int prio, const char *name)
			: Thread(prio, DFLT_STACK_SIZE, name), period_(per) {
		activate();
	}

	// When the user want to quit we post the "quit semaphore" 
	// to signal  the run() method to exit.
	virtual void quit() { quitSem_.post(); }
};

// --------------------------------------------------------------------------
// The thread function. This loops until another thread sets the "quit
// semaphore" indicating the funtion, and thus thread, should exit.
// The thread sleeps on a wait of the semaphore. The wait normally fails,
// which, in this case, is the expected behavior. The wait times out when
// the thread should be scheduled to run. 
// The thread then runs its periodic task (in this case prints a message
// to the debug output). It then updates the time for the next sleep by 
// adding the desired time period to the previous timeout time. Thus the
// trigger time will not be dependent on the time for the task as long as
// the task completes within the time period.

int PeriodicThread::run()
{
	unsigned	n = 0;
	Time		t = Time::from_now(period_);

	while (!quitSem_.wait_until(t)) {
		DPRINTF("%s: clicked [%u]\n", name(), ++n);
		t += period_;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// The main routine (thread) creates two periodic threads: one that runs at
// one second intervals and one that runs at five second intervals. Main 
// then just sleeps and lets the two threads run.
// On a desk-top target, the user can just hit <ENTER> to stop the threads
// and end the program.

int App::main(int, char**)
{
	// ----- Create and start the threads -----

	PeriodicThread	oneSecThread(sec(1.0), 
								 Thread::PRIORITY_ABOVE_NORMAL + Thread::PRIORITY_INCREASE,
								 "OneSec Thread"),
					fiveSecThread(sec(5.0), Thread::PRIORITY_ABOVE_NORMAL,
								  "FiveSec Thread");

	// ----- This thread (main) just waits for user abort -----

	#if defined(CTRLR_FX_POSIX) || defined(WIN32)
		getchar();
	#else
		AutoResetEvent doneEvt;
		doneEvt.wait();
	#endif

	// ----- Stop the threads and exit the program -----

	oneSecThread.quit();
	fiveSecThread.quit();

	oneSecThread.wait();
	fiveSecThread.wait();

	return 0;
}

