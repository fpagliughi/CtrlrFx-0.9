// DigIOSim.cpp
//
// Example program for the Performance Controller Framework. It simulates
// a remote object invocation on  
//
// PROJECT:
//		Controller Framework	(Test/Demo Code)
//
// DESCRIPTION:
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		June 2003
//

// Make sure DPRINT is enabled
#if !defined(CFX_DEBUG_LEVEL)
	#define CFX_DEBUG_LEVEL 1
#endif

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/TcpAcceptor.h"
#include "CtrlrFx/DistObjSrvr.h"
#include "CtrlrFx/DigIO_skel.h"
#include "CtrlrFx/BinDistObj.h"
#include "CtrlrFx/debug.h"

using namespace CtrlrFx;

const uint16 TCP_PORT = 12345;

// Data bit values shared by the output & input ports.

const int N_BIT = 32;
static bool bit_val[N_BIT];

/////////////////////////////////////////////////////////////////////////////
//							Global Declarations
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//								DigIn
/////////////////////////////////////////////////////////////////////////////

class DigIn_impl : public DigIn_skel
{
public:
	/// Gets the width (# of bits) in the port
	virtual int width() { return N_BIT; }

	virtual bool value(int bit) {
		return (bit >= 0 && bit < N_BIT) ? bit_val[bit] : false;
	}
};

/////////////////////////////////////////////////////////////////////////////
//								DigOut
/////////////////////////////////////////////////////////////////////////////

class DigOut_impl : public DigOut_skel
{
public:
	/// Gets the width (# of bits) in the port
	virtual int width() { return N_BIT; }
	
	/// Sets the output bit high.
	virtual void set(int bit) {
		set(bit, true);
	}
	
	/// Sets the output bit to the specified value (true=high).
	virtual void set(int bit, bool b) {
		DPRINTF("DigOut: Setting bit %d %s\n", bit, b ? "On" : "Off");
		if (bit >= 0 && bit < N_BIT)
			bit_val[bit] = b;
	}

	/// Sets the output bit low.
	virtual void clear(int bit) {
		DPRINTF("DigOut: Clearing Bit %d\n", bit);
		if (bit >= 0 && bit < N_BIT)
			bit_val[bit] = false;
	}
	
	/// Toggles the output bit.
	virtual void toggle(int bit) {
		DPRINTF("DigOut: Toggling Bit %d\n", bit);
		if (bit >= 0 && bit < N_BIT)
			bit_val[bit] = !bit_val[bit];
	}
};

/////////////////////////////////////////////////////////////////////////////

int App::main(int, char**)
{
	DPUTS("DigIOSim.  Digital I/O Remote object simulator\n\n");

	// Create a TCP Server/acceptor socket

	TcpAcceptor	acceptor(TCP_PORT);

	if (!acceptor) {
		DPRINTF("Error opening incoming TCP port.\n");
		return EXIT_FAILURE;
	}

	// Create target object implementations & a server for them

	DigIn_impl	dig_in;
	DigOut_impl	dig_out;
	DistObjSrvr	obj_srvr;

	// Register target objects with the server

	obj_srvr.register_obj(tgt_obj(DIST_OBJ_CLASS_DIG_IN,  0), &dig_in);
	obj_srvr.register_obj(tgt_obj(DIST_OBJ_CLASS_DIG_OUT, 0), &dig_out);

	while (true) {
		// Wait for a TCP client connection

		DPUTS("\nAwaiting TCP connection...\n");
		TcpSocket sock = acceptor.accept();

		if (!sock) {
			DPRINTF("Error.");
			return EXIT_FAILURE;
		}

		DPUTS("Ok\n");

		// Give the client connection to the server
		// & run the server until the connection is closed

		obj_srvr.port(&sock);
		obj_srvr.run();
	}

	DPUTS("\nDone... Exiting\n");
	return EXIT_SUCCESS;
}

