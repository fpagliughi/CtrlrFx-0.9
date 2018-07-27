/// @file TcpAcceptor.h
/// 
/// Class for a TCP server to accept incoming connections.
///
/// @author	Frank Pagliughi
///	@author	SoRo Systems, Inc.
///	@author	www.sorosys.com
///
/// @date	December 2003
///

//
// Copyright (c) 2003, SoRo Systems, Inc.
// All Rights Reserved
//

#ifndef __CtrlrFx_TcpAcceptor_h
#define __CtrlrFx_TcpAcceptor_h

#include "InetAddr.h"
#include "Socket.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// Class for creating a TCP server.
/// Objects of this class bind and listen on TCP ports for incoming 
/// connections. Normally, a server thread creates one of these and blocks
/// on the call to accept incoming connections. The call to accept creates
/// and returns a @ref TcpSocket which can then be used for the actual 
/// communications.

class TcpAcceptor : public Socket
{
	/// The default listener queue size.
	static const int DFLT_QUE_SIZE = 4;

	/// The address to which the acceptor will bind.
	InetAddr addr_;

	/// Binds the socket to the specified address.
	int bind(const InetAddr& addr);

	/// Sets the socket listening on the port to which it is bound.
	int	listen(int queSize);

	// Non-copyable
	TcpAcceptor(const TcpAcceptor&);
	TcpAcceptor& operator=(const TcpAcceptor&);

public:
	/// Creates an unconnected acceptor.
	TcpAcceptor() {}

	/// Creates a acceptor and starts it listening on the specified port.
	/// The acceptor binds to the specified port for any address on the 
	/// local host.
	TcpAcceptor(uint16_t port, int queSize=DFLT_QUE_SIZE);

	/// Creates a acceptor and starts it listening on the specified port.
	TcpAcceptor(const InetAddr& addr, int queSize=DFLT_QUE_SIZE);

	/// Gets the address to which we are bound.
	const InetAddr& addr() const { return addr_; }

	/// Opens the acceptor socket.
	/// This binds the socket and starts it listening.
	virtual int	open(uint16_t port, int queSize=DFLT_QUE_SIZE);

	/// Opens the acceptor socket and binds it to the specified address
	virtual int	open(const InetAddr& addr, int queSize=DFLT_QUE_SIZE);
	
	/// Accepts an incoming TCP connection
	TcpSocket accept();
};

// --------------------------------------------------------------------------

inline TcpAcceptor::TcpAcceptor(uint16_t port, int queSize /*=DFLT_QUE_SIZE*/)
									: addr_(port)
{
	open(port, queSize);
}

inline TcpAcceptor::TcpAcceptor(const InetAddr& addr, int queSize /*=DFLT_QUE_SIZE*/)
									: addr_(addr)
{
	open(addr, queSize);
}

// --------------------------------------------------------------------------

inline int TcpAcceptor::bind(const InetAddr& addr)
{
	return ::bind(handle(), addr.sockaddr_ptr(), sizeof(sockaddr_in));
}

inline int TcpAcceptor::listen(int que_limit /*=DFLT_QUEUE_LIMIT*/)
{
	return ::listen(handle(), que_limit);
};

// --------------------------------------------------------------------------

inline int TcpAcceptor::open(uint16_t port, int queSize /*=DFLT_QUE_SIZE*/)
{
	return open(InetAddr(port), queSize);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_TcpAcceptor_h

