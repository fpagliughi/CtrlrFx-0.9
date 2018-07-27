/// @file error.h

#ifndef __CtrlrFx_error_h
#define __CtrlrFx_error_h

#include "CtrlrFx/EnumDef.h"

ENUM_BEGIN_MIN(CFXE, 1)
	CFXE_SYSTEM,			///< (1) System error
	CFXE_NO_MEM,			///< (2) Not enough memory for requested operation
	CFXE_PACKET_WRITE,		///< (3) Error writing a distributed packet
	CFXE_PACKET_READ,		///< (4) Error reading a distributed packet
	CFXE_PACKET_SIZE,		///< (5) Incoming packet size out of range
	CFXE_UNKNOWN_OBJ,		///< (6) Unknown remote object
	CFXE_UNKNOWN_CMD,		///< (7) Unknown command on a (known) remote object
	CFXE_SRVR_CLOSING		///< (8) Command not completed because server closing
ENUM_END(CFXE);

#endif		// __CtrlrFx_error_h

