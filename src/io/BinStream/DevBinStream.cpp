// DevBinStream.cpp
//
// Implementation of the binary byte stream (BinStream) and its underlying
// buffer mechanism (BinStreamBuf) for devices using the standard UNIX/POSIX
// unbuffered I/O (open/read/write/close) mechanism.
//
// DevBinStreamBuf provides 
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#include "CtrlrFx/CtrlrFx.h"

#if defined(WIN32)
	#include <io.h>
#else
	#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/stat.h>
#include "CtrlrFx/DevBinStream.h"

using namespace std;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////
//							InDevBinStreamBuf
/////////////////////////////////////////////////////////////////////////////

InDevBinStreamBuf::InDevBinStreamBuf() : m_rd(-1)
{
	setg(0, 0, 0);
}

// --------------------------------------------------------------------------

InDevBinStreamBuf* InDevBinStreamBuf::open(const char *name)
{
	close();

	#if defined(_DOS)
		m_rd = ::open(name, O_RDONLY | O_BINARY);
	#else
		m_rd = ::open(name, O_RDONLY);
	#endif

	return is_open() ? this : NULL;
}

// --------------------------------------------------------------------------

InDevBinStreamBuf* InDevBinStreamBuf::close()
{
	if (m_rd) {
		::close(m_rd);
		m_rd = 0;
	}

	return this;
}

// --------------------------------------------------------------------------
// Refills the buffer from the file device. Before doing so, it copies the
// last few bytes read into the putback area at the begining of the buffer.
//
// RETURNS:
//		on success:		# bytes read
//		on error:		-1
//

int InDevBinStreamBuf::fill_buf()
{
	unsigned nb = (unsigned) min((long) (gptr() - eback()), (long) BACK_BUF_SIZE);

	if (nb > 0)
		memcpy(m_buf + BACK_BUF_SIZE-nb, gptr()-nb, nb);

	int n = ::read(m_rd, m_buf+BACK_BUF_SIZE, BUF_SIZE);

	if (n < 0) {
		setg(0, 0, 0);
		return -1;
	}
	else {
		byte *p = m_buf + BACK_BUF_SIZE;
		setg(p-nb, p, p+n);
	}
	return n;
}

// --------------------------------------------------------------------------
// Reads an array from the stream. It's optimized to read data into the 
// buffer blocks at a time, rather than one byte at a time. If n is smaller
// than the amount of data currently in the buffer then it's all copied w/ 
// a single memcpy() without going to the data source.
//
// RETURNS:
//		the # of bytes read (could be <= n)
//

unsigned InDevBinStreamBuf::xsgetn(byte buf[], unsigned n)
{
	unsigned	nrx = 0,
				nbuf, ncpy;

	while (nrx < n) {
		if ((nbuf = in_avail()) == 0) {
			if (underflow() < 0)
				return nrx;
			continue;
		}

		ncpy = min(n-nrx, nbuf);
		memcpy(buf + nrx, gptr(), ncpy);
		gbump(ncpy);
		nrx += ncpy;
	}

	return n;
}

// --------------------------------------------------------------------------
// This routine is called when the buffer is empty and needs more data. We
// double check that the buffer is empty (curr ptr is at the end) then call
// fill_buf() to read in the data. If it returns an error we get out and
// return -1
//

int InDevBinStreamBuf::underflow()
{
	if (gptr() == egptr() && fill_buf() < 0)
		return -1;

	return *gptr();
}


/////////////////////////////////////////////////////////////////////////////
//							OutDevBinStreamBuf
/////////////////////////////////////////////////////////////////////////////

OutDevBinStreamBuf::OutDevBinStreamBuf() : m_wr(-1)
{
	setp(m_buf, m_buf + BUF_SIZE);
}

// --------------------------------------------------------------------------

OutDevBinStreamBuf* OutDevBinStreamBuf::open(const char *name)
{
	close();

	#if defined(_DOS)
		m_wr = ::open(name, O_WRONLY | O_BINARY);
	#else
		m_wr = ::open(name, O_WRONLY);
	#endif

	return (m_wr < 0) ? 0 : this;
}

// --------------------------------------------------------------------------

OutDevBinStreamBuf* OutDevBinStreamBuf::close()
{
	if (m_wr >= 0) {
		sync();
		::close(m_wr);
		m_wr = -1;
	}

	return this;
}

// --------------------------------------------------------------------------
// Writes an array to the stream. It's optimized to write data a block at a
// time rather than bytes at a time. It will sync with the data source as 
// necessary, but the tail end of the array might be left in the internal 
// buffer.
//
// RETURNS:
//		the # of bytes written (could be <= n)
//

unsigned OutDevBinStreamBuf::xsputn(const byte buf[], unsigned n)
{
	/*
	unsigned nbuf = (unsigned) (epptr() - pptr());

	if (n < nbuf) {
		memcpy(pptr(), buf, n);
		pbump(n);
	}
	else {
		for (unsigned i=0; i<n; i++) {
			if (sputc(buf[i]) < 0)
				return i;
		}
	}

	return n;
	*/

	unsigned	ntx = 0,
				nbuf, ncpy;

	while (ntx < n) {
		if ((nbuf = unsigned(epptr() - pptr())) == 0) {
			if (sync() < 0)
				return ntx;
			continue;
		}

		ncpy = min(n-ntx, nbuf);
		memcpy(pptr(), buf+ntx, ncpy);
		pbump(ncpy);
		ntx += ncpy;
	}

	return n;
}

// --------------------------------------------------------------------------
// This routine is called when the application tried to write a byte to us
// but our buffer was full. We clear the buffer by sending its entire 
// contents to the data sink, then placing the character in the now-empty
// buffer.
//

int OutDevBinStreamBuf::overflow(int c)
{
	if (sync() < 0)
		return -1;
	else if (c < 0)
		return 0;

	return sputc(c);
}

// --------------------------------------------------------------------------
// Synchronizes the buffer with the data store by writing the buffer out.
//
// RETURNS:
//		0	on success
//		-1	on failure
//

int OutDevBinStreamBuf::sync()
{
	int nbuf = (int) (pptr() - pbase()),
		n = write(m_wr, pbase(), nbuf);

	if (n >= 0) {
		pbump(-n);
		return 0;
	}

	return -1;
}

