/// @file StdPlatform.h
/// Header file for Controller Framework Library and Applications 
/// on Analog Device DSP's using the VisualDSP++ Kernel (vdk).
///
/// @note The project option for the library and any applications should
/// select the "Use std:: namespace" option. We define and empty std 
/// namespace just in case. This prevents the compiler from complaining that
/// it doesn't know of the existince of std.
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

#include "VDK.h"

namespace std {}
#include <algorithm>

// Define sized integers. ADI compiler lacks <stdint.h>

typedef signed char		int8_t;		///< 8-bit signed integer
typedef unsigned char	uint8_t;	///< 8-bit unsigned integer

typedef short			int16_t;	///< 16-bit signed integer
typedef unsigned short	uint16_t;	///< 16-bit unsigned integer

typedef long			int32_t;	///< 32-bit signed integer
typedef unsigned long	uint32_t;	///< 32-bit unsigned integer

typedef long long			int64_t;	///< 64-bit signed integer
typedef unsigned long long	uint64_t;	///< 64-bit unsigned integer

// ADI compiler doesn't define ssize_t
typedef long ssize_t;

#endif		// __CtrlrFx_StdPlatform_h

