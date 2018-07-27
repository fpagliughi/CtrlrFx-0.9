// TcpAcceptor.cpp

#include <cstring>
#include "CtrlrFx/TcpAcceptor.h"

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------
// This attempts to open the acceptor, bind to the requested address, and 
// start listening. On any error it will be sure to leave the underlying
// socket in an unopened/invalid state.
// If the acceptor appears to already be opened, this will quietly succeed
// without doing anything. 

int TcpAcceptor::open(const InetAddr& addr, int queSize /*=DFLT_QUE_SIZE*/)
{
	if (is_open())
		return 0;

	if (!addr.is_set())
		return -1;

	// TODO: Check queue size?

	reset(TcpSocket::create());

	if (!is_valid())
		return handle();

	int ret;

	#if defined(CFX_POSIX)
		int reuse = 1;
		if ((ret = ::setsockopt(handle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int))) < 0) {
			release();
			return ret;
		}
	#endif	
	
	if ((ret = bind(addr)) < 0) {
		release();
		return ret;
	}

	if ((ret = listen(queSize)) < 0) {
		release();
		return ret;
	}

	addr_ = addr;
	return 0;
}

// --------------------------------------------------------------------------

TcpSocket TcpAcceptor::accept()
{
	InetAddr	clientAddr;
	socklen_t	len = sizeof(InetAddr);

	socket_t s = ::accept(handle(), clientAddr.sockaddr_ptr(), &len);
	return TcpSocket(s);
}

