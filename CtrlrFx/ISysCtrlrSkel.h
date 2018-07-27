/// @file ISysCtrlrSkel.h
///

#ifndef __CtrlrFx_ISysCtrlrSkel_h
#define __CtrlrFx_ISysCtrlrSkel_h

#include "ISysCtrlr.h"
#include "ByteBuffer.h"

interface IHostPort;
interface ISerialCtrlrEvt;

/////////////////////////////////////////////////////////////////////////////
// Interface for system controllers that can accept commands from a
// remote host.

interface ISysCtrlrServer : public virtual ISysCtrlr
{
	// ----- Identity -----

	virtual uint16_t	dev_class() const =0;
	virtual uint16_t	dev_num() const =0;
	virtual uint32_t	target() const =0;

	// ----- Outgoing Events & Data -----

	/*
	virtual void	fire_evt(const ISerialCtrlrEvt& evt) =0;
	virtual void	fire_data(const ByteBuffer& buf) =0;
	*/

	// ----- Incoming Commands & Data -----

	virtual int dispatch_command(const ByteBuffer& cmd, ByteBuffer *rsp) =0;
	virtual int dispatch_data(IHostPort *port, int nLen, ByteBuffer *rsp) =0;

	virtual ~ISysCtrlrServer() {}
};


#endif		// __CtrlrFx_ISysCtrlrSkel_h

