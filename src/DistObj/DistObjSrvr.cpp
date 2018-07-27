// DistObjSrvr.cpp
// 

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/DistObjSrvr.h"
#include "CtrlrFx/BinDistObj.h"
#include "CtrlrFx/FixedBuffer.h"
#include "CtrlrFx/debug.h"
#include "CtrlrFx/error.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

DistObjSrvr::DistObjSrvr() : port_(0), msg_id_(0), 
					cmd_buf_(DFLT_CMD_RSP_SIZE), rsp_buf_(DFLT_CMD_RSP_SIZE),
					quit_(false)
{
}

DistObjSrvr::DistObjSrvr(size_t buf_sz) : port_(0), msg_id_(0), 
					cmd_buf_(buf_sz), rsp_buf_(buf_sz),
					quit_(false)
{
}

DistObjSrvr::DistObjSrvr(Array<byte>& cmd_arr, Array<byte>& rsp_arr)
				 : port_(0), msg_id_(0), 
					cmd_buf_(cmd_arr.c_array(), cmd_arr.size()), 
					rsp_buf_(rsp_arr.c_array(), rsp_arr.size()), 
					quit_(false)
{
}

DistObjSrvr::DistObjSrvr(IDevice* port) : port_(port), msg_id_(0),
					cmd_buf_(DFLT_CMD_RSP_SIZE), rsp_buf_(DFLT_CMD_RSP_SIZE),
					quit_(false)
{
}

// --------------------------------------------------------------------------
//							Protected Members
// --------------------------------------------------------------------------

// This makes a concerted effort to load a packet header from the input
// stream. It will scan the input looking for the "CX" bytes that denote
// the start of the header, throwing away any bytes received before then.

int DistObjSrvr::get_pkt_hdr(BinPktHdr *pkt_hdr)
{
	int		n, nx = 0;
	byte	magic[2];

	do {  
		if ((n = port_->read_n(magic+nx, 2-nx)) <= 0)
			return -1;

		if (magic[1] == 'C') {
			magic[0] = 'C';
			magic[1] = '\0';
			nx = 1;
		}
	}
	while (memcmp(magic, "CX", 2) && !quit_);

	if (quit_)
		return -CFXE_SRVR_CLOSING;

	byte* b = reinterpret_cast<byte*>(pkt_hdr);
	memcpy(b, "CX", 2);
    n = port_->read_n(b+2, sizeof(BinPktHdr)-2);

	return (n < 0) ? n : 0;
}

// --------------------------------------------------------------------------
//								Public Interface
// --------------------------------------------------------------------------

int DistObjSrvr::register_obj(key_t key, IDistObj* obj)
{
	MyGuard g(lock_);
	int n = obj_reg_.register_obj(key, obj);
	g.release();
	return n;
}

// --------------------------------------------------------------------------
// Send a packet to the remote server.

int DistObjSrvr::send(byte msg_type, ByteBuffer& packet, ByteBuffer& rsp)
{
	// TODO: Set real byte order
	BinPktHdr	pkt_hdr = { { 'C', 'X' }, 0, msg_type, (uint32_t) packet.available() },
				rsp_pkt_hdr;
	BinRspHdr	rsp_hdr;

	MyGuard g(lock_);

	// Send the packet
	
	if (port_->write_n(&pkt_hdr, sizeof(pkt_hdr)) < 0)
		return -CFXE_PACKET_WRITE;

	if (port_->write_n(packet) < 0)
		return -CFXE_PACKET_WRITE;

	// Wait for a response

	if (port_->read_n(&rsp_pkt_hdr, sizeof(rsp_pkt_hdr)) != sizeof(rsp_pkt_hdr))
		return -CFXE_PACKET_READ;

	if (port_->read_n(&rsp_hdr, sizeof(rsp_hdr)) != sizeof(rsp_pkt_hdr))
		return -CFXE_PACKET_READ;

	uint32_t n = rsp_pkt_hdr.msg_size - sizeof(rsp_hdr);

	if (n != 0) {
		rsp.size(n);
		port_->read_n(rsp);
		rsp.flip();
	}

	return (int) rsp_hdr.reply;
}

// --------------------------------------------------------------------------
// Signal the run routine to quit.

void DistObjSrvr::quit()
{
	// TODO: We need a way to knock the run() off a read_n()
	// This won't take effect until the next packet arrives from the client
	quit_ = true;
}

// --------------------------------------------------------------------------
// Run the server to receive and process incoming packets.
// This will exit when a read gets an error or zero-sized packet.
// The latter is typical of a comm port closing.

int DistObjSrvr::run()
{
	BinPktHdr	pkt_hdr;
	BinCmdHdr	cmd_hdr;
	BinRspHdr	rsp_hdr;

	BinNativeDecoder cmd_decoder(cmd_buf_);
	BinNativeEncoder rsp_encoder(rsp_buf_);

	while (!quit_) {

		int n = get_pkt_hdr(&pkt_hdr);

		if (quit_)
			break;

		if (n < 0) {
			DPRINTF("DistObjSrvr: get_pkt_hdr() returned %d\n", n);
			break;
		}

		DPRINTF3("DistObjSrvr: Got packet header for %u byte payload\n",
				(unsigned) pkt_hdr.msg_size);
		DUMP_BUF(&pkt_hdr, sizeof(pkt_hdr));

		// Get the packet payload
		// TODO: make sure payload won't overrun the buffer

		cmd_buf_.clear(pkt_hdr.msg_size);
		port_->read_n(cmd_buf_);

		cmd_buf_.flip();
		cmd_decoder.rewind();

		if (pkt_hdr.msg_type != BIN_MSG_TYPE_CMD) {
			DPRINTF("Received something other than command: %u\n",
					(unsigned) pkt_hdr.msg_type);
			continue;
		}

		DPRINTF3("Cmd Buf has %u bytes\n", (unsigned) cmd_buf_.available());
		DUMP_BUF(cmd_buf_.position_ptr(), cmd_buf_.available());

		// Extract the command header

		cmd_decoder.get_uint32(&cmd_hdr.msg_id);
		cmd_decoder.get_uint32(&cmd_hdr.tgt_obj);
		cmd_decoder.get_uint32(&cmd_hdr.operation);

		DPRINTF3("DistObjSrvr: Got command request, Obj: 0x%08x, Op: %u\n",
					(unsigned) cmd_hdr.tgt_obj, (unsigned) cmd_hdr.operation);

		// Invoke the command

		rsp_encoder.clear();

		MyGuard g(lock_);
		int ret = obj_reg_.dispatch(cmd_hdr.tgt_obj, cmd_hdr.operation,
									cmd_decoder, rsp_encoder);
		g.release();

		// If an error occured, throw away the response and send 
		// just the error code back to the client

		if (ret < 0) {
			DPRINTF("DistObjSrvr: Command failed with code: %d\n", ret);
		}
		else {
			DPRINTF3("DistObjSrvr: Command Succeeded\n");
		}

		// Send the response back to client

		rsp_buf_.flip();

		pkt_hdr.msg_type = BIN_MSG_TYPE_RSP;
		pkt_hdr.msg_size = sizeof(rsp_hdr) + rsp_buf_.available();

		rsp_hdr.msg_id = cmd_hdr.msg_id;
		rsp_hdr.reply = ret;

		DPRINTF3("DistObjSrvr: Returning %u byte response\n", (unsigned) rsp_buf_.available());

		g.acquire();
		port_->write_n(&pkt_hdr, sizeof(pkt_hdr));
		port_->write_n(&rsp_hdr, sizeof(rsp_hdr));

		if (pkt_hdr.msg_size > 0)
			port_->write_n(rsp_buf_);
	}

	return 0;
}

