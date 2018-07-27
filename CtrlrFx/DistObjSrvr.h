/// @file DistObjSrvr.h
/// 
/// 

#ifndef __CtrlrFx_DistObjSrvr_h
#define __CtrlrFx_DistObjSrvr_h

#include "CtrlrFx/os.h"
#include "CtrlrFx/IDevice.h"
#include "CtrlrFx/DistObj.h"
#include "CtrlrFx/Guard.h"
#include "CtrlrFx/BinDistObj.h"
#include "CtrlrFx/Array.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class DistObjSrvr
{
public:
	static const size_t DFLT_CMD_RSP_SIZE = 256;

	typedef DistObjRegistry::key_t key_t;

private:	
	typedef Guard<Mutex> MyGuard;

	Mutex			lock_;
	IDevice* 		port_;
	DistObjRegistry	obj_reg_;
	uint32_t		msg_id_;	// @todo this should be atomic

	Buffer<byte>	cmd_buf_,
					rsp_buf_;

	volatile bool	quit_;

	int get_pkt_hdr(BinPktHdr *pkt_hdr);

	// Non-copyable
	DistObjSrvr(const DistObjSrvr&);
	DistObjSrvr& operator=(const DistObjSrvr&);

public:
	DistObjSrvr();
	DistObjSrvr(size_t buf_sz);
	DistObjSrvr(Array<byte>& cmd_arr, Array<byte>& rsp_arr);
	DistObjSrvr(IDevice* port);

	virtual ~DistObjSrvr() {}

	/// Gets the communications port used by the server.
	IDevice* port() const { return port_; }

	/// Sets the communications port used by the server.
	/// This should not be called while the server is running.gy
	void port(IDevice* port) { port_ = port; }

	/// Get the next unique Message ID
	uint32_t next_msg_id() { return msg_id_++; }

	/// Register an object for incoming commands from a remote client.
	int register_obj(key_t key, IDistObj* obj);

	/// Send a message to a remote server
	int send(byte msg_type, ByteBuffer& packet, ByteBuffer& rsp);

	/// Run this server to process incoming messages.
	virtual int run();

	/// Signals a running server to quit and return from the run() function.
	virtual void quit();
};                                                                                                          ;

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
};

#endif		// __CtrlrFx_DistObjSrvr_h

