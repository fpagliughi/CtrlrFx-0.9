// BufCodec.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/BufCodec.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

int BinSwapEncoder::put_swap2(int16_t v)
{
	if (buf_.available() < 2)
		return 0;

	const byte* b = reinterpret_cast<const byte*>(&v);
	buf_.put(b[1]);
	buf_.put(b[0]);
	return 2;
}

// --------------------------------------------------------------------------

int BinSwapEncoder::put_swap4(int32_t v)
{
	if (buf_.available() < 4)
		return 0;

	const byte* b = reinterpret_cast<const byte*>(&v);
	buf_.put(b[3]);
	buf_.put(b[2]);
	buf_.put(b[1]);
	buf_.put(b[0]);
	return 4;
}

// --------------------------------------------------------------------------

int BinSwapEncoder::put_swap8(int64_t v)
{
	if (buf_.available() < 8)
		return 0;

	const byte* b = reinterpret_cast<const byte*>(&v);
	for (int i=7; i>=0; --i)
		buf_.put(b[i]);
	return 8;
}

/////////////////////////////////////////////////////////////////////////////

int BinSwapDecoder::get_swap2(void* p)
{
	if (buf_.available() < 2)
		return 0;

	byte* b = reinterpret_cast<byte*>(p);
	buf_.get(b+1);
	buf_.get(b);
	return 2;
}

// --------------------------------------------------------------------------

int BinSwapDecoder::get_swap4(void* p)
{
	if (buf_.available() < 4)
		return 0;

	byte* b = reinterpret_cast<byte*>(p);
	buf_.get(b+3);
	buf_.get(b+2);
	buf_.get(b+1);
	buf_.get(b);
	return 4;
}

// --------------------------------------------------------------------------

int BinSwapDecoder::get_swap8(void* p)
{
	if (buf_.available() < 8)
		return 0;

	byte* b = reinterpret_cast<byte*>(p);
	for (int i=7; i>=0; --i)
		buf_.get(b+i);
	return 8;
}


