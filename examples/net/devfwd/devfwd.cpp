// devfwd.cpp
//
// This is a CtrlrFx example application.
// 
// It is a utility program to forward packets from a TCP connection to a
// device communications port, such as a serial port. It demonstrates the
// use of a number of low-level CtrlrFx communications classes.
// It uses a separate thread to monitor communications in each direction.
// On a specific OS this might be done more efficiently with a select()
// but this demonstrates simple multitasking.
// 
// DEMONSTRATES:
// 		- Low-level TCP socket (server) classes: TcpAcceptor, Socket
// 		- Low-level Device Communications: InDevice, OutDevice
// 		- Simple Mulit-Threaded programming: Thread
// 
// AUTHOR:
//		Frank Pagliughi
//
// DATE:
//		April 2005

#include <cstdio>
#include <errno.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/Buffer.h"
#include "CtrlrFx/TcpAcceptor.h"
#include "CtrlrFx/Thread.h"
#include "CtrlrFx/Device.h"

// TODO: Replace this with framework TRACING when it's fixed.
#define TRACE printf

using namespace std;
using namespace CtrlrFx;

const uint16_t		PORT = 2233;
const char * const 	DEVICE = "/dev/ttyS0";
const int			BUF_SIZE = 4096;

/////////////////////////////////////////////////////////////////////////////
// The device monitor thread. It reads packets from the device and sends
// them to the TCP port.

class DevMonThread : public Thread
{
	TcpSocket& 		sock_;
	const char 		*devname_;
	
protected:
	virtual int run();

public:
	DevMonThread(TcpSocket& sock, const char *devname)
		: Thread(PRIORITY_ABOVE_NORMAL), sock_(sock), devname_(devname) {}

	virtual void quit() {
		quit_ = true;
		wait();
	}
};

// --------------------------------------------------------------------------
// This is the active thread function to monitor incoming data on the device.
// The thread is only run while the TCP socket is open. 
//
// We open a handle to the device and wait for incoming data. To allow for
// a clean termination of the thread, we open the device in non-blocking
// mode and essentially poll it.
//
// Data read from the device is just sent out the socket.

int DevMonThread::run()
{
	TRACE("DevMonThread: Started\n");

	InDevice idev(devname_, O_NONBLOCK);
	
	if (!idev.is_open()) {
		TRACE("DevMonThread: Error opening the input device\n");
		return -1;
	}

	byte		bufmem[BUF_SIZE];
	ByteBuffer	buf(bufmem, BUF_SIZE);
	
	while (!quit_) {
		if (idev.read(buf.clear()) < 0) {
			if (errno != EWOULDBLOCK)
				TRACE("DevMonThread: Error reading from the device: %d\n", errno);
			sleep(250);
		}
		else
			sock_.write(buf.flip());
	}
	
	TRACE("DevMonThread: Done\n");
	return 0;
}

////////////////////////////////////////////////////////////////////////////
// The main thread waits for incoming TCP connections. When one arrives, 
// a socket is opened, a thread is started to monitor the incoming device
// data, and we monitor incoming data on the socket.
// When the socket is closed by the client, we close the device thread,
// and the device itself, and await another incoming TCP connection.
// 
// This is essentially single-threaded in regards to the incoming TCP 
// connection and can only service one connection at a time.

int App::main(int, char **)
{
	//int			n;
	byte		bufmem[BUF_SIZE];
	ByteBuffer	buf(bufmem, BUF_SIZE);

	// ----- Create a TCP acceptor -----

	TcpAcceptor	acceptor(PORT);

	if (!acceptor) {
		TRACE("Error opening incoming TCP port.\n");
		return -1;
	}
	
	while (true) {
		// ----- Wait for a TCP connection -----

		TRACE("Awaiting TCP connection on port %u...\n", (unsigned) PORT);
		TcpSocket sock = acceptor.accept();

		if (!sock) {
			TRACE("Error.\n");
			return -1;
		}

		TRACE("Ok\n");

		// ----- We've got a connection. -----

		DevMonThread dev_thread(sock, DEVICE);
		dev_thread.activate();
		
		OutDevice odev(DEVICE);
		if (!odev.is_open()) {
			TRACE("Main: Error opening output device");
			return -1;
		}

		StdOutDevice stddev;
		
		// ----- Read incoming data on the TCP socket & send to dev -----

		while (sock.read(buf.clear()) > 0) {
			stddev.write(buf.flip());
			odev.write(buf.rewind());
		}

		// ----- Connection closed by client -----

		TRACE("Main: Connection closed. Cleaning up...\n");
		dev_thread.quit();
		sock.close();
		odev.close();
		TRACE("Ok\n");
	}
	
	return 0;	
}
