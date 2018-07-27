/// MemBinStream.h
/// Classes for manipulating in-memory byte arrays using the
/// Controller Stream library.
/// 
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date December 2003

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_MemBinStream_h
#define __CtrlrFx_MemBinStream_h

#include "BinStream.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class MemBinStreamBuf : public BinStreamBuf
{
public:
	MemBinStreamBuf() {}
	MemBinStreamBuf(byte *p, streamsize n);

	virtual MemBinStreamBuf* setbuf(byte *p, streamsize n);

	void reset(BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);

	virtual long seekoff(long off, BinStreamBase::seekdir dir,
							BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);
	virtual long seekpos(long pos, BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);

};

// --------------------------------------------------------------------------

inline MemBinStreamBuf::MemBinStreamBuf(byte *p, streamsize n)
{
	setbuf(p, n);
}

// --------------------------------------------------------------------------

inline MemBinStreamBuf* MemBinStreamBuf::setbuf(byte *p, streamsize n)
{
	setg(p, p, p+n);
	setp(p, p+n);
	return this;
}

// --------------------------------------------------------------------------

inline void MemBinStreamBuf::reset(BinStreamBase::openmode mode)
{
	if (mode & BinStreamBase::in)
		gbump(streamsize(eback() - gptr()));

	if (mode & BinStreamBase::out)
		pbump(streamsize(pbase() - pptr()));
}

/////////////////////////////////////////////////////////////////////////////
//								InMemBinStream
/////////////////////////////////////////////////////////////////////////////

class InMemBinStream : public InBinStream
{
protected:
	MemBinStreamBuf	buf_;

public:
	InMemBinStream(const byte *p, unsigned n);

	virtual bool reset();
};

inline InMemBinStream::InMemBinStream(const byte *p, unsigned n) : InBinStream(&buf_)
{
	buf_.setbuf(const_cast<byte*>(p), n);
}

inline bool InMemBinStream::reset()
{
	state_ = goodbit;
	buf_.reset(in);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
//								OutMemBinStream
/////////////////////////////////////////////////////////////////////////////

class OutMemBinStream : public OutBinStream
{
protected:
	MemBinStreamBuf	buf_;

public:
	OutMemBinStream(byte *p, streamsize n);

	virtual bool reset();
};

// --------------------------------------------------------------------------

inline OutMemBinStream::OutMemBinStream(byte *p, streamsize n) : OutBinStream(&buf_)
{
	buf_.setbuf(p, n);
}

// --------------------------------------------------------------------------

inline bool OutMemBinStream::reset()
{
	state_ = goodbit;
	buf_.reset(out);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __MemBinStream_h

