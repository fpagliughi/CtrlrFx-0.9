// BinStream.cpp
// 
//
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		CtrlrFramework (I/O)
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		December 2003
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/BinStream.h"
#include "CtrlrFx/Buffer.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

InBinStream& InBinStream::operator>>(ByteBuffer& buf)
{
	uint16_t len;
	(*this) >> len;	// TODO: Check len <= buf.capacity()

	get(buf.c_array(), len);
	buf.size(len);

    return *this;
}

int InBinStream::get()
{
	int c;

	state_ = goodbit;
	if ((c = buf_->sbumpc()) < 0)
		state_ |= eofbit | failbit;

	return c;
}

InBinStream& InBinStream::get(byte& by)
{
	int c;

	state_ = goodbit;
	if ((c = buf_->sbumpc()) >= 0)
		by = byte(c);
	else
		state_ |= eofbit | failbit;
	return *this;
}

InBinStream& InBinStream::get(void *buf, streamsize n)
{
	state_ = goodbit;
	if (buf_->sgetn(buf, n) < n)
		state_ |= eofbit | failbit;
	return *this;
}

InBinStream& InBinStream::putback(byte by)
{
	state_ = goodbit;
	if (rdbuf()->sputbackc(by) < 0)
		state_ |= failbit;
	return *this;
}

InBinStream& InBinStream::unget()
{
	state_ = goodbit;
	if (rdbuf()->sungetc() < 0)
		state_ |= failbit;
	return *this;
}

InBinStream& InBinStream::seekg(long pos)
{
	state_ = goodbit;
	if (rdbuf()->pubseekpos(pos, in) < 0)
		state_ |= failbit;
	return *this;
}

InBinStream& InBinStream::seekg(long off, BinStreamBase::seekdir dir)
{
	state_ = goodbit;
	if (rdbuf()->pubseekoff(off, dir, in) < 0)
		state_ |= failbit;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////

OutBinStream& OutBinStream::operator<<(const ByteBuffer& buf)
{
	(*this) << uint16_t(buf.size());
	return write(buf.data(), buf.size());
}

OutBinStream& OutBinStream::put(byte by)
{
	state_ = goodbit;
	if (buf_->sputc(by) < 0)
		state_ |= failbit;
	return *this;
}

OutBinStream& OutBinStream::write(const void* buf, streamsize n)
{
	state_ = goodbit;
	if (buf_->sputn(buf, n) < n)
		state_ |= failbit;
	return *this;
}

OutBinStream& OutBinStream::flush()
{
	state_ = goodbit;
	if (buf_->pubsync() < 0)
		state_ |= failbit;
	return *this;
}

long OutBinStream::tellp()
{
	return (fail()) ? -1 : rdbuf()->pubseekoff(0, cur, out);	
}

OutBinStream& OutBinStream::seekp(long pos)
{
	state_ = goodbit;
	if (rdbuf()->pubseekpos(pos, out) < 0)
		state_ |= failbit;
	return *this;
}

OutBinStream& OutBinStream::seekp(long off, BinStreamBase::seekdir dir)
{
	state_ = goodbit;
	if (rdbuf()->pubseekoff(off, dir, out) < 0)
		state_ |= failbit;
	return *this;
}




