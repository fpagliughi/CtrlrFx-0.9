// DigIO_stub.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/DigIO_stub.h"
#include "CtrlrFx/FixedBuffer.h"
#include "CtrlrFx/BinDistObj.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
// 							DigOut_stub
/////////////////////////////////////////////////////////////////////////////

DigIn_stub::DigIn_stub()
			: obj_srvr_(0), tgt_obj_(tgt_obj(DIST_OBJ_CLASS_DIG_IN, 0))
{
}

DigIn_stub::DigIn_stub(DistObjSrvr* srvr, uint16_t inst)
			: obj_srvr_(srvr), tgt_obj_(tgt_obj(DIST_OBJ_CLASS_DIG_IN, inst))
{
}

// --------------------------------------------------------------------------
// Sends a width request to the remote DigIn port

int DigIn_stub::width()
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	cmd_encoder.put_uint32(obj_srvr_->next_msg_id());
	cmd_encoder.put_uint32(tgt_obj_);
	cmd_encoder.put_uint32(WIDTH_CMD);

	if (obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp) != 0)
		return 0;

	int32_t wd = 0;
	BinNativeDecoder rsp_decoder(rsp);

	rsp_decoder.get(&wd);
	return wd;
}

// --------------------------------------------------------------------------
// Sends a bit value request to the remote DigIn port

bool DigIn_stub::value(int bit)
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	cmd_encoder.put_uint32(obj_srvr_->next_msg_id());
	cmd_encoder.put_uint32(tgt_obj_);
	cmd_encoder.put_uint32(VALUE_CMD);

	cmd_encoder.put_uint32(bit);

	if (obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp) != 0)
		return 0;

	bool val = false;
	BinNativeDecoder rsp_decoder(rsp);

	rsp_decoder.get(&val);
	return val;
}

/////////////////////////////////////////////////////////////////////////////
// 							DigOut_stub
/////////////////////////////////////////////////////////////////////////////

DigOut_stub::DigOut_stub()
			: obj_srvr_(0), tgt_obj_(tgt_obj(DIST_OBJ_CLASS_DIG_OUT, 0))
{
}

DigOut_stub::DigOut_stub(DistObjSrvr* srvr, uint16_t inst)
			: obj_srvr_(srvr), tgt_obj_(tgt_obj(DIST_OBJ_CLASS_DIG_OUT, inst))
{
}

// --------------------------------------------------------------------------
// Encodes a command header into a packet for the given command enumeration.

void DigOut_stub::put_cmd_hdr(BufEncoder& enc, op_t cmd)
{
	enc.put_uint32(obj_srvr_->next_msg_id());
	enc.put_uint32(tgt_obj_);
	enc.put_uint32(cmd);
}

// --------------------------------------------------------------------------
// Sends a width request to the remote DigOut object

int DigOut_stub::width()
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	put_cmd_hdr(cmd_encoder, WIDTH_CMD);

	if (obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp) != 0)
		return 0;

	int32_t wd = 0;
	BinNativeDecoder rsp_decoder(rsp);

	rsp_decoder.get(&wd);
	return wd;
}

// --------------------------------------------------------------------------
// Sends a bit set command to the remote DigOut port.

void DigOut_stub::set(int bit, bool b)
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	put_cmd_hdr(cmd_encoder, SET_CMD);
	cmd_encoder.put_int32(bit);
	cmd_encoder.put_bool(b);

	obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp);
}

// --------------------------------------------------------------------------
// Sends a bit clear command to the remote DigOut port.

void DigOut_stub::clear(int bit)
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	put_cmd_hdr(cmd_encoder, CLEAR_CMD);
	cmd_encoder.put_int32(bit);

	obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp);
}

// --------------------------------------------------------------------------
// Sends a bit toggle command to the remote DigOut port.

void DigOut_stub::toggle(int bit)
{
	FixedBuffer<byte, CMD_SIZE> cmd, rsp;
	BinNativeEncoder cmd_encoder(cmd);

	put_cmd_hdr(cmd_encoder, TOGGLE_CMD);
	cmd_encoder.put_int32(bit);

	obj_srvr_->send(BIN_MSG_TYPE_CMD, cmd.flip(), rsp);
}

