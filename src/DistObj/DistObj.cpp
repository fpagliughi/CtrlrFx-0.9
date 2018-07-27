// DistObj.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/DistObj.h"
#include "CtrlrFx/error.h"
#include "CtrlrFx/debug.h"

using namespace CtrlrFx;

static DistObjRegistry the_dispatcher;

// --------------------------------------------------------------------------

DistObjRegistry::DistObjRegistry()
{
	obj_map_[0].obj = 0;
}

// --------------------------------------------------------------------------

int DistObjRegistry::register_obj(key_t key, IDistObj* obj)
{
	int i=0;

	while (obj_map_[i].obj != 0 && i < MAX_NUM_DIST_OBJ)
		++i;

	if (i == MAX_NUM_DIST_OBJ)
		return -CFXE_NO_MEM;

	obj_map_[i].key = key;
	obj_map_[i].obj = obj;

	DPRINTF3("DistObj registered key 0x%08X for obj %p\n", unsigned(key), obj);

	if (i < MAX_NUM_DIST_OBJ-1)
		obj_map_[++i].obj = 0;
	return i;
}

// --------------------------------------------------------------------------

IDistObj* DistObjRegistry::get_obj(key_t key)
{
	int i=0;
 
	while (obj_map_[i].obj != 0 && i < MAX_NUM_DIST_OBJ) {
		if (obj_map_[i].key == key)
			return obj_map_[i].obj;
		i++;
	}

	DPRINTF3("Unknown dist obj key: 0x%08X\n", unsigned(key));
	#if defined(CFX_DEBUG_LEVEL) && defined(CFX_DUMP_LEVEL) && \
				(CFX_DEBUG_LEVEL >= CFX_DUMP_LEVEL)
		DPRINTF("Known dist obj keys:\n");
		i=0;
		while (obj_map_[i].obj != 0 && i < MAX_NUM_DIST_OBJ) {
			DPRINTF("\t0x%08X for obj %p\n", 
						unsigned(obj_map_[i].key), obj_map_[i].obj);
			i++;
		}
	#endif

	return 0;
}

// --------------------------------------------------------------------------

DistObjRegistry* DistObjRegistry::instance()
{
	return &the_dispatcher;
}

// --------------------------------------------------------------------------

int DistObjRegistry::dispatch(key_t obj_key, op_t op, 
							  BufDecoder& param, BufEncoder& rsp)
{
	IDistObj* p = get_obj(obj_key);
	return (p == 0) ? -CFXE_UNKNOWN_OBJ : p->invoke(op, param, rsp);
}

// --------------------------------------------------------------------------

int DistObjRegistry::dispatch(BufDecoder& cmd, BufEncoder& rsp)
{
	key_t	key;
	op_t	op;
	
	cmd.get_uint32(&key);
	cmd.get_uint32(&op);

	IDistObj* p = get_obj(key);
	return (p == 0) ? -CFXE_UNKNOWN_OBJ : p->invoke(op, cmd, rsp);
}


