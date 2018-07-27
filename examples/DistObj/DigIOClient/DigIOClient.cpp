// DigIOClient.cpp
//

// Make sure DPRINT is enabled
#if !defined(CFX_DEBUG_LEVEL)
	#define CFX_DEBUG_LEVEL 1
#endif

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/TcpConnector.h"
#include "CtrlrFx/DistObjSrvr.h"
#include "CtrlrFx/DigIO_stub.h"
#include "CtrlrFx/debug.h"

using namespace CtrlrFx;

const uint16_t DFLT_TCP_PORT = 12345;

// --------------------------------------------------------------------------

int App::main(int argc, char* argv[])
{
	int			bit = 1;
	char		host[512] = "localhost";
	uint16_t	port = DFLT_TCP_PORT;

	if (argc == 2)
		port = (uint16_t) atoi(argv[1]);
	else if (argc == 3) {
		strcpy(host, argv[1]);
		port = (uint16_t) atoi(argv[2]);
	}

	DigIn_stub		dig_in;
	DigOut_stub		dig_out;

	InetAddr		addr(host, port);
	TcpConnector	sock;

	if (sock.connect(addr) < 0) {
		DPUTS("Connection Error\n");
		return EXIT_FAILURE;
	}

	DistObjSrvr obj_srvr(&sock);

	dig_out.obj_srvr(&obj_srvr);
	dig_in.obj_srvr(&obj_srvr);

	int wd = dig_out.width();
	DPRINTF("Dig out port is %d bits wide\n", wd);

	bool b = dig_in.value(bit);
	DPRINTF("Port %d is %s\n", bit, b ? "High" : "Low");

	dig_out.toggle(bit);
	b = dig_in.value(bit);
	DPRINTF("Port %d is %s\n", bit, b ? "High" : "Low");

	return 0;
}

