// DigIO_skel.cpp
// Example of a remote object skeleton.
// 
// This contains implementations for a digital input port and a digital 
// output port. 
// 

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/DigIO_skel.h"
#include "CtrlrFx/debug.h"
#include "CtrlrFx/error.h"

using namespace std;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//							Dig In Bit
/////////////////////////////////////////////////////////////////////////////

const DigIn_skel::MethodMap DigIn_skel::method_map_[] =
{
	{ WIDTH_CMD, &DigIn_skel::width_cmd },
	{ VALUE_CMD, &DigIn_skel::value_cmd },
	{ op_t(-1), NULL }
};

// --------------------------------------------------------------------------

DigIn_skel::MbrPtr DigIn_skel::get_mbr_ptr(op_t op)
{
	int i=0;
	
	while (method_map_[i].mptr != NULL) {
		if (method_map_[i].op == op)
			return method_map_[i].mptr;
		i++;
	}
	
	return NULL;
}

// --------------------------------------------------------------------------

int DigIn_skel::invoke(op_t op, BufDecoder& param, BufEncoder& rsp)
{
	DTRACE_FUNC();

	MbrPtr p = get_mbr_ptr(op);
	return (p == 0) ? -1 : ((this->*p)(param, rsp));
}

// --------------------------------------------------------------------------
//						Dig In Command Handlers
// --------------------------------------------------------------------------

int DigIn_skel::width_cmd(BufDecoder&, BufEncoder& rsp)
{
	DTRACE_FUNC();
	uint32_t wd = this->width();

	rsp.put(wd);
	return 0;
}

// --------------------------------------------------------------------------


int DigIn_skel::value_cmd(BufDecoder& param, BufEncoder& rsp)
{
	DTRACE_FUNC();

	int32_t bit;
	param.get(&bit);

	bool b = this->value(bit);
	rsp.put_bool(b);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
//								Dig Out
/////////////////////////////////////////////////////////////////////////////

const DigOut_skel::MethodMap DigOut_skel::method_map_[] =
{
	{ WIDTH_CMD,	&DigOut_skel::width_cmd },
	{ SET_CMD,		&DigOut_skel::set_cmd },
	{ CLEAR_CMD,	&DigOut_skel::clear_cmd },
	{ TOGGLE_CMD,	&DigOut_skel::toggle_cmd },
	{ op_t(-1), 	NULL }
};

// --------------------------------------------------------------------------

DigOut_skel::MbrPtr DigOut_skel::get_mbr_ptr(op_t op)
{
	int i=0;
	
	while (method_map_[i].mptr != NULL) {
		if (method_map_[i].op == op)
			return method_map_[i].mptr;
		i++;
	}
	
	return NULL;
}

// --------------------------------------------------------------------------

int DigOut_skel::invoke(op_t op, BufDecoder& param, 
						   BufEncoder& rsp)
{
	DTRACE_FUNC();

	MbrPtr p = get_mbr_ptr(op);
	return (p == 0) ? -CFXE_UNKNOWN_CMD : ((this->*p)(param, rsp));
}

// --------------------------------------------------------------------------
//						Dig Out Command Handlers
// --------------------------------------------------------------------------

int DigOut_skel::width_cmd(BufDecoder&, BufEncoder& rsp)
{
	DTRACE_FUNC();
	uint32_t wd = this->width();

	rsp.put(wd);
	return 0;
}

// --------------------------------------------------------------------------

int DigOut_skel::set_cmd(BufDecoder& param, BufEncoder&)
{
	DTRACE_FUNC();

	int32_t bit;
	param.get(&bit);

	bool b = true;
	param.get(&b);	// It's ok if this fails (optional param)

	this->set(bit, b);
	return 0;
}

// --------------------------------------------------------------------------

int DigOut_skel::clear_cmd(BufDecoder& param, BufEncoder&)
{
	DTRACE_FUNC();

	int32_t bit;
	param.get(&bit);

	this->clear(bit);
	return 0;
}

// --------------------------------------------------------------------------

int DigOut_skel::toggle_cmd(BufDecoder& param, BufEncoder&)
{
	DTRACE_FUNC();

	int32_t bit;
	param.get(&bit);

	this->toggle(bit);
	return 0;
}
