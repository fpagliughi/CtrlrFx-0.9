// InetAddr.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/InetAddr.h"
#include <cstring>

#if !defined(WIN32)
	#include <netdb.h>
#endif

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------

bool InetAddr::is_set() const
{
	const byte* b = reinterpret_cast<const byte*>(this);

	for (size_t i=0; i<sizeof(InetAddr); ++i) {
		if (b[i] != 0)
			return true;
	}
	return false;
}

// --------------------------------------------------------------------------

in_addr_t InetAddr::resolve_name(const char *saddr)
{
	#if !defined(WIN32)
		in_addr ia;
		if (::inet_aton(saddr, &ia))
			return ia.s_addr;
	#endif

	hostent *host = ::gethostbyname(saddr);
	return (host) ? *((in_addr_t*) host->h_addr_list[0]) : 0;
}

// --------------------------------------------------------------------------

void InetAddr::create(uint32_t addr, uint16_t port)
{
	zero();
	sin_family = AF_INET;
	sin_addr.s_addr = htonl(addr);
	sin_port = htons(port);
}

// --------------------------------------------------------------------------

void InetAddr::create(const char *saddr, uint16_t port)
{
	zero(); 
	sin_family = AF_INET;
	sin_addr.s_addr = resolve_name(saddr);
	sin_port = htons(port);
}

