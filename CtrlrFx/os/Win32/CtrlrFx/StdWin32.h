// StdWin32.h
//
// Standard includes and definitions for the Win32 target environment
// for embedded CtrlrFramework apps.
//

#ifndef __StdWin32_h
#define __StdWin32_h

#if !defined(WIN32)
	#error "This is a Win32 implementation file: Requires WIN32 defined"
#endif

//#pragma warning(4 : 4800)		// Forcing value to bool (performance warning)
#pragma warning(4 : 4996)	// Deprecated functions (CRT & all)
#pragma warning(4 : 4250)	// Inheritance via dominance

#include <cstddef>
#include <windows.h>
#define CTRLR_FX_INTERFACE_DEFINED		// Windows bring this in

typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef short				int16_t;
typedef unsigned short		uint16_t;
typedef int					int32_t;
typedef unsigned int		uint32_t;
typedef __int64				int64_t;
typedef unsigned __int64	uint64_t;

#define CTRLR_FX_SOCKET_T_DEFINED
typedef SOCKET	socket_t;

typedef int 		socklen_t;
typedef uint16_t	in_port_t;
typedef uint32_t	in_addr_t;

#endif		// __StdWin32_h