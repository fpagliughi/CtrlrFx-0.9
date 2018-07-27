/// @file BufBinStream.h
///
/// @author	Frank Pagliughi
/// @author	SoRo Systems, Inc.
/// @author	www.sorosys.com
///
/// @date	January 2004
///
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_BufBinStream_h
#define __CtrlrFx_BufBinStream_h

#include "MemBinStream.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// Class for manipulating in-memory byte buffers using the CtrlrFramework
/// stream library. 
///
/// This is based on the MemBinStream, but sync'ing the buffer (i.e., 
/// flushing the stream) updates the ByteBuffer's Read and Write pointers.
///
/// @par
/// 	The current implementation allows for only input or output stream,
///		even though the underlying buffer could be used for either. The
///		get area is static - set at initialization - and doesn't increase
///		as data is written to the put area. This is consistent with the 
///		original intent of the buffer objects; they are completely filled
///		by producer threads, then completely emptied by consumer threads.

class BufBinStreamBuf : public MemBinStreamBuf
{
protected:
	ByteBuffer	*buf_;

	virtual int sync();

public:
	BufBinStreamBuf() {}
	BufBinStreamBuf(ByteBuffer& buf);

	virtual BufBinStreamBuf* setbuf(byte *p, unsigned n);
	virtual BufBinStreamBuf* setbuf(ByteBuffer& buf);
};

// --------------------------------------------------------------------------

inline BufBinStreamBuf::BufBinStreamBuf(ByteBuffer& buf)
{
	setbuf(buf);
}

// --------------------------------------------------------------------------
// TODO: Figure out if there's something better to be done here.

inline BufBinStreamBuf* BufBinStreamBuf::setbuf(byte *p, unsigned n)
{
	return 0;
}

// --------------------------------------------------------------------------
// Get area is only given the currently available read data; Put area, the
// currently available write area.
//

inline BufBinStreamBuf* BufBinStreamBuf::setbuf(ByteBuffer& buf)
{
	buf_ = &buf;

	setg(buf.GetPtr(), buf.GetPtr(), buf.PutPtr());
	setp(buf.Buf(), buf.PutPtr(), buf.PutPtr() + buf.PutAvail());
	return this;
}

// --------------------------------------------------------------------------
// Synchronizing updates the ByteBuffer's get and put indices to coincide 
// with the stream buffer's get and put pointers, respectively.

inline int BufBinStreamBuf::sync()
{
	buf_->GetPosition(unsigned(gptr() - eback()));
	buf_->Size(unsigned(pptr() - pbase()));
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/// Input memory-based BinStream buffer

class InBufBinStream : public InBinStream
{
protected:
	BufBinStreamBuf	m_buf;

public:
	InBufBinStream(const ByteBuffer& buf);

	virtual bool reset();
};

// --------------------------------------------------------------------------

inline InBufBinStream::InBufBinStream(const ByteBuffer& buf) : InBinStream(&m_buf)
{
	m_buf.setbuf(const_cast<ByteBuffer&>(buf));
}

inline bool InBufBinStream::reset()
{
	state_ = goodbit;
	m_buf.reset(in);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
/// Output memory-based BinStream buffer

class OutBufBinStream : public OutBinStream
{
protected:
	BufBinStreamBuf m_buf;

public:
	OutBufBinStream(ByteBuffer& buf);

	virtual bool reset();
};

// --------------------------------------------------------------------------

inline OutBufBinStream::OutBufBinStream(ByteBuffer& buf) : OutBinStream(&m_buf)
{
	m_buf.setbuf(buf);
}

inline bool OutBufBinStream::reset()
{
	state_ = goodbit;
	m_buf.reset(out);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __BufBinStream_h

