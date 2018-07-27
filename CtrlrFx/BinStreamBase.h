/// @file BinStreamBase.h
///
/// The base class for all input and output streams. Acts like a combination
/// of std::basic_ios and std::ios_base in iostream library. It maintains
/// a pointer to the stream buffer and the stream state.
///
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@authow www.sorosys.com
///
/// @date   December 2003

// PROJECT:
//		CtrlrFramework (I/O)
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved

#ifndef __CtrlrFx_BinStreamBase_h
#define __CtrlrFx_BinStreamBase_h

#include <cstddef>
#include "CtrlrFx/swap.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class BinStreamBuf;

/////////////////////////////////////////////////////////////////////////////

class BinStreamBase
{
	BinStreamBase(const BinStreamBase&);	// Prevent assignment/copy
	BinStreamBase& operator=(const BinStreamBase&);

public:
	enum ByteOrder {
		BYTE_ORDER_LITTLE_ENDIAN,
		BYTE_ORDER_BIG_ENDIAN,
		BYTE_ORDER_NETWORK = BYTE_ORDER_BIG_ENDIAN
	};
	
	typedef signed long streamsize;

	typedef unsigned iostate;
	typedef unsigned openmode;
	typedef unsigned seekdir;

	#if defined(__BORLANDC__)
		static const iostate	goodbit, eofbit, failbit, badbit;
		static const openmode	in, out;
		static const seekdir	beg, cur, end;
	#else
		static const iostate goodbit	= (iostate) 0x00;
		static const iostate eofbit		= (iostate) 0x01;
		static const iostate failbit	= (iostate) 0x02;
		static const iostate badbit		= (iostate) 0x04;

		static const openmode in	 	= (openmode) 0x01;
		static const openmode out		= (openmode) 0x02;

		static const seekdir beg 		= (seekdir) 0x00;
		static const seekdir cur 		= (seekdir) 0x01;
		static const seekdir end 		= (seekdir) 0x02;
	#endif

protected:
	iostate			state_;		///< The current stream state
	BinStreamBuf	*buf_;		///< The stream's buffer
	bool			swap_,		///< Whether words should be byte-swapped
					align_;		///< Whether data should be aligned

	BinStreamBase() : state_(goodbit), buf_(0), swap_(false), align_(false) {}
	void init(BinStreamBuf *buf) { buf_ = buf; }

	// miscelaneous
	BinStreamBuf* rdbuf() const { return buf_; }
	BinStreamBuf* rdbuf(BinStreamBuf *buf) { return buf_ = buf; }

public:
	BinStreamBase(BinStreamBuf *buf)
			: state_(goodbit), buf_(buf), swap_(false), align_(false) {}
	virtual ~BinStreamBase() {}
	
	/// Gets the machine's native byte order
	static ByteOrder host_byte_order();
	
	/// Gets the machine's swapped byte order.
	/// This is the opposite of the native byte order
	static ByteOrder swapped_byte_order();
	
	/// Gets the stream's byte order
	ByteOrder byte_order() const;

	/// Sets the stream's byte order
	void byte_order(ByteOrder order);

	/// Determines if the stream is swapping byte
	bool byte_swap() const { return swap_; }
	
	/// Determines if the stream is aligning words on boundaries
	bool align() const { return align_; }

	/// Sets the alignment for the stream
	void align(bool on) { align_ = on; }

	operator void*() const { return fail() ? 0 : ((void*) this); }
	bool operator!() const { return fail(); }
	iostate rdstate() const { return state_; }
	void clear(iostate st = goodbit) { state_ = st; }
	void setstate(iostate st) { state_ = st; }
	bool good() const { return state_ == goodbit; }
	bool eof() const { return (state_ & eofbit) != 0; }
	bool fail() const { return (state_ & failbit) != 0; }
	bool bad() const { return (state_ & badbit) != 0; }

	virtual void close() {}
	virtual bool reset() { return state_ == goodbit; }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __BinStreamBase_h

