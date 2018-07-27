/// @file BinDistObj.h
/// Definitions for binary wire protocol for distributed objects.
/// 
/// @author	Frank Pagliughi
/// @date	January 2006

#ifndef __CtrlrFx_BinDistObj_h
#define __CtrlrFx_BinDistObj_h

namespace CtrlrFx {

enum {
	BIN_MSG_TYPE_CMD,
	BIN_MSG_TYPE_RSP,
	BIN_MSG_TYPE_EVT,
	BIN_MSG_TYPE_DATA,
};

/////////////////////////////////////////////////////////////////////////////

enum {
	DIST_OBJ_CLASS_APP,			///< Normally the application object
	DIST_OBJ_CLASS_DIG_IN,		///< Digital Input Port
	DIST_OBJ_CLASS_DIG_OUT		///< Digital Output Port
};

inline uint32_t tgt_obj(uint16_t obj_class, uint16_t obj_inst)
{
	return (uint32_t(obj_class) << 16) | obj_inst;
}

/////////////////////////////////////////////////////////////////////////////
/// The header structure for binary packets

struct BinPktHdr
{
	char		magic[2];		///< Magic number "CX"
	byte		byte_order,		///< Byte Order
				msg_type;		///< Message Type: Cmd, Rsp, Evt, Data
	uint32_t	msg_size;		///< The number of bytes to follow
};

/////////////////////////////////////////////////////////////////////////////
/// Header structure for commands

struct BinCmdHdr
{
	uint32_t	msg_id,			///< Command message ID. Unique number.
				tgt_obj,		///< The target object
				operation;		///< The command operation
};

/////////////////////////////////////////////////////////////////////////////
/// Header structure for command responses

struct BinRspHdr
{
	uint32_t	msg_id;		///< Msg ID matches the command/request
	int32_t		reply;		///< Reply to the message. 0 normally means @em success
};

/////////////////////////////////////////////////////////////////////////////
/// Header structure for asynchronous events.

struct BinEvtHdr
{
	uint32_t	msg_id;			///< Message ID. Unique number.
	uint16_t	evt_num,		///< Event number. Specific to evt class & app
				evt_class;		///< Event class: Status, Warning, Error
};

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
};

#endif		// __CtrlrFx_BinDistObj_h

