// StdioBinStream.cpp
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
//		January 2004
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/StdioBinStream.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------
//								StdioBinStreamBuf
// --------------------------------------------------------------------------

StdioBinStreamBuf* StdioBinStreamBuf::open(const char *name)
{
	close();
	rd_ = ::fopen(name, "r");
	wr_ = ::fopen(name, "w");
	return is_open() ? this : NULL;
}

// --------------------------------------------------------------------------

StdioBinStreamBuf* StdioBinStreamBuf::close()
{
	if (rd_) {
		fclose(rd_);
		rd_ = 0;
	}

	if (wr_) {
		fclose(wr_);
		wr_ = 0;
	}

	return this;
}

// --------------------------------------------------------------------------
//								StdioBinStream
// --------------------------------------------------------------------------

StdioBinStream::StdioBinStream(const char *name)
{
 	StdioBinStreamBuf *p = new StdioBinStreamBuf();
	init(p);

	state_ = goodbit;
	if (!p->open(name))
		state_ |= failbit;
}

inline void StdioBinStream::open(const char *name)
{
	state_ = goodbit;
	if (!buf()->open(name))
		state_ |= failbit;
}

inline StdioBinStream& StdioBinStream::mode(const char *s)
{
	state_ = goodbit;
	if (!buf()->mode(s))
		state_ |= failbit;
	return *this;
}

inline StdioBinStream& StdioBinStream::timeout(uint32 ms)
{
	state_ = goodbit;
	if (!buf()->timeout(ms))
		state_ |= failbit;
	return *this;
}
