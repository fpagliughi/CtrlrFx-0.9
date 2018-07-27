/// @file Win32.cpp
/// 

#include "CtrlrFx\os.h"
#include <process.h>

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//									Thread
/////////////////////////////////////////////////////////////////////////////

Thread::Thread(int prio) : quit_(false)
{
	create(DFLT_STACK_SIZE);
	priority(prio);
}

// --------------------------------------------------------------------------

Thread::Thread(int prio, unsigned stackSize) : quit_(false)
{
	create(stackSize);
	priority(prio);
}

// --------------------------------------------------------------------------

Thread::Thread(int prio, unsigned stackSize, const char*) : quit_(false)
{
	create(stackSize);
	priority(prio);
}

// --------------------------------------------------------------------------

void Thread::create(unsigned stackSize)
{
	unsigned id;

	attach((HANDLE) ::_beginthreadex(NULL, stackSize,
									 (ThreadFunc) startup,
									 static_cast<void*>(this),
									 CREATE_SUSPENDED, &id));
}

// --------------------------------------------------------------------------
//					Static Thread Functions
// --------------------------------------------------------------------------

int __stdcall Thread::startup(void *p)
{
	Thread* pThread = static_cast<Thread*>(p);
	int ret = pThread->run();
	pThread->close();
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
//									App
/////////////////////////////////////////////////////////////////////////////

// Starts up the Win32 environment for a CtrlrFx application, then runs the
// App::main() routine, which implements the application.
// 
// Win32 requires the WinSock DLL to be initialized by every process that
// uses it. We make the assumption that most applications on Win32 will
// be using sochkets...

int main(int argc, char *argv[])
{
	WSADATA wsadata;

	WSAStartup(MAKEWORD(2, 0), &wsadata);

	int ret = App::main(argc, argv);

	WSACleanup();
	return ret;
}

