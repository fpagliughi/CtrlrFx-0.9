// EvtTest.cpp
//
// CtrlrFx Test Application
//
// This is a test of creating a heirarchy of objects of different sizes using
// memory allocation from a pool of fixed-size blocks.

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/AtomicCounter.h"
#include "CtrlrFx/MsgQueue.h"
#include "CtrlrFx/ObjPool.h"
#include "CtrlrFx/MemPool.h"
#include "CtrlrFx/debug.h"
#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
// The abstract base class for Events.
// This creates a pool of fixed-size memory blocks from which derived events
// will be allocated. This forces the application to pre-define the maximum
// number of active events and the maximum size of an event, but prevents 
// heap fragmentaion by using fixed-size blocks. It also insures 
// thread-safety of the allocator by using a MemPool object, which is 
// thread safe.
//
// This implementation blocks on an allocation if the memory pool is empty.
// This changes the nature of the 'new' paradigm, but insures allocation
// never fails, even though a deadlock will occur in the event of a memory
// leak. Another implementation could use a non-blocking allocation with
// failure potential, if desired.

class Event
{
	static BytePool			memPool_;
	static AtomicCounter	instCtr_;

	static const size_t N_EVTS = 12,
						MAX_EVT_SIZE = 256;

	int ninst_;

public:
	Event() : ninst_(++instCtr_) {}

	virtual ~Event() {}

	virtual const char* type_name() const =0;

	int ninst() const { return ninst_; }

	void* operator new(size_t size) {
		return (void*) memPool_.get();
	}

	void operator delete(void* p) {
		memPool_.put((byte*) p);
	}
};

MemPool<byte> Event::memPool_(N_EVTS, MAX_EVT_SIZE);
AtomicCounter Event::instCtr_;

/////////////////////////////////////////////////////////////////////////////

class StatusEvt : public Event
{
	int typ_;

public:
	StatusEvt(int typ) : typ_(typ) {}
	virtual const char* type_name() const { return "Status Event"; }
};

class WarningEvt : public Event
{
public:
	WarningEvt() {}
	virtual const char* type_name() const { return "Warning Event"; }
};

class ErrorEvt : public Event
{
public:
	ErrorEvt() {}
	virtual const char* type_name() const { return "Error Event"; }
};

/////////////////////////////////////////////////////////////////////////////
// An event client is an object that can receive events.

class IEvtClient
{
public:
	virtual void on_event(Event* evt) = 0;
	virtual ~IEvtClient() {}
};

/////////////////////////////////////////////////////////////////////////////
// The event source thread.
// It runs, creates random types of events, and "fires" them off to clients.

class SrcThread : public Thread
{
	Semaphore	tmrSem_;
	IEvtClient*	client_;

	virtual int run();

	void fire(Event* evt) {
		if (client_)
			client_->on_event(evt);
	}

public:
	SrcThread() : Thread(PRIORITY_ABOVE_NORMAL), client_(0) {}

	void register_client(IEvtClient* client) {
		client_ = client;
	}

	virtual void quit() {
		tmrSem_.post();
	}
};

// --------------------------------------------------------------------------

int SrcThread::run()
{
	DPRINTF("SrcThread: Starting\n");
	Time t = Time::from_now(msec(5));

	while (!tmrSem_.wait_until(t)) {
		int n = rand() / (RAND_MAX >> 2) + 1;
		for (int i=0; i<n; ++i) {
			Event *evt;
			switch (i) {
				case 1:
				case 2: evt = new StatusEvt(i);
						break;
				case 3: evt = new WarningEvt();
						break;
				default:
						evt = new ErrorEvt();
						break;
			}
			fire(evt);
		}
		t += Duration(msec(1000));
	}

	fire((Event*) 0);

	DPRINTF("SrcThread: Ending\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// The destinataion thread receives events.

class DestThread : public Thread, virtual public IEvtClient
{
	MsgQueue<Event*> que_;

	virtual int run();

public:
	DestThread() : Thread(PRIORITY_BELOW_NORMAL), que_(5) {}

	virtual void on_event(Event* evt) {
		que_.put(evt);
	}
};

// --------------------------------------------------------------------------
// The destination thread function. It waits on incoming events and just
// writes them to the debug port.

int DestThread::run()
{
	DPRINTF("DestThread: Starting\n");
	while (true) {
		Event* evt = que_.get();

		if (evt == 0)
			break;

		DPRINTF("Event [%d]: %s\n", evt->ninst(), evt->type_name());
		delete evt;
	}

	DPRINTF("DestThread: Ending\n");
	return 0;
}

/////////////////////////////////////////////////////////////////////////////

int App::main(int argc, char * argv[])
{
	DPRINTF("EvtTest Starting\n");

	DestThread	dst;
	SrcThread	src;

	src.register_client(&dst);

	dst.activate();
	src.activate();

	getchar();

	src.quit();
	dst.wait();

	return EXIT_SUCCESS;
}

