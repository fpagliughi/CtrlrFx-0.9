// BinStreamBuf.cpp
// This code borrows heavily from the standard C++ <iostream> library,
// but is based on binary byte streams rather than arbitrary-length 
// character (text) streams. This code is bare-bones, doesn't use 
// templates, but like the C++ library, can be easily expanded to any 
// byte-wide I/O port.
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
#include <cstring>
//#include <algorithm>
#include "CtrlrFx/BinStreamBuf.h"

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------
//								Get Area
// --------------------------------------------------------------------------

void BinStreamBuf::galign(int n)
{
	streamsize rem = streamsize(gnext_ - gbegin_) % n;		
	
	if (rem != 0) {
		n = (int) (n - rem);
		while (n--)
			sbumpc();
	}
}

// --------------------------------------------------------------------------

BinStreamBuf::streamsize BinStreamBuf::xsgetn(void* buf, streamsize n)
{
	int 		c;
	streamsize	cnt = n;
	byte* 		bybuf = (byte*) buf;

	while (cnt) {
		if (gnext_ && gnext_ < gend_) {
			streamsize size = min(cnt, streamsize(gend_ - gnext_));
			memcpy(bybuf, gnext_, size);
			cnt -= size;
			bybuf += size;
			gbump(size);
		}
		else if ((c = uflow()) < 0)
			break;
		else {
			*bybuf++ = byte(c);
			cnt--;
		}
	}
	return n - cnt;
}

// --------------------------------------------------------------------------
//								Put Area
// --------------------------------------------------------------------------

void BinStreamBuf::palign(int n)
{
	streamsize rem = streamsize(pnext_ - pbegin_) % n;		
	
	if (rem != 0) {
		n = (int) (n - rem);
		while (n--)
			sputc(0x00);
	}
}

// --------------------------------------------------------------------------

BinStreamBuf::streamsize BinStreamBuf::xsputn(const void *buf, streamsize n)
{
	streamsize	cnt = n;
    byte		*bybuf = (byte*) buf;

	while (cnt) {
		if (pnext_ && pnext_ < pend_) {
			streamsize size = min(cnt, streamsize(pend_ - pnext_));
			memcpy(pnext_, bybuf, size);
			cnt -= size;
			bybuf += size;
			pbump(size);
		}
		else if (overflow(*bybuf) < 0)
			break;
		else {
			cnt--;
			bybuf++;
		}
	}
	return n - cnt;
}


