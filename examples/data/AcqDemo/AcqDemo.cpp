// AcqDemo.cpp
//
// Demonstrates the use of the Performance Controller Framework. It simulates
// a multitasking data acquisition system containing three threads: one to 
// collect data, one to process it, and one to send it off to a host.
// It uses a global MtAcqBufQueue to for passing buffers between queues.
//
// PROJECT:
//		Controller Framework	(Test/Demo Code)
//
// DESCRIPTION:
//		This program is a simulation of a data acquisition and processing
//		program, that passes processed data on to a host computer.
//
//		The "Acquisition" thread grabs an empty buffer, generates a stream
//		of incremental 16-bit unsigned words, and places them into the "raw"
//		data queue. The "process" thread grabs the raw thread when available and
//		multiplies each number by 2. This is done in place, without actually
//		copy any of the data (other than the arithmetic operation back to the
//		original memory location). The buffer is then placed in the "finished"
//		queue, where the "communications" thread gets it. This last thread 
//		validates the integrity of the data, then reports a running tally to
//		the user via the trace facility..
//
// ALGORITHM:
//		This program demonstrates the use of an acquisition buffer queue
//		being used by multiple threads of control to pass data down a 
//		process chain:
//			- The first thread collects data and adds it to the pool,
//			- The second processes/modifies the data, passing it 
//					through the pool, and
//			- The third sends it off, removing it from the pool.
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		June 2001
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/AcqBufQueue.h"

#define _TRACE_STDOUT
//#include "trace.h"
#include <stdio.h>
#define TRACE printf
#define TRACE_FLUSH() fflush(stdout)

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//							Global Declarations
/////////////////////////////////////////////////////////////////////////////

const int	BUF_SIZE	= 50,	// The size of each buffers.
			N_BUF		= 4;	// The number of buffers in the pool.

enum { 
	EMPTY_QUE, 			// Empty Buffers
	RAW_QUE, 			// Raw data from the Acq Thread
	FINISHED_QUE, 		// Processed data from the Process Thread
	N_QUE				// The # of buffer queues
};

AcqBufQueue<uint16_t> bufQue;

/////////////////////////////////////////////////////////////////////////////
//								AcqThread
/////////////////////////////////////////////////////////////////////////////

// This is the acquisition thread. It gets empty buffers from the EMPTY_QUE,
// fills them with simulated "raw" acquisition data, and places the filled
// data in the RAW_QUE.

class AcqThread : public Thread
{
protected:
	virtual int run();

public:
	AcqThread(int prio);
};

// --------------------------------------------------------------------------
// The constructor simply starts the thread running.
//

AcqThread::AcqThread(int prio) : Thread(prio, DFLT_STACK_SIZE, "AcqThread")
{
	activate();
}

// --------------------------------------------------------------------------
// The acquisition thread function. Each time through the loop it tries to
// acquire a new (empty) buffer without blocking. This simulates use from an
// ISR where blocking is not allowed. A failure here would be devestating.
// Once it has a buffer it fills the buffer with an incremental count, and
// places the buffer back into the pool in the "raw" data queue. It then sleeps 
// for a few milliseconds and gets back and does it again.
//
// In this example, we write to the buffer as if it were a normal array then
// manually set the size.

int AcqThread::run()
{
	uint16_t ctr = 0;

	TRACE("[AcqThread] Started.\n");
	
	while (!quit_) {
		Buffer<uint16_t>& buf = *bufQue[EMPTY_QUE].get();

		if (quit_)
			break;

		buf.clear();
		size_t n = buf.capacity();

		for (size_t i=0; i<n; i++)
			buf[i] = ctr++;

		buf.position(n);
		bufQue[RAW_QUE].put(&buf);

		sleep(50);
	}

	bufQue[RAW_QUE].release();

	TRACE("[AcqThread] Done.\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//								ProcessThread
/////////////////////////////////////////////////////////////////////////////

// This is the process thread. It retrieves buffers of raw data from the
// RAW_QUE, does some calculations of the data, then places the buffer of
// processed data into the FINISHED_QUEUE.

class ProcessThread : public Thread
{
protected:
	virtual int run();

public:
	ProcessThread(int prio);
};

// --------------------------------------------------------------------------
// The constructor simply starts the thread running.
//

ProcessThread::ProcessThread(int prio) 
					: Thread(prio, DFLT_STACK_SIZE, "ProcessThread")
{
	activate();
}

// --------------------------------------------------------------------------
// The active routine for the process thread. It simply gets the next buffer
// from the "raw" queue of the buffer pool, multiplies each of the numbers
// by two, and places the buffer into the "finished" queue of the pool.
//
// The processing is done "in place" in the buffer, and the old data is
// overwritten. None of the internal pointers in the buffer are updated, so
// it will still be full when passed to the next thread.

int ProcessThread::run()
{
	TRACE("[ProcessThread] Started.\n");

	while (!quit_) {
		Buffer<uint16_t>& buf = *bufQue[RAW_QUE].get();

		if (quit_)
			break;

		for (size_t i=0; i<buf.size(); i++)
			buf[i] *= 2;

		bufQue[FINISHED_QUE].put(&buf);
	}

	bufQue[FINISHED_QUE].release();

	TRACE("[ProcessThread] Done.\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//								CommThread
/////////////////////////////////////////////////////////////////////////////

// The communications thread simulates storing the final data or sending it
// to a host process. It rerieves the processed data from the FINISHED_QUE
// empties the buffer, and places the empty back in the EMPTY_QUE.

class CommThread : public Thread
{
protected:
	virtual int run();

public:
	CommThread(int prio);
};

// --------------------------------------------------------------------------
// Again, the constructor simply starts the thread running.
//

CommThread::CommThread(int prio) : Thread(prio, DFLT_STACK_SIZE, "CommThread")
{
	activate();
}

// --------------------------------------------------------------------------
// The active function for the "communications" thread. In a normal data 
// acquisition system, this would send the data to a host or a storage system.
// In this demo, it writes a value to standard output once in a while to 
// show that the system is running.
//
// After each buffer is "processed" it is placed back into the "empty" queue,
// to be reused by the data acquisition system.
//

int CommThread::run()
{
	unsigned	n = 0;
	uint16_t	ctr = 0;
	
	TRACE("[CommThread] Started.\n");

	while (!quit_) {
		Buffer<uint16_t>& buf = *bufQue[FINISHED_QUE].get();

		if (quit_)
			break;

		// ----- Check the buffer for errors -----

		buf.flip();

		while (buf.available()) {
			uint16_t v = buf.get();

			if (ctr != v) {
				TRACE("Error detected in buffer %u. Expected %u, got %u\n", 
							n, (unsigned) ctr, (unsigned) v);
				break;
			}
			ctr += 2;
		}
		
		// ----- Show progress -----
			
		if ((n++ % 5) == 0) {
			TRACE("\r%6u", (unsigned)  (buf[0]/100));
			TRACE_FLUSH();
		}

		// ----- Put the "empty" buffer back -----

		buf.clear();		
		bufQue[EMPTY_QUE].put(&buf);
	}

	TRACE("[CommThread] Done.\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

int App::main(int, char**)
{
	TRACE("Acquisition Demo.\n\n");

	// ----- Create the buffer pool for threads to use -----

	bufQue.resize(N_QUE, N_BUF, BUF_SIZE);

	// ----- Creating the threads starts them running -----

	CommThread 		commThread(Thread::PRIORITY_NORMAL - Thread::PRIORITY_INCREASE);
	ProcessThread	processThread(Thread::PRIORITY_BELOW_NORMAL);
	AcqThread		acqThread(Thread::PRIORITY_ABOVE_NORMAL);

	// ----- This thread (main) just waits for user abort -----

	#if defined(_POSIX) || defined(WIN32)
		getchar();
	#else
		AutoResetEvent doneEvt;
		doneEvt.wait();
	#endif
	
	// ----- Perform an orderly shutdown -----

	TRACE("\nPerforming an orderly shutdown...\n");
	TRACE("Signaling threads to quit...\n");

	TRACE("\tCommThread...");
	TRACE_FLUSH();

	commThread.quit();
	commThread.wait();

	TRACE("\tProcessThread...");
	TRACE_FLUSH();
	
	processThread.quit();
	processThread.wait();

	TRACE("\tAcqThread...");
	TRACE_FLUSH();
	
	acqThread.quit();
	acqThread.wait();

	TRACE("\nDone... Exiting\n");
	return EXIT_SUCCESS;
}

