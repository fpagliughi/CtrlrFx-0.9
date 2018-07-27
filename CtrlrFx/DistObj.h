/// @file DistObj.h
/// Base definitions for distributed objects.

#ifndef __CtrlrFx_DistObj_h
#define __CtrlrFx_DistObj_h

#include "BufCodec.h"

#ifndef CFX_MAX_NUM_DIST_OBJ
	#define CFX_MAX_NUM_DIST_OBJ 64
#endif

namespace CtrlrFx {
	
/////////////////////////////////////////////////////////////////////////////

enum {
	DIST_OBJ_MAIN,
	DIST_OBJ_DIG_IN_BIT,
	DIST_OBJ_DIG_OUT_BIT,
	DIST_OBJ_DIG_IO_BIT,
	DIST_OBJ_DIG_IN_PORT,
	DIST_OBJ_DIG_OUT_PORT,
	DIST_OBJ_DIG_IO_PORT,
	DIST_OBJ_ANALOG_IN,
	DIST_OBJ_ANALOG_OUT,
	DIST_OBJ_PWM,
	DIST_OBJ_MOTOR
};

/////////////////////////////////////////////////////////////////////////////
/// Interface for distributed objects

interface IDistObj
{
	typedef uint32_t op_t;

	virtual int invoke(op_t op, BufDecoder& param, BufEncoder& rsp) =0;

	virtual ~IDistObj() {}
};

/////////////////////////////////////////////////////////////////////////////

class DistObjRegistry
{
public:
	typedef uint32_t		key_t;
	typedef IDistObj::op_t	op_t;

private:	
	static const int MAX_NUM_DIST_OBJ = CFX_MAX_NUM_DIST_OBJ;
	struct ObjMap { key_t key; IDistObj* obj; };

	ObjMap obj_map_[MAX_NUM_DIST_OBJ];
	
public:
	DistObjRegistry();

	DistObjRegistry* instance();

	int register_obj(key_t key, IDistObj* obj);

	IDistObj* get_obj(key_t key);

	int dispatch(BufDecoder& cmd, BufEncoder& rsp);
	int dispatch(key_t obj_key, op_t op, BufDecoder& param, BufEncoder& rsp);
};

/////////////////////////////////////////////////////////////////////////////
/*
template <typename T>
class DistObj : public virtual IDistObj
{
	typedef int (T::* MbrPtr)(BinNativeDecoder& param, BinNativeEncoder& rsp);
	struct MethodMap { op_t op; MbrPtr mptr; };

	/// Derived implementation class must define this: 
	/// static const MbrPtr method_map_[];

	MbrPtr get_mbr_ptr(op_t op);
};
*/
/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};	

#endif		// __CtrlrFx_DistObj_h

