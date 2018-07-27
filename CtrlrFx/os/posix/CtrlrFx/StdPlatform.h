/// @file StdPlatform.h
/// Standard CtrlrFx header file for POSIX platforms. 
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// @date July 2006
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved


#ifndef __CtrlrFx_StdPlatform_h
#define __CtrlrFx_StdPlatform_h

#include <cstddef>
#include <unistd.h>
#include <algorithm>
#include <stdint.h>

namespace CtrlrFx {
	using std::min;
	using std::max;
	using std::size_t;

	const int INVALID_SOCKET = -1;
}

#endif		// __CtrlrFx_StdPlatform_h

