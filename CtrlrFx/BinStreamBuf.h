/// @file BinStreamBuf.h
///
/// This code borrows heavily from the standard C++ <iostream> library,
/// but is based on binary byte streams rather than arbitrary-length 
/// character (text) streams. This code is bare-bones, doesn't use 
/// templates, but like the C++ library, can be easily expanded to any 
/// byte-wide I/O port.
/// 
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author www.sorosys.com
///
/// @date	December 2003

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//

#ifndef __BinStreamBuf_h
#define __BinStreamBuf_h

#include "xtypes.h"
#include "BinStreamBase.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class BinStreamBuf
{
public:
	typedef BinStreamBase::streamsize streamsize;
	
private:
	byte	*pbegin_,	///< Pointer to begining of @em put buffer
			*pnext_,	///< Pointer to next @em put position
			*pend_,		///< Pointer to one past end of @em put buffer
			*gbegin_,	///< Pointer to begining of @em get buffer
			*gnext_,	///< Pointer to next @em get position
			*gend_;		///< Pointer to one past end of @em get buffer

protected:
	// constructor
	BinStreamBuf();

	// get area
	byte*	eback() const { return gbegin_; }
	byte*	gptr()  const { return gnext_; }
	byte*	egptr() const { return gend_; }

	void	gbump(int n);
	void	setg(byte *gbeg, byte *gnex, byte *gen);

	virtual int			showmanyc();
	virtual streamsize	xsgetn(void* buf, streamsize n);
	virtual int			underflow();
	virtual int			uflow();
	virtual int			pbackfail(int c = -1);

	// put area
	byte*	pbase() const { return pbegin_; }
	byte*	pptr()  const { return pnext_; }
	byte*	epptr() const { return pend_; }

	void	pbump(int n);
	void	setp(byte *pbeg, byte *pen);
	void	setp(byte *pbeg, byte *pnex, byte *pen);

	virtual streamsize	xsputn(const void* buf, streamsize n);
	virtual int			overflow(int c);

	// buffer management
	virtual BinStreamBuf*	setbuf(byte *p, streamsize n);
	virtual int sync();

	// positioning
	virtual long	seekoff(long off, BinStreamBase::seekdir dir,
							BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);
	virtual long	seekpos(long pos, BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);

public:
	// destructor
	virtual ~BinStreamBuf() {}

	// get area
	streamsize	in_avail();
	void		galign(int n);
	int			snextc();
	int			sbumpc();
	int			sgetc();
	streamsize	sgetn(void* buf, streamsize n);

	// putback
	int			sputbackc(byte by);
	int			sungetc();

	// put area
	void		palign(int n);
	int			sputc(byte by);
	streamsize	sputn(const void* buf, streamsize n);

	// buffer management
	int			pubsync();

	// positioning
	long	pubseekoff(long off, BinStreamBase::seekdir dir,
							BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);
	long	pubseekpos(long pos, BinStreamBase::openmode mode = BinStreamBase::in | BinStreamBase::out);

};

// --------------------------------------------------------------------------

inline BinStreamBuf::BinStreamBuf() : pbegin_(0), pnext_(0), pend_(0), 
											gbegin_(0), gnext_(0), gend_(0) {}

inline void BinStreamBuf::gbump(int n)
{
	gnext_ += n;
}

inline void BinStreamBuf::setg(byte *gbeg, byte *gnex, byte *gen)
{
	gbegin_ = gbeg;
	gnext_ = gnex;
	gend_ = gen;
}

inline int BinStreamBuf::showmanyc()
{
	return 0;
}

inline int BinStreamBuf::underflow()
{
	return -1;
}

inline int BinStreamBuf::uflow()
{
	int c = underflow();
	if (c >= 0 && gnext_)
		gnext_++;
	return c;
}

inline int BinStreamBuf::pbackfail(int c /*= -1*/)
{
	return -1;
}

inline BinStreamBuf::streamsize BinStreamBuf::in_avail()
{
	return streamsize(gnext_ ? (gend_-gnext_) : showmanyc());
}

inline int BinStreamBuf::snextc()
{
	return (sbumpc() < 0) ? -1 : sgetc();
}

inline int BinStreamBuf::sbumpc()
{
	return (gnext_ && gnext_ < gend_) ? int(*gnext_++) : uflow();
}

inline int BinStreamBuf::sgetc()
{
	return (gnext_ && gnext_ < gend_) ? int(*gnext_) : underflow();
}

inline BinStreamBuf::streamsize BinStreamBuf::sgetn(void* buf, streamsize n)
{
	return xsgetn(buf, n);
}

inline int BinStreamBuf::sputbackc(byte by)
{
	return (gnext_ && gnext_ > gbegin_ && gnext_[-1] == by) ? int(*(--gnext_)) : pbackfail(by);
}

inline int BinStreamBuf::sungetc()
{
	return (gnext_ && gnext_ > gbegin_) ? int(*(--gnext_)) : pbackfail();
}

inline void BinStreamBuf::pbump(int n)
{
	pnext_ += n;
}

inline void BinStreamBuf::setp(byte *pbeg, byte *pen)
{
	pbegin_ = pnext_ = pbeg;
	pend_ = pen;
}


inline void BinStreamBuf::setp(byte *pbeg, byte *pnex, byte *pen)
{
	pbegin_ = pbeg;
	pnext_ = pnex;
	pend_ = pen;
}


inline int BinStreamBuf::overflow(int c)
{
	return -1;
}

inline BinStreamBuf* BinStreamBuf::setbuf(byte *p, streamsize n)
{
	// overridden classes can handle this
	return this;
}

inline int BinStreamBuf::sync()
{
	return 0;
}

inline long BinStreamBuf::seekoff(long, BinStreamBase::seekdir,
											BinStreamBase::openmode)
{
	return -1L;
}

inline long BinStreamBuf::seekpos(long, BinStreamBase::openmode)
{
	return -1L;
}

inline int BinStreamBuf::sputc(byte by)
{
	return (pnext_ && pnext_ < pend_) ? int(*pnext_++ = by) : overflow(by);
}

inline BinStreamBuf::streamsize BinStreamBuf::sputn(const void* buf, streamsize n)
{
	return xsputn(buf, n);
}

inline int BinStreamBuf::pubsync()
{
	return sync();
}

inline long BinStreamBuf::pubseekoff(long off, BinStreamBase::seekdir dir,
							BinStreamBase::openmode mode)
{
	return seekoff(off, dir, mode);
}

inline long BinStreamBuf::pubseekpos(long pos, BinStreamBase::openmode mode)
{
	return seekpos(pos, mode);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __BinStreamBuf_h

