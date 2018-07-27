// BinStreamBase.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/BinStreamBase.h"

using namespace CtrlrFx;

#if defined(__BORLANDC__)
	typedef BinStreamBase::iostate	iostate;
	typedef BinStreamBase::openmode	openmode;
	typedef BinStreamBase::seekdir	seekdir;

	const iostate	BinStreamBase::goodbit	= (iostate) 0x00;
	const iostate	BinStreamBase::eofbit	= (iostate) 0x01;
	const iostate	BinStreamBase::failbit	= (iostate) 0x02;
	const iostate	BinStreamBase::badbit	= (iostate) 0x04;

	const openmode	BinStreamBase::in 		= (openmode) 0x01;
	const openmode	BinStreamBase::out		= (openmode) 0x02;

	const seekdir	BinStreamBase::beg 		= (seekdir) 0x00;
	const seekdir	BinStreamBase::cur 		= (seekdir) 0x01;
	const seekdir	BinStreamBase::end 		= (seekdir) 0x02;
#endif

// --------------------------------------------------------------------------
// Determines the machine's native byte order. If the '1' byte is placed
// at the word's low memory, the machine is little endian.

BinStreamBase::ByteOrder BinStreamBase::host_byte_order()
{
	int32_t n = 1;
	byte* b = reinterpret_cast<byte*>(&n);
	return (*b == 1) ? BYTE_ORDER_LITTLE_ENDIAN
					: BYTE_ORDER_BIG_ENDIAN;
}

// --------------------------------------------------------------------------
// Gets the byte order for swapped words (the opposite of the native format).

BinStreamBase::ByteOrder BinStreamBase::swapped_byte_order()
{
	int32_t n = 1;
	byte* b = reinterpret_cast<byte*>(&n);
	return (*b == 1) ? BYTE_ORDER_BIG_ENDIAN
					: BYTE_ORDER_LITTLE_ENDIAN;
}

// --------------------------------------------------------------------------
// Gets the byte order of the stream

BinStreamBase::ByteOrder BinStreamBase::byte_order() const
{
	return (swap_) ? host_byte_order() : swapped_byte_order();
}

void BinStreamBase::byte_order(ByteOrder order)
{
	swap_ = (order == swapped_byte_order());
}


