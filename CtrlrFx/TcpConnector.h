/// @file TcpConnector.h
/// Class for creating client-side TCP connections
///
/// @author	Frank Pagliughi
/// @author SoRo Systems, Inc.
/// @author www.sorosys.com
///
/// @date	December 2003
///

#ifndef __CtrlrFx_TcpConnector_h
#define __CtrlrFx_TcpConnector_h

#include "Socket.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// Class to create a client TCP connection.

class TcpConnector : public TcpSocket
{
	// Non-copyable
	TcpConnector(const TcpConnector&);
	TcpConnector& operator=(const TcpConnector&);

public:
	/// Creates an unconnected connector.
	TcpConnector() {}

	/// Creates the connector and attempts to connect to the specified address.
	TcpConnector(const InetAddr& addr);

	/// Attempts to connects to the specified server.
	/// If the socket is currently connected, this will close the current
	/// connection and open the new one.
	int	connect(const InetAddr& addr);
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_TcpConnector_h
