/// @file DigIO_stub.h

#ifndef __CtrlrFx_DigIO_stub_h
#define __CtrlrFx_DigIO_stub_h

#include "CtrlrFx/DigIO.h"
#include "CtrlrFx/DistObjSrvr.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////	

class DigIn_stub : public DigIn
{
	static const size_t CMD_SIZE = DistObjSrvr::DFLT_CMD_RSP_SIZE;

	DistObjSrvr*	obj_srvr_;
	uint32_t		tgt_obj_;

public:
	DigIn_stub();
	DigIn_stub(DistObjSrvr* srvr, uint16_t inst);

	DistObjSrvr* obj_srvr() const { return obj_srvr_; }
	void obj_srvr(DistObjSrvr* srvr) { obj_srvr_ = srvr; }

	/// Gets the width (# of bits) in the port
	virtual int width();

	/// Gets the current value of the bit (true=high)
	virtual bool value(int bit);
};

/////////////////////////////////////////////////////////////////////////////	

class DigOut_stub : public DigOut
{
	static const size_t CMD_SIZE = DistObjSrvr::DFLT_CMD_RSP_SIZE;

	typedef IDistObj::op_t op_t;

	DistObjSrvr*	obj_srvr_;
	uint32_t		tgt_obj_;

	void put_cmd_hdr(BufEncoder& enc, op_t cmd);

public:
	DigOut_stub();
	DigOut_stub(DistObjSrvr* srvr, uint16_t inst);

	DistObjSrvr* obj_srvr() const { return obj_srvr_; }
	void obj_srvr(DistObjSrvr* srvr) { obj_srvr_ = srvr; }
	
	/// Gets the width (# of bits) in the port
	virtual int width();

	/// Sets the output bit high.
	virtual void set(int bit) { set(bit, true); }
	
	/// Sets the output bit to the specified value (true=high).
	virtual void set(int bit, bool b);
	
	/// Sets the output bit low.
	virtual void clear(int bit);
	
	/// Toggles the output bit.
	virtual void toggle(int bit);
};

/////////////////////////////////////////////////////////////////////////////	
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_DigIO_stub_h
