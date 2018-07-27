/// @file Socket.h
/// 
/// Class for a TCP socket.
///
/// @author	Frank Pagliughi
/// @author	SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date	December 2003
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2003, SoRo Systems, Inc.
///		All Rights Reserved
///

#ifndef __CtrlrFx_Socket_h
#define __CtrlrFx_Socket_h

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/InetAddr.h"
#include "CtrlrFx/Buffer.h"
#include "CtrlrFx/IDevice.h"

#if !defined(WIN32)
	#include <sys/socket.h>
#endif

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

#ifndef CTRLR_FX_SOCKET_T_DEFINED
	typedef int socket_t;		///< The OS socket handle
	#define CTRLR_FX_SOCKET_T_DEFINED
#endif

/////////////////////////////////////////////////////////////////////////////
/// A wrapper class to provide the Socket class with reference semantics.
/// See various papers on "fixing" auto_ptr's by Bill Gibbons and Greg 
/// Colvin for details of the problems with ownership transfer smart 
/// pointers that this helper class solves.

struct SocketRef
{
	socket_t sock_;
	explicit SocketRef(socket_t s) : sock_(s) {}
};

/////////////////////////////////////////////////////////////////////////////
/// Base class for socket objects.
/// This class wraps an OS socket handle and maintains strict ownership
/// semantics. If a Socket object has a valid, open socket, then it owns
/// the handle and will close it when the object is destroyed. Assignment
/// follows ownership transfer semantics in a manner similar to the 
/// standard auto_ptr<> template. When one Socket object is copied to 
/// another the source gives up ownership to the destination. The source is
/// then left holding an invalid handle value. The rhs is modified in an 
/// assignment.

class Socket
{
	socket_t sock_;		///< OS socket handle

public:
	/// Creates an unconnected (invalid) socket
	Socket() : sock_(INVALID_SOCKET) {}

	/// Creates a socket from an OS socket handle.
	/// The object takes ownership of the handle.
	explicit Socket(socket_t sock) : sock_(sock) {}

	/// Creates a copy of the socket and transfers ownership.
	/// @note The source reference is non-const.
	Socket(Socket& s) : sock_(s.release()) {}
	
	/// Destructor closes the socket.
	virtual ~Socket() { close(); }

	/// Iniitializes the Socket library.
	/// This is only required for Win32. On platforms that use a standard
	/// socket implementation this is an empty call.
	static void initialize();

	/// Iniitializes the Socket library.
	/// This is only required for Win32. On platforms that use a standard
	/// socket implementation this is an empty call.
	static void destroy();

	/// Gets the OS socket handle without giving up ownership.
	socket_t handle() const { return sock_; }
	
	/// Determines if the socket is currently open.
	/// @sa is_valid
	/// @return
	/// @li @em true If the socket is valid and open.
	/// @li @em false If the socket is closed or invalid.
	bool is_open() const { return sock_ != INVALID_SOCKET; }
	
	/// Determines if the socket is valid.
	/// @sa is_open
	/// @return
	/// @li @em true If the socket is valid and open.
	/// @li @em false If the socket is closed or invalid.
	bool is_valid() const { return sock_ != INVALID_SOCKET; }

	/// Determines if the socket is valid.
	operator void*() const { return (void*) is_valid(); }

	/// Determines if the socket is not valid.
	bool operator!() const { return !is_valid(); }

	/// Gets the code for the last errror.
	int last_error() const;

	/// Releases ownership of the underlying socket handle.
	/// This is typically used to manually transfer ownership of the socket. 
	/// It sets the current handle to @em invalid and returns the previous 
	/// handle value. 
	/// @return The socket handle.
	socket_t release();

	/// Closes the socket.
	/// After closing the socket, the handle is set to @em invalid.
	void close();

	/// Resets the internal socket handle to the one specified.
	/// The current socket is closed before the new handle is accepted.
	void reset(socket_t s);

	/// Assigns the rhs socket to this one and transfers ownership.
	/// @note The rhs socket object is non-const. It will relinquish 
	/// ownership and be invalid after the assignmennt.
	Socket& operator=(Socket& s);

	// ----- Conversions to and from SockRef -----

	Socket(SocketRef ref) : sock_(ref.sock_) {}

	Socket& operator=(SocketRef ref) {
		reset(ref.sock_);
		return *this;
	}

	operator SocketRef() { 
		return SocketRef(release());
	}
};

/////////////////////////////////////////////////////////////////////////////
/// Class that wraps UDP sockets

#if !defined(WIN32)

class UdpSocket : public Socket
{
protected:
	static socket_t create();

public:
	/// Creates an unbound UDP socket.
	/// This can be used as a client or later bound as a server socket.
	UdpSocket() : Socket(create()) {}

	/// Creates a UDP socket and binds it to the specified port.
	UdpSocket(uint16_t port);

	/// Creates a UDP socket and binds it to the address.
	UdpSocket(const InetAddr& addr);

	//int open();

	/// Binds the socket to the address.
	int bind(const InetAddr& addr);

	// Connects the socket to an address.
	int connect(const InetAddr& addr);

	// ----- I/O -----

	/// Sends a UDP packet to the specified internet address
	int	sendto(const void* buf, size_t n, const InetAddr& addr);

	/// Sends a UDP packet to the specified internet address
	int	sendto(ByteBuffer& buf, const InetAddr& addr);

	/// Sends a UDP packet to the connected peer
	/// Note that this is only appropriate if the socket is connected.
	int	send(const void* buf, size_t n);

	/// Sends a UDP packet to the connected peer
	/// Note that this is only appropriate if the socket is connected.
	int	send(ByteBuffer& buf);

	/// Receives a UDP packet
	int	recvfrom(void* buf, size_t n, InetAddr& addr);

	/// Receives a UDP packet
	int	recvfrom(ByteBuffer& buf, InetAddr& addr);

	/// Receives a UDP packet
	int	recv(void* buf, size_t n);

	/// Receives a UDP packet without caring about the peer address
	int	recv(ByteBuffer& buf);
};

// --------------------------------------------------------------------------

inline socket_t UdpSocket::create()
{
	return (socket_t) ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

inline int UdpSocket::bind(const InetAddr& addr)
{
	return ::bind(handle(), addr.sockaddr_ptr(), sizeof(sockaddr_in));
}

inline int UdpSocket::connect(const InetAddr& addr)
{
	return ::connect(handle(), addr.sockaddr_ptr(), sizeof(sockaddr_in));
}

inline int UdpSocket::sendto(const void* buf, size_t n, const InetAddr& addr)
{
	return ::sendto(handle(), buf, n, 0, addr.sockaddr_ptr(), addr.size());
}

inline int UdpSocket::send(const void* buf, size_t n)
{
	return ::send(handle(), buf, n, 0);
}

inline int UdpSocket::recv(void* buf, size_t n)
{
	return ::recv(handle(), buf, n, 0);
}

#endif	// !WIN32

/////////////////////////////////////////////////////////////////////////////
/// Wrapper class for TCP sockets.
/// This is the streaming connection between two TCP peers. It looks like
/// a readable/writeable device.

class TcpSocket : public virtual IDevice, public Socket
{
protected:
	friend class TcpAcceptor;

	/// Creates a TCP socket.
	static socket_t create();

public:
	/// Creates an unconnected TCP socket.
	TcpSocket() {}

	/// Creates a TCP socket from an existing OS socket handle and claims
	/// ownership of the handle.
	explicit TcpSocket(socket_t sock) : Socket(sock) {}

	/// Creates a TCP socket by copying the socket handle from the specified
	/// socket object and transfers ownership of the socket.
	TcpSocket(TcpSocket& sock) : Socket(sock) {}

	/// Open the socket.
	int open();

	/// Assigns an OS socket handle to this device and claims
	/// ownership of the socket handle.
	/// @param s the OS socket handle.
	//TcpSocket& operator=(socket_t s);

	/// Copies the socket and transfers ownership to this object.
	TcpSocket& operator=(TcpSocket& s);

	// ----- Conversions to and from SockRef -----

	TcpSocket(SocketRef ref) : Socket(ref.sock_) {}

	TcpSocket& operator=(SocketRef ref) {
		reset(ref.sock_);
		return *this;
	}

	// ----- IDevice Interface -----

	/// Reads from the port
	virtual int read(void *buf, size_t n);

	/// Best effort attempts to read the specified number of bytes
	virtual int read_n(void *buf, size_t n);
	
	/// Reads data into a buffer object
	virtual int read(ByteBuffer& buf);

	/// Best effort attempt to read the whole buffer
	virtual int read_n(ByteBuffer& buf);
	/**
     * Set a timeout for read operations.
     * Sets the timout that the device uses for read operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool read_timeout(const Duration& d);

	/// Writes the buffer to the port
	virtual int write(const void *buf, size_t n);

	/// Attempts to write the whole buffer.
	virtual int write_n(const void *buf, size_t n);

	/// Writes the buffer to the port
	virtual int write(ByteBuffer& buf);

	/// Best effort attempt to write the whole buffer to the port
	virtual int write_n(ByteBuffer& buf);
	/**
     * Set a timeout for write operations.
     * Sets the timout that the device uses for write operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool write_timeout(const Duration& d);
};

// --------------------------------------------------------------------------

inline socket_t TcpSocket::create()
{
	return (socket_t) ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Socket_h

