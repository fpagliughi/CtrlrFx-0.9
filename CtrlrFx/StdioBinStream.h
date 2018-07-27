/// @file DevBinStream.h
/// Controller Stream and Buffer implementation for any "file" device.
///
/// The underlying implementation uses the "C" stdio library for communicating
/// with the device, and so this can apply to anything that uses a file handle,
/// including an actual disk file, serial port, etc.
///
/// The implementation of the buffer is unbuffered (yeah, I know how that 
/// sounds). It's assumed that the hardware has an underlying buffer, as is
/// the case with the serial ports
///
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
/// @author www.sorosys.com
///
/// @date   January 2004

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __CtrlrFx_StdioBinStream_h
#define __CtrlrFx_StdioBinStream_h

#include "BinStream.h"
#include <stdlib.h>
#include <stdio.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class StdioBinStreamBuf : public BinStreamBuf
{
protected:
	int		byte_buf_;
	FILE	*rd_,
			*wr_;

	// get area
	virtual unsigned	xsgetn(byte buf[], unsigned n);
	virtual int			underflow();
	//virtual int		uflow();

	virtual unsigned	xsputn(const byte buf[], unsigned n);
	virtual int 		overflow(int c);

	virtual int			sync();


public:
	StdioBinStreamBuf() : byte_buf_(-1), rd_(0), wr_(0) {}
	//StdioBinStreamBuf(const char *name);
	//StdioBinStreamBuf(HANDLE h) : h_(h), byte_buf_(-1) {}

	virtual ~StdioBinStreamBuf();

	bool	is_open() const { return rd_ != 0 && wr_ != 0; }

	StdioBinStreamBuf*	open(const char *name);
	StdioBinStreamBuf*	close();

	// communication resources
	bool	mode(const char *s);
	bool	timeout(uint32 ms);
};

// --------------------------------------------------------------------------

/*
inline StdioBinStreamBuf::StdioBinStreamBuf(const char *name) : byte_buf(-1)
{
	open(name);
}
*/

inline StdioBinStreamBuf::~StdioBinStreamBuf()
{
	close();
}

inline bool StdioBinStreamBuf::mode(const char *s)
{
	// TODO: Is there a way to do this?
	return false;
}

inline bool	StdioBinStreamBuf::timeout(uint32 ms)
{
	// TODO: Is there a way to do this?
	return false;
}

inline unsigned StdioBinStreamBuf::xsgetn(byte buf[], unsigned n)
{
	return (unsigned) ::fread(buf, 1, n, rd_);
}

inline int StdioBinStreamBuf::underflow()
{
	return ::getc(rd_);
}

inline unsigned StdioBinStreamBuf::xsputn(const byte buf[], unsigned n)
{
	return (unsigned) ::fwrite(buf, 1, n, wr_);
}

inline int StdioBinStreamBuf::overflow(int c)
{
	return ::putc(c, wr_);
}

inline int StdioBinStreamBuf::sync()
{
	return ::fflush(wr_);
}

/////////////////////////////////////////////////////////////////////////////

class StdioBinStream : public IOBinStream
{
protected:
	StdioBinStreamBuf* buf();

public:
	StdioBinStream();
	StdioBinStream(const char *s);
	~StdioBinStream() { delete buf(); }

	bool	is_open() const;

	void	open(const char *s);
	void	close();

	// communication resources
	StdioBinStream&	mode(const char *s);
	StdioBinStream&	timeout(uint32 ms);
};

inline StdioBinStream::StdioBinStream()
{
	init(new StdioBinStreamBuf());
}

inline StdioBinStreamBuf* StdioBinStream::buf()
{
	return static_cast<StdioBinStreamBuf*>(BinStreamBase::buf_);
}

inline bool	StdioBinStream::is_open() const 
{
	return static_cast<StdioBinStreamBuf*>(buf_)->is_open();
}

inline void StdioBinStream::close()
{
	buf()->close();
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_StdioBinStream_h

