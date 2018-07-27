// MemBinStream.cpp
// 
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		CtrlrFramework (I/O)
//
// NOTES:
//		In general, memory buffers should be limited to a maximum length of 
//		INT_MAX (not UINT_MAX), since the entire stream IS the local buffer.
//		Stream seeks just move the local get/put pointer. Since pointer 
//		subtraction (difference) results in an int, the max int value is the
//		furthest "bump" possible.
//
//		Platform: DOS or any 16-bit x86
//			In DOS "large" model programs, far pointers are	segment:offset 
//			pairs, but pointer addition & subtraction only operate on the 
//			offset. Subtraction, however, gives a result of the type ptrdiff_t
//			which is a long in the far model! So, if you subract one pouinter
//			from another, then add the result to a third, you get a compiler
//			warning concerning a possible loss of significant digits, since
//			you're trying to add a long to a pointer!  Boy, who thought that
//			one up?
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

#include <limits.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/MemBinStream.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------

long MemBinStreamBuf::seekoff(long off, BinStreamBase::seekdir dir,
									BinStreamBase::openmode mode)
{
	long n = -1L;
	byte *p = 0;

	//assert(off >= INT_MIN && off <= INT_MAX);

	if (mode & BinStreamBase::in) {
		switch (dir) {
			case 0 /*BinStreamBase::beg*/ :
				p = eback() + int(off);
				break;

			case 1 /*BinStreamBase::cur*/ :
				p = gptr() + int(off);
				break;

			case 2 /*BinStreamBase::end*/ :
				p = egptr() + int(off);
				break;
		}
		if (p >= eback() && p <= egptr()) {
			n = long(p - eback());
			gbump(int(p - gptr()));
		}
		else
			n = -1L;
	}

	if (mode & BinStreamBase::out) {
		switch (dir) {
			case 0 /*BinStreamBase::beg*/ :
				p = pbase() + int(off);
				break;

			case 1 /*BinStreamBase::cur*/ :
				p = pptr() + int(off);
				break;

			case 2 /*BinStreamBase::end*/ :
				p = epptr() + int(off);
				break;
		}
		if (p >= pbase() && p <= epptr()) {
			n = long(p - pbase());
			pbump(int(p - pptr()));
		}
		else
			n = -1L;
	}
	return n;
}

// --------------------------------------------------------------------------
// Tries to change the file pointer in the underlying stream buffer.
// For a memory stream, the buffer is the entire stream.

long MemBinStreamBuf::seekpos(long pos, BinStreamBase::openmode mode)
{
	long	n = -1L,
			posMax = (long) unsigned(-1);

	if (posMax < 0)
		posMax = LONG_MAX;

	if (pos >= 0 && pos <= posMax) {
		if (mode & BinStreamBase::in) {
			n = pos;
			gbump(int(pos + (eback() - gptr())));
		}

		if (mode & BinStreamBase::out) {
			n = pos;
			pbump(int(pos + (pbase() - pptr())));
		}
	}
	return n;
}

