// Socket.cpp
// 

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Socket.h"

#if !defined(WIN32)
	#include <unistd.h>
	#include <sys/fcntl.h>
	// TODO: Are these POSIX-only?
	#include <signal.h>
	#include <errno.h>
#endif

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//								Socket
/////////////////////////////////////////////////////////////////////////////

void Socket::initialize()
{
	#if defined(CFX_POSIX)
		// Don't signal on socket write errors.
		::signal(SIGPIPE, SIG_IGN);
	#elif defined(WIN32)
		WSADATA wsadata;
		::WSAStartup(MAKEWORD(2, 0), &wsadata);
	#endif
}

// --------------------------------------------------------------------------

void Socket::destroy()
{
	#if defined(WIN32)
		::WSACleanup();
	#endif
}

// --------------------------------------------------------------------------

int Socket::last_error() const
{
	#if defined(WIN32)
		return ::WSAGetLastError();
	#else
		return errno;
	#endif	
}

// --------------------------------------------------------------------------
// "Releases" ownership of the socket by simply assigning the handle to the
// invalid socket constant. It returns the previous value.

socket_t Socket::release()
{
	socket_t s = sock_;
	sock_ = INVALID_SOCKET;
	return s;
}

// --------------------------------------------------------------------------
// Closes the socket

void Socket::close()
{
	if (sock_ >= 0) {
		#if defined(WIN32)
			::closesocket(release());
		#else
			::close(release());
		#endif
	}
}

// --------------------------------------------------------------------------

void Socket::reset(socket_t s)
{
	if (s != sock_) {
		close();
		sock_ = s; 
	}
}

// --------------------------------------------------------------------------

Socket& Socket::operator=(Socket& s)
{
	if (&s != this)
		reset(s.release());
	return *this;
}


/////////////////////////////////////////////////////////////////////////////
//								UdpSocket
/////////////////////////////////////////////////////////////////////////////

#if !defined(WIN32)

UdpSocket::UdpSocket(uint16_t port) : Socket(create())
{
	bind(InetAddr(port));
}

UdpSocket::UdpSocket(const InetAddr& addr) : Socket(create())
{
	bind(addr);
}

// Opens a UDP socket. If it was already open, it just succeeds without
// doing anything.

/*
int UdpSocket::open()
{
	if (!is_open())
		reset(create());
		
	return is_open() ? 0 : -1;
}
*/

int UdpSocket::sendto(ByteBuffer& buf, const InetAddr& addr)
{
	int n = sendto(buf.position_ptr(), buf.available(), addr);

	if (n > 0)
		 buf.incr_position(n);
		 
	return n;
}

int UdpSocket::send(ByteBuffer& buf)
{
	int n = send(buf.position_ptr(), buf.available());

	if (n > 0)
		 buf.incr_position(n);
		 
	return n;
}

int UdpSocket::recvfrom(void* buf, size_t n, InetAddr& addr)
{
	sockaddr_in	sai;
	socklen_t	alen = sizeof(sockaddr_in);
	
	int ret = ::recvfrom(handle(), buf, n, 0, (sockaddr*) &sai, &alen);

	if (ret >= 0)
		addr = sai;
		
	return ret;
}

int UdpSocket::recvfrom(ByteBuffer& buf, InetAddr& addr)
{
	int n = recvfrom(buf.position_ptr(), buf.available(), addr);

	if (n > 0)
		buf.incr_position(n);

	return n;
}

int UdpSocket::recv(ByteBuffer& buf)
{
	int n = recv(buf.position_ptr(), buf.available());

	if (n > 0)
		buf.incr_position(n);

	return n;
}

#endif

/////////////////////////////////////////////////////////////////////////////
//								TcpSocket
/////////////////////////////////////////////////////////////////////////////

// Opens a TCP socket. If it was already open, it just succeeds without
// doing anything.

int TcpSocket::open()
{
	if (!is_open())
		reset(create());
		
	return is_open() ? 0 : -1;
}

// --------------------------------------------------------------------------
/*
TcpSocket& TcpSocket::operator=(socket_t s)
{
	if (s != sock_) {
		close();
		sock_ = s; 
	}
	return *this;
}
*/
// --------------------------------------------------------------------------

int TcpSocket::read(void *buf, size_t n)
{
	return ::recv(handle(), (char*) buf, n, 0);
}

// --------------------------------------------------------------------------
// Attempts to read the requested number of bytes by repeatedly calling 
// read() until it has the data or an error occurs.
// 

int TcpSocket::read_n(void *buf, size_t n)
{
	size_t	nr = 0;
	int		nx = 0;

	byte *b = reinterpret_cast<byte*>(buf);

	while (nr < n) {
		if ((nx = read(b+nr, n-nr)) <= 0)
			break;

		nr += nx;
	}

	return (nr == 0 && nx < 0) ? nx : int(nr);
}

// --------------------------------------------------------------------------
// Reads data into the buffer.

int TcpSocket::read(ByteBuffer& buf)
{
	int n = read(buf.position_ptr(), buf.available());

	if (n > 0)
		 buf.incr_position(n);

	return n;
}

// --------------------------------------------------------------------------
// Best effort attempt at reading the whole buffer

int TcpSocket::read_n(ByteBuffer& buf)
{
	size_t	nr = 0;
	int		nx = 0;
	
	while (!buf.full()) {
		 if ((nx = read(buf.position_ptr(), buf.available())) <= 0)
		 	break;

		 buf.incr_position(nx);
		 nr += nx;
	}	
		 
	return (nr == 0 && nx < 0) ? nx : int(nr);
}

// --------------------------------------------------------------------------

bool TcpSocket::read_timeout(const Duration& d)
{
	#if defined(CFX_POSIX)
		timeval tv = d.to_timeval();
		return ::setsockopt(handle(), SOL_SOCKET, SO_RCVTIMEO, 
							&tv, sizeof(timeval)) == 0;
	#else
		return false;
	#endif
}

// --------------------------------------------------------------------------

int TcpSocket::write(const void *buf, size_t n)
{
	return ::send(handle(), (const char*) buf, n , 0);
}

// --------------------------------------------------------------------------
// Attempts to write the entire buffer by repeatedly calling write() until
// either all of the data is sent or an error occurs.

int TcpSocket::write_n(const void *buf, size_t n)
{
	size_t	nw = 0;
	int		nx = 0;

	const byte *b = reinterpret_cast<const byte*>(buf);

	while (nw < n) {
		if ((nx = write(b+nw, n-nw)) <= 0)
			break;

		nw += nx;
	}

	return (nw == 0 && nx < 0) ? nx : int(nw);
}

// --------------------------------------------------------------------------
// Attempts to write the buffer to the socket.

int TcpSocket::write(ByteBuffer& buf)
{
	int n = write(buf.position_ptr(), buf.available());

	if (n > 0)
		 buf.incr_position(n);
		 
	return n;
}

// --------------------------------------------------------------------------
// Attempts to write all of the available data in the buffer to the socket.

int TcpSocket::write_n(ByteBuffer& buf)
{
	size_t	nw = 0;
	int		nx = 0;
	
	while (!buf.empty()) {
		if ((nx = write(buf.position_ptr(), buf.available())) <= 0)
			break;

		buf.incr_position(nx);
		nw += nx;
	}	
		 
	return (nw == 0 && nx < 0) ? nx : nw;
}

// --------------------------------------------------------------------------

bool TcpSocket::write_timeout(const Duration& d)
{
	#if defined(CFX_POSIX)
		timeval tv = d.to_timeval();
		return ::setsockopt(handle(), SOL_SOCKET, SO_SNDTIMEO, 
							&tv, sizeof(timeval)) == 0;
	#else
		return false;
	#endif
}

