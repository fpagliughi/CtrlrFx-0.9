/// @file InetAddr.h
/// Class for a TCP/IP socket address.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date December 2003


// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved

#ifndef __CtrlrFx_InetAddr_h
#define __CtrlrFx_InetAddr_h

#include <cstring>
#include "CtrlrFx/CtrlrFx.h"

#if !defined(WIN32)
	#include <arpa/inet.h>
#endif

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// Class that represents an internet address.
/// This inherits from the IP-specific form of a socket address, 
/// @em sockaddr_in. 

class InetAddr : public sockaddr_in
{
	// NOTE: This class makes heavy use of the fact that it is completely
	// binary compatible with a sockaddr/sockaddr_in, and the same size as
	// one of those structures. Do not add any other member variables, 
	// without going through the whole of the class to fixup!

	/// Sets the contents of this object to all zero
	void zero();
	
public:
	/// Constructs an empty address.
	/// The address is initialized to all zeroes.
	InetAddr();

	/// Constructs an address for the local host using the specified port.
	InetAddr(uint16_t port);

	/// Constructs an address for the specified host using the specified port.
	InetAddr(uint32_t addr, uint16_t port);

	/// Constructs an address using the name of the host and the specified port.
	/// This attempts to resolve the host name to an address.
	InetAddr(const char* saddr, uint16_t port);

	/// Constructs the address by copying the specified structure.
	InetAddr(const sockaddr& addr);

	/// Constructs the address by copying the specified structure.
	InetAddr(const sockaddr_in& addr);

	/// Constructs the address by copying the specified address.
	InetAddr(const InetAddr& addr);

	/// Checks if the address is set to some value.
	/// This doesn't attempt to determine if the address is valid, simply that
	/// it's not all zero.
	bool is_set() const;

	/// Equality comparator.
	/// This does a bitwise comparison.
	bool operator==(const InetAddr& rhs) const;

	/// Inequality comparator
	/// This does a bitwise comparison.
	bool operator!=(const InetAddr& rhs) const;

	/// Attempts to resolve the host name into a 32-bit internet address.
	/// @return The internet address in network byte order.
	static in_addr_t resolve_name(const char *saddr);

	/// Creates the socket address using the specified host address and 
	/// port number.
	void create(in_addr_t addr, uint16_t port);

	/// Creates the socket address using the specified host name and 
	/// port number.
	void create(const char *saddr, uint16_t port);

	/// Gets the 32-bit Internet Address.
	/// @ return The Internet address in the local host's byte order.
	uint32_t address() const { return ntohl(this->sin_addr.s_addr); }

	/// Gets the port number.
	/// @ return The port number in the local host's byte order.
	uint16_t port() const { return ntohs(this->sin_port); }

	/// Gets the size of this structure.
	/// This is equivalent to sizeof(this) but more convenient in some places.
	size_t size() const { return sizeof(InetAddr); }

	/// Returns a pointer to this object cast to a @em sockaddr.
	sockaddr* sockaddr_ptr() const;

	/// Returns a pointer to this object cast to a @em sockaddr_in.
	sockaddr_in* sockaddr_in_ptr() const;
};

// --------------------------------------------------------------------------

inline InetAddr::InetAddr()
{
	zero();
}

inline InetAddr::InetAddr(uint16_t port) 
{
	create(in_addr_t(INADDR_ANY), port);
}

inline InetAddr::InetAddr(in_addr_t addr, uint16_t port)
{
	create(addr, port);
}

inline InetAddr::InetAddr(const char* saddr, uint16_t port)
{
	create(saddr, port);
}

inline InetAddr::InetAddr(const sockaddr& addr)
{
	std::memcpy(sockaddr_ptr(), &addr, sizeof(sockaddr));
}

inline InetAddr::InetAddr(const sockaddr_in& addr)
{
	std::memcpy(sockaddr_in_ptr(), &addr, sizeof(sockaddr_in));
}

inline InetAddr::InetAddr(const InetAddr& addr)
{
	std::memcpy(this, &addr, sizeof(InetAddr));
}

// --------------------------------------------------------------------------

inline bool InetAddr::operator==(const InetAddr& rhs) const
{
	return std::memcmp(this, &rhs, sizeof(InetAddr)) == 0;
}

inline bool InetAddr::operator!=(const InetAddr& rhs) const
{
	return !operator==(rhs);
}

// --------------------------------------------------------------------------

inline void InetAddr::zero()
{
	std::memset(this, 0, sizeof(InetAddr));
}

inline sockaddr* InetAddr::sockaddr_ptr() const
{
	return (sockaddr*) this;
}

inline sockaddr_in* InetAddr::sockaddr_in_ptr() const
{
	return (sockaddr_in*) this;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_InetAddr_h

