// echo.cpp
// This is a CtrlrFx example application.
//
// It is an interactive application, meant to run a desktop OS, such as
// Linux or Win32, so it uses the STL.
//

#include <iostream>
#include <string>
#include <cstdlib>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/TcpConnector.h"

const uint16_t DFLT_PORT = 12345;

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	if (argc < 3) {
		cerr << "Usage: echo <host[:port]> [message]" << endl;
		return EXIT_FAILURE;
	}

	// Not required for all platforms, but harmless.
	Socket::initialize();

	string		saddr = argv[1];
	uint16_t	port = 7;
	
	string::size_type pos = saddr.find(':')	;
	if (pos != string::npos) {
		string sport = saddr.substr(pos+1);
		port = atoi(sport.c_str());
		saddr = saddr.substr(0, pos);		
	}

	cout << "Connecting to " << saddr << ":" << port << "..." << flush;
	
	InetAddr		addr(saddr.c_str(), port);
	TcpConnector	sock(addr);

	if (!sock) {
		#if defined(CFX_POSIX)
			perror("\n\tError");
		#else
			cerr << "\n\tConnection Error: " << sock.last_error() << endl;
		#endif
		return EXIT_FAILURE;
	}
	
	cout << "Ok" << endl;
	
	char buf[4096];
	strcpy(buf, argv[2]);
	
	int n = sock.write(buf, strlen(buf)+1);
	if (n < 0) {
		cerr << "Error writing: " << n << endl;
		return EXIT_FAILURE;
	}
	
	n = sock.read(buf, n);
	if (n < 0) {
		cerr << "Error reading: " << n << endl;
		return EXIT_FAILURE;
	}
	
	buf[n] = '\0';
	cout << buf << endl;
	
	// Not required for most platforms, but harmless.
	Socket::destroy();
	return EXIT_SUCCESS;
}
