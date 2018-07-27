/// @file ISysClient.h
/// Interface definition for a system controller client. 
///
/// A system client is an object that can receive asynchronous events from a
/// system controller (ISysCtrlr). In oreder to receive events, the client 
/// must register itself with the controller by calling the controller object's
/// ISysCtrlr::register_client() method.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date April 2003

// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved

#ifndef __CtrlrFx_ISysClient_h
#define __CtrlrFx_ISysClient_h

#include "ByteBuffer.h"

namespace CtrlrFx {

interface ICtrlrEvt;
interface ISysCtrlr;

/////////////////////////////////////////////////////////////////////////////
/// Client to a System Controller (ISysCtrlr)

interface ISysClient
{
	/// Handler for an asynchronous event from the controller.
	virtual void on_ctrlr_evt(const ISysCtrlr& ctrlr, const ICtrlrEvt& evt) =0;

	/// Handler for a data message from the controller.
	virtual void on_ctrlr_data(const ISysCtrlr& ctrlr, const ByteBuffer& buf) =0;

	virtual ~ISysClient() {}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_ISysClient_h

