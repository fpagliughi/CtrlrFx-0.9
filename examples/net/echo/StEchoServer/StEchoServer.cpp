// StEchoServer.cpp
// 
// Single-threaded TCP echo server built with CtrlrFx.
//
// CtrlrFx Example Application.
// 

#include <iostream>
#include "CtrlrFx/TcpAcceptor.h"

using namespace std;
using namespace CtrlrFx;

const int		BUF_SIZE = 4096;
const uint16	TCP_PORT = 12345;
const int		LISTEN_QUE_SIZE = 1;

int main()
{
	int		n;
	byte	buf[BUF_SIZE];

	// Not required on all platforms, but harmless either way.
	Socket::initialize();

	TcpAcceptor	acceptor(TCP_PORT, LISTEN_QUE_SIZE);

	if (!acceptor.is_open()) {
		cerr << "Error opening incoming TCP port." << endl;
		return -1;
	}

	while (true) {
		cout << "Awaiting TCP connection on port " 
				<< TCP_PORT << "..." << flush;

		TcpSocket s = acceptor.accept();

		if (!s.is_valid()) {
			cerr << "Error." << endl;
			break;
		}
		cout << "Ok" << endl;

		while ((n = s.read(buf, BUF_SIZE)) > 0)
			s.write_n(buf, n);

		cout << "Connection closed." << endl;
	}

	// Only required for WinSock, but harmless either way.
	Socket::destroy();
	return 0;
}

