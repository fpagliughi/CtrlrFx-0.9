// StdPlatform.h
//
// Standard includes and definitions for the eCos target environment for
// embedded CtrlrFramework apps.
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		CtrlrFramework (OS eCos)
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		May 2004
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#ifndef __CtrlrFx_StdPlatform_h
#define __CtrlrFx_StdPlatform_h

#include <cyg/kernel/kapi.h>		// Kernel API.
#include <cyg/hal/hal_arch.h>
#include <pkgconf/kernel.h>

typedef cyg_int8	int8_t;
typedef cyg_uint8	uint8_t;

typedef cyg_int16	int16_t;
typedef cyg_uint16	uint16_t;

typedef cyg_int32	int32_t;
typedef cyg_uint32	uint32_t;

typedef cyg_int64	int64_t;
typedef cyg_uint64	uint64_t;


// eCos doesn't support STL, so define min/max

namespace std {
	template<class T> inline const T& min(const T& a, const T& b) { return (a < b) ? a : b; }
	template<class T> inline const T& max(const T& a, const T& b) { return (a > b) ? a : b; }
};

namespace CtrlrFx {
	using std::min;
	using std::max;
};

//#include "CtrlrFx/xtypes.h"

#endif		// __CtrlrFx_StdPlatform_h

