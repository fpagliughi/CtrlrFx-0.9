// MtEchoServer.cpp
// Multi-threaded TCP echo server built with CtrlrFx.
//
// CtrlrFx Example Application.
// 
// This application demonstrates a number of classes and techniques in the 
// CtrlrFx library. It shows how the the TcpAcceptor and TcpSocket classes
// are used to create a network server that await and services incoming
// connections. Each connection is serviced by a separate, specialized thread
// derived from the Thread class. A ThreadPool is used to keep the service
// threads, and limit the number of simultaneous connections.

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/MsgSlot.h"
#include "CtrlrFx/ThreadPool.h"
#include "CtrlrFx/TcpAcceptor.h"
#include "CtrlrFx/FixedBuffer.h"
#include "CtrlrFx/debug.h"

using namespace std;
using namespace CtrlrFx;

const int		BUF_SIZE	= 4096,		// The buffer size for each connection
				POOL_SIZE	= 4;		// The max number of simultaneous connections
const uint16_t	TCP_PORT	= 12345;	// The TCP server's port

/////////////////////////////////////////////////////////////////////////////
// This class handles individual TCP connections for an echo server.
// Each instance waits for a TCP socket to arrive via a message slot.
// Once it has the socket connection it just waits for data packets to 
// arrive and then sends them back. When the client closes the connection
// the thread places itself back into the thread pool.
// Note that the buffer may be large, and will live on the thread's stack,
// so we must be sure to create a thread with a large enoughstack to hold it.

class EchoConnHandler : public Thread
{
	static int nInstance_;		// An instance counter (for diagnostics)

	int nobj_;					// Our object instance number
	MsgSlot<socket_t> msg_;		// Thread-safe to pass us messages

	virtual int run();

public:
	EchoConnHandler() : Thread(PRIORITY_NORMAL, DFLT_STACK_SIZE+BUF_SIZE),
														nobj_(nInstance_++) {}

	// This function, called by main() gives our thread instance
	// a socket connection to handle.
	void handle_conn(socket_t s) { msg_.put(s); }
};

// --------------------------------------------------------------------------

int EchoConnHandler::nInstance_ = 0;

// --------------------------------------------------------------------------
// The thread pool (global var)

EchoConnHandler 				threadArr[POOL_SIZE];
ThreadPool<EchoConnHandler>		threadPool(threadArr, POOL_SIZE);

// --------------------------------------------------------------------------
// The connection handler thread function.
// It gets an open socket handle through it's internal MsgSlot, processes
// packets from the client untili the client closes the connection, then
// returns the thread back to the pool and waits for the next connection.

int EchoConnHandler::run()
{
	DPRINTF("[EchoConnHandler:%d] Starting...\n", nobj_);
	FixedBuffer<byte, BUF_SIZE> buf;

	while (!quit_) {
		// Wait for main to give us a connection.
		TcpSocket sock(msg_.get());

		if (!sock || quit_)
			continue;

		DPRINTF("[EchoConnHandler:%d] Handling connection (%d).\n", nobj_, (int) sock.handle());

		// Read and write data packets until client closes connection
		while (sock.read(buf.clear()) > 0)
			sock.write_n(buf.flip());

		DPRINTF("[EchoConnHandler:%d] Connection closed.\n", nobj_);

		// Return this thread to the pool.
		threadPool.put(this);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// The main thread waits for incoming connections. When one arrives it
// gets a connection handler thread from the pool and sends it the socket
// handle for the connection.

int App::main(int, char**)
{
	// Not required on all platforms, but harmless either way.
	Socket::initialize();

	TcpAcceptor	acceptor(TCP_PORT);

	if (!acceptor) {
		DPRINTF("Error opening incoming TCP port.\n");
		return -1;
	}

	// Activate all the threads in the pool.

	for (int i=0; i<POOL_SIZE; ++i)
		threadArr[i].activate();

	while (true) {
		// Wait for an incoming connection
		DPRINTF("Awaiting TCP connection on port %u...\n", unsigned(TCP_PORT));
		TcpSocket sock = acceptor.accept();

		if (!sock) {
			DPRINTF("Error.\n");
			break;
		}

		DPRINTF("Ok\n");

		// Get a thread from the pool and send it the connection handle
		EchoConnHandler* thread = threadPool.get();
		thread->handle_conn(sock.release());
	}

	// Only required for WinSock, but harmless either way.
	Socket::destroy();
	return 0;
}

