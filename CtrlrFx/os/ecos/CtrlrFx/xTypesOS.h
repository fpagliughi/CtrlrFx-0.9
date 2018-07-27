// xtypes_ecos.h
//
// Type definitions for the eCos RTOS. These should generally work for any
// target, since the OS environment defines types for the designated target.
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		eCos apps
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		February 2003
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#ifndef __CtrlrFx_xtypes_ecos_h
#define __CtrlrFx_xtypes_ecos_h

#include <cyg/infra/cyg_type.h>

typedef cyg_int8		int8_t;
typedef cyg_uint8		uint8_t;

typedef cyg_int16		int16_t;
typedef cyg_uint16		uint16_t;

typedef cyg_int32		int32_t;
typedef cyg_uint32		uint32_t;

//typedef cyg_int64		int64_t;
//typedef cyg_uint64	uint64_t;

//static inline uint16_t make_word(byte hi, byte lo)		{ return ((uint16_t) hi << 8)  | lo; }
//static inline uint32_t make_dword(uint16 hi, uint16 lo)	{ return ((uint32_t) hi << 16) | lo; }

#endif		// __CtrlrFx_xtypes_ecos_h
