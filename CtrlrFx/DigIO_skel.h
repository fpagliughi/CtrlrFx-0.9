/// @file DigIO_skel.h

#ifndef __CtrlrFx_DigIO_skel_h
#define __CtrlrFx_DigIO_skel_h

#include "DigIO.h"
#include "DistObj.h"

namespace CtrlrFx {
	
/////////////////////////////////////////////////////////////////////////////

class DigIn_skel : virtual public DigIn, virtual public IDistObj
{
	typedef int (DigIn_skel::* MbrPtr)(BufDecoder& param, BufEncoder& rsp);
	struct MethodMap { op_t op; MbrPtr mptr; };
	
	static const MethodMap method_map_[];

	MbrPtr get_mbr_ptr(op_t op);

	// ----- Command Handlers -----

	virtual int width_cmd(BufDecoder& param, BufEncoder& rsp);
	virtual int value_cmd(BufDecoder& param, BufEncoder& rsp);
	
public:
	virtual int invoke(op_t op, BufDecoder& param, BufEncoder& rsp);
};

/////////////////////////////////////////////////////////////////////////////

class DigOut_skel : virtual public DigOut, virtual public IDistObj
{
	typedef int (DigOut_skel::* MbrPtr)(BufDecoder& param, BufEncoder& rsp);
	struct MethodMap { op_t op; MbrPtr mptr; };
	
	static const MethodMap method_map_[];

	MbrPtr get_mbr_ptr(op_t op);

	// ----- Command Handlers -----

	virtual int width_cmd(BufDecoder& param, BufEncoder& rsp);
	virtual int set_cmd(BufDecoder& param, BufEncoder& rsp);
	virtual int clear_cmd(BufDecoder& param, BufEncoder& rsp);
	virtual int toggle_cmd(BufDecoder& param, BufEncoder& rsp);
	
public:
	virtual int invoke(op_t op, BufDecoder& param, BufEncoder& rsp);
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_DigIO_skel_h
