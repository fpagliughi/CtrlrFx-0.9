/// @file ISysCtrlr.h
///
/// C++ version of SysCtrlr interface (especially for target-side apps) .
///
///
/// @author	Frank Pagliughi
/// @author	SoRo Systems, Inc.
///	@author	www.sorosys.com
///
/// @date	April 2003

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_ISysCtrlr_h
#define __CtrlrFx_ISysCtrlr_h

#include "EnumDef.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

ENUM_BEGIN(CTRLR_CLASS)
	CTRLR_CLASS_MAIN,
	CTRLR_CLASS_DIGITAL_IO,
	CTRLR_CLASS_ANALOG_IO
ENUM_END(CTRLR_CLASS)

interface ISysClient;

/////////////////////////////////////////////////////////////////////////////
/// Base interface for System Controllers.

interface ISysCtrlr
{
 	// ----- Client Registration -----

 	virtual int		register_client(ISysClient *client) =0;

	// ----- Commands -----

	/// Get the version number of the controller
	/// @returns A 32-bit integer version number
	virtual uint32_t version() const =0;

	/// Get the current operating state of the controller.
	virtual int state() const =0;

	/// Reset the controller back to a known state.
	virtual int reset() =0;

	/// Stops the current operation and returns the controller to an idle state.
	virtual int stop() =0;

	virtual ~ISysCtrlr() {}
};

/////////////////////////////////////////////////////////////////////////////
// 
#endif		// __CtrlrFx_ISysCtrlr_h
