/// @file ICtrlrEvt.h
/// Interface for asynchronous events thrown by a system controller.
///
/// For controllers that interface with a remote host (iSysCtrlrServer) there's
/// the iSerialCtrlrEvt interface for events that will be shipped back up out
/// a host port.
///
/// @author	Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author	www.sorosys.com
///
/// @date	April 2003

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_ICtrlrEvt_h
#define __CtrlrFx_ICtrlrEvt_h

#include "EnumDef.h"
#include "iSerializable.h"

ENUM_BEGIN(CTRLR_EVT_CLASS)
	CTRLR_EVT_CLASS_ERROR,
	CTRLR_EVT_CLASS_WARNING,
	CTRLR_EVT_CLASS_STATUS
ENUM_END(CTRLR_EVT_CLASS)

/////////////////////////////////////////////////////////////////////////////

interface ICtrlrEvt
{
	virtual byte	EvtClass() const =0;
	virtual byte	EvtType() const =0;

	virtual ~iCtrlrEvt() {}
};

/////////////////////////////////////////////////////////////////////////////
/*
interface ISerialCtrlrEvt : public virtual ICtrlrEvt, public virtual ISerializable
{
	virtual ~iSerialCtrlrEvt() {}
};

// --------------------------------------------------------------------------

inline OutBinStream& operator<<(OutBinStream& os, const ISerialCtrlrEvt& evt)
{
	return evt.write(os);
}

inline InBinStream& operator>>(InBinStream& is, ISerialCtrlrEvt& evt)
{
	return evt.read(is);
}
*/

/////////////////////////////////////////////////////////////////////////////

interface ICtrlrErrorEvt : public virtual ICtrlrEvt
{
};


/////////////////////////////////////////////////////////////////////////////

interface ICtrlrWarningEvt : public virtual ICtrlrEvt
{
};


/////////////////////////////////////////////////////////////////////////////

interface ICtrlrStatusEvt : public virtual ICtrlrEvt
{
};

#endif		// __iCtrlrEvt_h

