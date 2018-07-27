// TcpConnector.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/TcpConnector.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------

TcpConnector::TcpConnector(const InetAddr& addr) : TcpSocket(TcpSocket::create())
{
	if (::connect(handle(), addr.sockaddr_ptr(), addr.size()) < 0)
		release();
}

// --------------------------------------------------------------------------

int TcpConnector::connect(const InetAddr& addr)
{
	int ret;

	reset(create());
	if ((ret = ::connect(handle(), addr.sockaddr_ptr(), addr.size())) < 0)
		release();

	return ret;
}

