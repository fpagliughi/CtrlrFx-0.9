/// @file BinStream.h
/// Base class for inter-processor communications streams.
///
/// This code borrows heavily from the standard C++ <iostream> library,
/// but is based on binary byte streams rather than arbitrary-width
/// character (text) streams. This code is bare-bones, doesn't use 
/// templates, but like the C++ library, can be easily expanded to any 
/// byte-wide I/O port.
///
/// DETAILS:
///		- Supports state, but not format or positioning.
///		- Comm timeouts set failbit & eof bit.
///		- badbit indicates underlying hardware error.
/// 		- Could eventually support tying, but not yet.
///
///
/// @author	Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date	December 2003

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved

#ifndef __CtrlrFx_BinStream_h
#define __CtrlrFx_BinStream_h

#include "BinStreamBase.h"
#include "BinStreamBuf.h"
#include "Buffer.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
// InBinStream

class InBinStream : public virtual BinStreamBase
{
protected:
	InBinStream() {}
	InBinStream(BinStreamBuf *p) { init(p); }

	template <typename T> InBinStream& extract(T& v);

public:
	// extractors
	InBinStream& operator>>(bool& b);
	InBinStream& operator>>(byte& by);
	InBinStream& operator>>(schar& ch);
	InBinStream& operator>>(int16_t& n);
	InBinStream& operator>>(uint16_t& w);
	InBinStream& operator>>(int32_t& n);
	InBinStream& operator>>(uint32_t& dw);
	InBinStream& operator>>(float& f);
	InBinStream& operator>>(double& d);
	InBinStream& operator>>(long double& ld);
	InBinStream& operator>>(ByteBuffer& buf);

	// byte input
	unsigned gcount() const;
	int get();
	InBinStream& get(byte& by);
	InBinStream& get(void* buf, streamsize n);

	InBinStream& ignore(unsigned n=1);
	int peek();
	InBinStream& read(void *buf, streamsize n);
	InBinStream& readsome(void *buf, streamsize n);

	// buffer management
	InBinStream& putback(byte by);
	InBinStream& unget();
	int sync();

	// positioning
	long tellg();
	InBinStream& seekg(long pos);
	InBinStream& seekg(long off, BinStreamBase::seekdir dir);
};

// --------------------------------------------------------------------------

template <typename T> InBinStream& InBinStream::extract(T& v)
{
	if (align_)
		buf_->galign(sizeof(T));
	
	get(&v, sizeof(T));

	if (swap_)
		swap_bytes(&v);
			
	return *this;
}

inline InBinStream& InBinStream::operator>>(bool& b)
{
	b = (get() > 0);
    return *this;
}

inline InBinStream& InBinStream::operator>>(byte& by)
{
	by = get();
    return *this;
}

inline InBinStream& InBinStream::operator>>(schar& ch)
{
	ch = (schar) get();
    return *this;
}

inline InBinStream& InBinStream::operator>>(int16_t& n)
{
	return extract(n);
}

inline InBinStream& InBinStream::operator>>(uint16_t& w)
{
	return extract(w);
}

inline InBinStream& InBinStream::operator>>(int32& n)
{
	return extract(n);
}

inline InBinStream& InBinStream::operator>>(uint32& dw)
{
	return extract(dw);
}

inline InBinStream& InBinStream::operator>>(float& f)
{
	return get(&f, sizeof(f));
}

inline InBinStream& InBinStream::operator>>(double& d)
{
	return get(&d, sizeof(d));
}

inline InBinStream& InBinStream::operator>>(long double& ld)
{
	return get(&ld, sizeof(ld));
}

inline InBinStream& InBinStream::read(void *buf, streamsize n)
{
	// TODO: Is this the coerrect behavior?
	return get(buf, n);
}

inline unsigned InBinStream::gcount() const
{
	// TODO: Check if this is an acceptable behavior
	return buf_->in_avail();
}

inline int InBinStream::sync()
{
	// TODO: Set stream state?
	return buf_->pubsync();
}

inline long InBinStream::tellg()
{
	return (fail()) ? -1 : rdbuf()->pubseekoff(0, cur, in);	
}

/////////////////////////////////////////////////////////////////////////////

class OutBinStream : public virtual BinStreamBase
{
protected:
	OutBinStream() {}
	OutBinStream(BinStreamBuf *p) { init(p); }

	template <typename T> OutBinStream& insert(T v);

public:
	// inserters
	OutBinStream& operator<<(bool b);
	OutBinStream& operator<<(byte by);
	OutBinStream& operator<<(schar ch);
	OutBinStream& operator<<(int16_t n);
	OutBinStream& operator<<(uint16_t w);
	OutBinStream& operator<<(int32_t n);
	OutBinStream& operator<<(uint32_t dw);
	OutBinStream& operator<<(float f);
	OutBinStream& operator<<(double d);
	OutBinStream& operator<<(long double ld);
	OutBinStream& operator<<(const ByteBuffer& buf);
	OutBinStream& operator<<(OutBinStream& (*pf)(OutBinStream& os));

	// byte output
	OutBinStream& put(byte by);
	OutBinStream& write(const void* buf, streamsize n);

	// buffer management
	OutBinStream& flush();

	// positioning
	long tellp();
	OutBinStream& seekp(long pos);
	OutBinStream& seekp(long off, BinStreamBase::seekdir dir);
};

// --------------------------------------------------------------------------

template <typename T> OutBinStream& OutBinStream::insert(T v)
{
	if (align_)
		buf_->palign(sizeof(T));
	
	if (swap_)
		swap_bytes(&v);
			
	write(&v, sizeof(T));
	return *this;
}

// --------------------------------------------------------------------------

inline OutBinStream& OutBinStream::operator<<(bool b)
{
	return put(byte(b ? 1 : 0));
}

inline OutBinStream& OutBinStream::operator<<(byte by)
{
	return put(by);
}

inline OutBinStream& OutBinStream::operator<<(schar ch)
{
	return put((byte) ch);
}

inline OutBinStream& OutBinStream::operator<<(int16_t n)
{
	return insert(n);
}

inline OutBinStream& OutBinStream::operator<<(uint16_t w)
{
	return insert(w);
}

inline OutBinStream& OutBinStream::operator<<(int32_t n)
{
	return insert(n);
}

inline OutBinStream& OutBinStream::operator<<(uint32_t dw)
{
	return insert(dw);
}

inline OutBinStream& OutBinStream::operator<<(float f)
{
	return write(&f, sizeof(f));
}

inline OutBinStream& OutBinStream::operator<<(double d)
{
	return write(&d, sizeof(d));
}

inline OutBinStream& OutBinStream::operator<<(long double ld)
{
	return write(&ld, sizeof(ld));
}

inline OutBinStream& OutBinStream::operator<<(OutBinStream& (*pf)(OutBinStream&))
{
	(*pf)(*this);
	return *this;
}

inline OutBinStream& flush(OutBinStream& os)
{
	return os.flush();
}

/////////////////////////////////////////////////////////////////////////////

class IOBinStream : public InBinStream, public OutBinStream
{
protected:
	IOBinStream() {}
	IOBinStream(BinStreamBuf *p)
		{ BinStreamBase::init(p); }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __BinStream_h

