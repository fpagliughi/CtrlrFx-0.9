// PoolTest.cpp
// 
// CtrlrFx Test Application.
// 
// This is a test of using pools and message queues in a multi-threaded 
// situation.
// 
// An "acquisition" thread gets an empty buffer from a pool, fills it, and 
// passes it to a "processing" thread. The processing thread checks the 
// contents, thereby emptying the buffer, and places the empty buffer back
// to the pool.
// 
// AUTHOR:
// 		Frank Pagliughi
//		SoRo Systems, Inc
// 
// DATE:
// 		July 2006


#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/MsgQueue.h"
#include "CtrlrFx/ObjPool.h"
#include "CtrlrFx/BufPool.h"
#include "CtrlrFx/Buffer.h"
#include <cstdio>

#define TRACE printf
#define TRACE_FLUSH() fflush(stdout)

using namespace std;
using namespace CtrlrFx;

const size_t	N_BUF = 4,
				BUF_SIZE = 512;

#if defined(_USE_OBJ_POOL)
	ObjPool<UIntBuffer> bufPool(N_BUF);
#else
	BufPool<uint> bufPool(N_BUF, BUF_SIZE);
#endif

MsgQueue<UIntBuffer*> bufQue(N_BUF);

/////////////////////////////////////////////////////////////////////////////
// 

class AcqThread : public Thread
{
	virtual int run();

public:
	AcqThread() : Thread(PRIORITY_ABOVE_NORMAL) {}
};

// --------------------------------------------------------------------------

int AcqThread::run()
{
	TRACE("AcqThread running\n");
	uint ctr = 0;

	while (!quit_) {
		UIntBuffer& buf = *bufPool.get();

		if (quit_)
			break;

		buf.clear();
		while (!buf.full())
			buf.put(ctr++);

		bufQue.put(&buf);
		sleep(25);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// 

class ProcThread : public Thread
{
	virtual int run();

public:
	ProcThread() : Thread(PRIORITY_NORMAL) {}
};

// --------------------------------------------------------------------------

int ProcThread::run()
{
	TRACE("ProcThread running\n");

	uint n, ctr = 0;

	while (!quit_) {
		UIntBuffer& buf = *bufQue.get();

		if (quit_)
			break;

		buf.flip();
		while (!buf.empty()) {
			if ((n = buf.get()) != ctr) {
				TRACE("Error: Expected %u, got %u\n", ctr, n);
				ctr = ++n;
			}
			else
				ctr++;
		}

		bufPool.put(&buf);

		if ((ctr / BUF_SIZE) % 10 == 0) {
			TRACE("\r%8u", ctr / BUF_SIZE);
			TRACE_FLUSH();
		}
	}

	return 0;
}


// --------------------------------------------------------------------------

int App::main(int, char**)
{
	// ----- Size the buffers -----

	#if defined(_USE_OBJ_POOL)
		for (size_t i=0; i<N_BUF; ++i) {
			UIntBuffer *buf = bufPool.get();
			buf->resize(BUF_SIZE);
			bufPool.put(buf);
		}
	#endif

	// ----- Start the threads -----

	AcqThread	acqThread;
	ProcThread	procThread;

	procThread.activate();
	acqThread.activate();

	// ----- Wait forever -----

	AutoResetEvent evt;
	evt.wait();

	return 0;
}

