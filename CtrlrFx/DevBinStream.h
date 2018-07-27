// DevBinStream.h
//
// Controller Stream and Buffer implementation for any "file" device.
// The underlying implementation uses the "C" stdio library for communicating
// with the device, and so this can apply to anything that uses a file handle,
// including an actual disk file, serial port, etc.
//
// The implementation of the buffer is unbuffered (yeah, I know how that 
// sounds). It's assumed that the hardware has an underlying buffer, as is
// the case with the serial ports
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
//		January 2003
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#ifndef __DevBinStream_h
#define __DevBinStream_h

#include "BinStream.h"

// TODO: Include io.h and all

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
//							InDevBinStreamBuf
/////////////////////////////////////////////////////////////////////////////

class InDevBinStreamBuf : public BinStreamBuf
{
private:
	int	fill_buf();

protected:
	enum {	
		BUF_SIZE = 128,
		BACK_BUF_SIZE = 4
	};

	int		m_rd;
	byte	m_buf[BUF_SIZE + BACK_BUF_SIZE];

	// get area
	virtual unsigned	xsgetn(byte buf[], unsigned n);
	virtual int			underflow();
	//virtual int		uflow();

public:
	InDevBinStreamBuf();

	virtual ~InDevBinStreamBuf();

	bool	is_open() const { return m_rd >= 0; }

	InDevBinStreamBuf*		open(const char *name);
	InDevBinStreamBuf*		close();

	// communication resources
	bool	mode(const char *s);
	bool	timeout(uint32 ms);
};

// --------------------------------------------------------------------------

/*
inline InDevBinStreamBuf::InDevBinStreamBuf(const char *name)
{
	open(name);
}
*/

inline InDevBinStreamBuf::~InDevBinStreamBuf()
{
	close();
}

inline bool InDevBinStreamBuf::mode(const char *s)
{
	// TODO: Is there a way to do this?
	return false;
}

inline bool	InDevBinStreamBuf::timeout(uint32 ms)
{
	// TODO: Is there a way to do this?
	return false;
}

/////////////////////////////////////////////////////////////////////////////
//							OutDevBinStreamBuf
/////////////////////////////////////////////////////////////////////////////

class OutDevBinStreamBuf : public BinStreamBuf
{
protected:
	enum { BUF_SIZE = 512 /*64*/ };

	int		m_wr;				// File Handle
	byte	m_buf[BUF_SIZE];	// Default buffer

	// put area
	virtual unsigned	xsputn(const byte buf[], unsigned n);
	virtual int 		overflow(int c);
	virtual int			sync();

public:
	OutDevBinStreamBuf();

	virtual ~OutDevBinStreamBuf();

	bool	is_open() const { return m_wr >= 0; }

	OutDevBinStreamBuf*	open(const char *name);
	OutDevBinStreamBuf*	close();

	// communication resources
	bool	mode(const char *s);
	bool	timeout(uint32 ms);
};

// --------------------------------------------------------------------------

inline OutDevBinStreamBuf::~OutDevBinStreamBuf()
{
	close();
}

// --------------------------------------------------------------------------

inline bool OutDevBinStreamBuf::mode(const char *s)
{
	// TODO: Is there a way to do this?
	return false;
}

inline bool	OutDevBinStreamBuf::timeout(uint32 ms)
{
	// TODO: Is there a way to do this?
	return false;
}

/////////////////////////////////////////////////////////////////////////////
//							InDevBinStream
/////////////////////////////////////////////////////////////////////////////

class InDevBinStream : public InBinStream
{
protected:
	InDevBinStreamBuf* buf();

public:
	InDevBinStream();
	InDevBinStream(const char *name);
	~InDevBinStream() { delete buf(); }

	bool	is_open() const;

	void	open(const char *name);
	void	close();

	// communication resources
	InDevBinStream&	mode(const char *s);
	InDevBinStream&	timeout(uint32 ms);
};

// --------------------------------------------------------------------------

inline InDevBinStream::InDevBinStream()
{
	init(new InDevBinStreamBuf());
}

inline InDevBinStream::InDevBinStream(const char *name)
{
 	InDevBinStreamBuf *p = new InDevBinStreamBuf();
	init(p);

	state_ = goodbit;
	if (!p->open(name))
		state_ |= failbit;
}

inline InDevBinStreamBuf* InDevBinStream::buf()
{
	return static_cast<InDevBinStreamBuf*>(BinStreamBase::buf_);
}

inline bool	InDevBinStream::is_open() const 
{
	return static_cast<InDevBinStreamBuf*>(buf_)->is_open();
}

inline void InDevBinStream::open(const char *name)
{
	state_ = goodbit;
	if (!buf()->open(name))
		state_ |= failbit;
}

inline void InDevBinStream::close()
{
	buf()->close();
}

inline InDevBinStream& InDevBinStream::mode(const char *s)
{
	state_ = goodbit;
	if (!buf()->mode(s))
		state_ |= failbit;
	return *this;
}

inline InDevBinStream& InDevBinStream::timeout(uint32 ms)
{
	state_ = goodbit;
	if (!buf()->timeout(ms))
		state_ |= failbit;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
//							OutDevBinStream
/////////////////////////////////////////////////////////////////////////////

class OutDevBinStream : public OutBinStream
{
protected:
	OutDevBinStreamBuf* buf();

public:
	OutDevBinStream();
	OutDevBinStream(const char *name);
	~OutDevBinStream() { delete buf(); }

	bool	is_open() const;

	void	open(const char *s);
	void	close();

	// communication resources
	OutDevBinStream&	mode(const char *s);
	OutDevBinStream&	timeout(uint32 ms);
};

// --------------------------------------------------------------------------

inline OutDevBinStream::OutDevBinStream()
{
	init(new OutDevBinStreamBuf());
}

inline OutDevBinStream::OutDevBinStream(const char *name)
{
 	OutDevBinStreamBuf *p = new OutDevBinStreamBuf();
	init(p);

	state_ = goodbit;
	if (!p->open(name))
		state_ |= failbit;
}

inline OutDevBinStreamBuf* OutDevBinStream::buf()
{
	return static_cast<OutDevBinStreamBuf*>(BinStreamBase::buf_);
}

inline bool	OutDevBinStream::is_open() const 
{
	return static_cast<OutDevBinStreamBuf*>(buf_)->is_open();
}

inline void OutDevBinStream::open(const char *name)
{
	state_ = goodbit;
	if (!buf()->open(name))
		state_ |= failbit;
}

inline void OutDevBinStream::close()
{
	buf()->close();
}

inline OutDevBinStream& OutDevBinStream::mode(const char *s)
{
	state_ = goodbit;
	if (!buf()->mode(s))
		state_ |= failbit;
	return *this;
}

inline OutDevBinStream& OutDevBinStream::timeout(uint32 ms)
{
	state_ = goodbit;
	if (!buf()->timeout(ms))
		state_ |= failbit;
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
//							DevBinStream
/////////////////////////////////////////////////////////////////////////////

/*
class DevBinStream : public IOBinStream
{
protected:
	DevBinStreamBuf* buf();

public:
	DevBinStream();
	DevBinStream(const char *s);
	~DevBinStream() { delete buf(); }

	bool	is_open() const;

	void	open(const char *s);
	void	close();

	// communication resources
	DevBinStream&	mode(const char *s);
	DevBinStream&	timeout(uint32 ms);
};

inline DevBinStream::DevBinStream()
{
	init(new DevBinStreamBuf());
}

inline DevBinStream::DevBinStream(const char *name)
{
 	DevBinStreamBuf *p = new DevBinStreamBuf();
	init(p);

	state = goodbit;
	if (!p->open(name))
		state |= failbit;
}

inline DevBinStreamBuf* DevBinStream::buf()
{
	return static_cast<DevBinStreamBuf*>(BinStreamBase::buf_);
}

inline bool	DevBinStream::is_open() const 
{
	return static_cast<DevBinStreamBuf*>(buf_)->is_open();
}

inline void DevBinStream::open(const char *name)
{
	state = goodbit;
	if (!buf()->open(name))
		state |= failbit;
}

inline void DevBinStream::close()
{
	buf()->close();
}

inline DevBinStream& DevBinStream::mode(const char *s)
{
	state = goodbit;
	if (!buf()->mode(s))
		state |= failbit;
	return *this;
}

inline DevBinStream& DevBinStream::timeout(uint32 ms)
{
	state = goodbit;
	if (!buf()->timeout(ms))
		state |= failbit;
	return *this;
}
*/

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __DevBinStream_h

