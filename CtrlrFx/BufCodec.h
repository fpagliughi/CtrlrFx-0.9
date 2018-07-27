/// @file BufCodec.h
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.

#ifndef __CtrlrFx_BufCodec_h
#define __CtrlrFx_BufCodec_h

#include "Buffer.h"

namespace CtrlrFx {

/// The integer type to use to indicate the size of buffers and arrays.
typedef uint16_t cnt_t;

/////////////////////////////////////////////////////////////////////////////
// Global routines to determine endian-ness.

/// Determines if the machine uses the little-endian multibyte format.
/// @return true if the machine is little endian.

inline bool is_little_endian()
{
	long n = 1L;
	return *(reinterpret_cast<byte*>(&n)) == 1;
}

/// Determines if the machine uses the big-endian multibyte format.
/// @return true if the machine is big endian.

inline bool is_big_endian()
{
	return !is_little_endian();
}

/////////////////////////////////////////////////////////////////////////////
//								BufCodec
/////////////////////////////////////////////////////////////////////////////

/// The base class for binary codecs.
/// This performs underlying buffer and state management.

class BufCodec
{
protected:
	ByteBuffer& buf_;	///< The buffer to which we write.
	bool		good_;	///< Goes bad (false) on a failed insert.

public:
	BufCodec(ByteBuffer& buf) : buf_(buf), good_(true) {}

	/// Determines if the encoder is in a good state
	operator void*() const { return (void*) good_; }

	/// Determines if the encoder is in a failed state
	bool operator!() const { return !good_; }

	/// Gets a reference to the underlying buffer.
	ByteBuffer& buf() { return buf_; }

	size_t size() const { return buf_.size(); }
	bool full() const { return buf_.full(); }
	bool empty() const { return buf_.empty(); }

	/// Sets the buffer's mark to the current position
	BufCodec& mark() { buf_.mark(); return *this; }

	/// Sets the buffer's position to the current mark and restores the good
	/// state of the encoder.
	BufCodec& reset() {
		buf_.reset(); good_ = true; return *this;
	}

	/// Sets the buffer's position & mark to the begining of the buffer and
	// restores the good state of the 
	BufCodec& rewind() {
		buf_.rewind(); good_ = true; return *this; 
	}

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	BufCodec& clear() {
		buf_.clear(); good_ = true; return *this; 
	}

};

/////////////////////////////////////////////////////////////////////////////
/// Buffer Encoder that writes data in the native binary format of the host.

class BinNativeEncoder : public BufCodec
{
	typedef BufCodec Base;

public:
	/// Constructs an encoder and associates it with the specified buffer.
	/// @param buf The byte buffer to which we should write.
	BinNativeEncoder(ByteBuffer& buf) : Base(buf) {}

	/// Sets the buffer's mark to the current position
	BinNativeEncoder& mark() { Base::mark(); return *this; }

	/// Sets the buffer's position to the current mark and restores the good
	/// state of the encoder.
	BinNativeEncoder& reset() { Base::reset(); return *this; }

	/// Sets the buffer's position & mark to the begining of the buffer and
	// restores the good state of the 
	BinNativeEncoder& rewind() { Base::rewind(); return *this; }

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	BinNativeEncoder& clear() { Base::clear(); return *this; }

	// ----- Templatized inserters -----

	template <typename T> int put(const T& v);
	template <typename T> int put(const T arr[], size_t n);

	template <typename T> BinNativeEncoder& operator<<(const T& v);

	// ----- Strict inserters -----

	int put_bool(bool b)		{ return put(byte(b));} 
	int put_char(char c)		{ return put(c); }  
	int put_byte(byte b)		{ return put(b); }
	int put_int16(int16_t n)	{ return put(n); }
	int put_uint16(uint16_t n)	{ return put(n); }
	int put_int32(int32_t n)	{ return put(n); }
	int put_uint32(uint32_t n)	{ return put(n); }

	BinNativeEncoder& operator<<(bool b) { put_bool(b); return *this; }

	#if !defined(CFX_NO_FLOAT)
		int put_float(float x)		{ return put(x); }
		int put_double(double x)	{ return put(x); }
	#endif
};

// --------------------------------------------------------------------------

template <typename T>
int BinNativeEncoder::put(const T& v)
{
	int nx = 0;

	if (good_) {
		buf_.align(sizeof(T));
		nx = buf_.put(reinterpret_cast<const byte*>(&v), sizeof(T));
		good_ = (nx == int(sizeof(T)));
	}
	return nx;
}

template <typename T> 
int BinNativeEncoder::put(const T arr[], size_t n)
{
	int nx = 0;

	if (good_) {
		if (put(cnt_t(n)) != sizeof(cnt_t))
			return 0;

		buf_.align(sizeof(T));
		int nx = buf_.put(arr, n);
		good_ = (nx == int(n));
	}
	return nx;
}

template <typename T> 
inline BinNativeEncoder& BinNativeEncoder::operator<<(const T& v)
{
	put(v);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////

/// Buffer decoder that reads data in the native binary format of the host.

class BinNativeDecoder : public BufCodec
{
	typedef BufCodec Base;

public:
	BinNativeDecoder(ByteBuffer& buf) : Base(buf) {}

	// ----- Buffer Queries -----

	/// Sets the buffer's mark to the current position
	BinNativeDecoder& mark() { Base::mark(); return *this; }

	/// Sets the buffer's position to the current mark and restores the good
	/// state of the encoder.
	BinNativeDecoder& reset() { Base::reset(); return *this; }

	/// Sets the buffer's position & mark to the begining of the buffer and
	// restores the good state of the 
	BinNativeDecoder& rewind() { Base::rewind(); return *this; }

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	BinNativeDecoder& clear() { Base::clear(); return *this; }

	// ----- Extractor templates -----

	template <typename T> int get(T* v);
	template <typename T> int get(T arr[], size_t n);

	template <typename T> BinNativeDecoder& operator>>(T& v);

	// ----- Extractors -----

	int get_bool(bool* c);
	int get_char(char* c)		{ return get(c); }  
	int get_byte(byte* b)		{ return get(b); }
	int get_int16(int16_t* n)	{ return get(n); }
	int get_uint16(uint16_t* n)	{ return get(n); }
	int get_int32(int32_t* n)	{ return get(n); }
	int get_uint32(uint32_t* n)	{ return get(n); }

	BinNativeDecoder& operator>>(bool& b) { get_bool(&b); return *this; }

	#if !defined(CFX_NO_FLOAT)
		int get_float(float* x)		{ return get(x); }
		int get_double(double* x)	{ return get(x); }
	#endif
};

// --------------------------------------------------------------------------

template <typename T>
int BinNativeDecoder::get(T* p)
{
	int nx = 0;

	if (good_) {
		buf_.align(sizeof(T));
		nx = buf_.get(reinterpret_cast<byte*>(p), sizeof(T));
		good_ = (nx == int(sizeof(T)));
	}
	return nx;
}

template <typename T> 
int BinNativeDecoder::get(T arr[], size_t n)
{
	int nx = 0;

	if (good_) {
		cnt_t nc;
		if (get(&nc) != sizeof(cnt_t))
			return 0;

		n = min<size_t>(n, nc);
		// TODO: Read the if n < nc, pop remaining bytes from buf?

		buf_.align(sizeof(T));
		nx = buf_.get(reinterpret_cast<byte*>(arr), n);
		good_ = (nx == int(n));
	}
	return nx;
}

inline int BinNativeDecoder::get_bool(bool* b)
{
	byte by = 0;
	int ret = get(&by);
	*b = (by != 0);
	return ret;
}

template <typename T> 
inline BinNativeDecoder& BinNativeDecoder::operator>>(T& v)
{
	get(&v);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
//						Swap Encoder/Decoder
/////////////////////////////////////////////////////////////////////////////

/// Buffer Encoder that writes data in the swapped binary format of the host.

class BinSwapEncoder : public BufCodec
{
	typedef BufCodec Base;

	template <typename T> int put(const T& v);

protected:
	int put_swap2(int16_t v);
	int put_swap4(int32_t v);
	int put_swap8(int64_t v);

public:
	/// Constructs an encoder and associates it with the specified buffer.
	/// @param buf The byte buffer to which we should write.
	BinSwapEncoder(ByteBuffer& buf) : Base(buf) {}

	// ----- Buffer Queries -----

	/// Sets the buffer's mark to the current position
	BinSwapEncoder& mark() { Base::mark(); return *this; }

	/// Sets the buffer's position to the current mark and restores the good
	/// state of the encoder.
	BinSwapEncoder& reset() { Base::reset(); return *this; }

	/// Sets the buffer's position & mark to the begining of the buffer and
	// restores the good state of the 
	BinSwapEncoder& rewind() { Base::rewind(); return *this; }

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	BinSwapEncoder& clear() { Base::clear(); return *this; }

	template <typename T> int put(const T arr[], size_t n);

	// ----- Strict inserters -----

	int put_bool(bool b)		{ return put(byte(b));} 
	int put_char(char c)		{ return put(c); }  
	int put_byte(byte b)		{ return put(b); }
	int put_int16(int16_t n)	{ return put_swap2(n); }
	int put_uint16(uint16_t n)	{ return put_swap2(n); }
	int put_int32(int32_t n)	{ return put_swap4(n); }
	int put_uint32(uint32_t n)	{ return put_swap4(n); }

	#if !defined(CTRLR_FX_NO_FLOAT)
		int put_float(float x)		{ return put(x); }
		int put_double(double x)	{ return put(x); }
	#endif
};

// --------------------------------------------------------------------------

template <typename T>
int BinSwapEncoder::put(const T& v)
{
	int nx = 0;

	if (good_) {
		buf_.align(sizeof(T));
		nx = buf_.put(reinterpret_cast<const byte*>(&v), sizeof(T));
		good_ = (nx == int(sizeof(T)));
	}
	return nx;
}

template <typename T> 
int BinSwapEncoder::put(const T arr[], size_t n)
{
	int nx = 0;

	if (good_) {
		if (put(cnt_t(n)) != sizeof(cnt_t))
			return 0;

		buf_.align(sizeof(T));
		int nx = buf_.put(arr, n);
		good_ = (nx == int(n));
	}
	return nx;
}

/////////////////////////////////////////////////////////////////////////////

/// Buffer decoder that reads data in the native binary format of the host.

class BinSwapDecoder : public BufCodec
{
	typedef BufCodec Base;

	template <typename T> int get(T* v);

protected:
	int get_swap2(void* p);
	int get_swap4(void* p);
	int get_swap8(void* p);

public:
	BinSwapDecoder(ByteBuffer& buf) : Base(buf) {}

	// ----- Buffer Queries -----

	/// Sets the buffer's mark to the current position
	BinSwapDecoder& mark() { Base::mark(); return *this; }

	/// Sets the buffer's position to the current mark and restores the good
	/// state of the encoder.
	BinSwapDecoder& reset() { Base::reset(); return *this; }

	/// Sets the buffer's position & mark to the begining of the buffer and
	// restores the good state of the 
	BinSwapDecoder& rewind() { Base::rewind(); return *this; }

	/// Sets the position to the start of the buffer and sets the size
	/// to the capacity.
	/// This essentially empties the buffer and places it into write mode.
	BinSwapDecoder& clear() { Base::clear(); return *this; }

	// ----- Extractor templates -----

	template <typename T> int get(T arr[], size_t n);

	// ----- Extractors -----

	int get_bool(bool* c);
	int get_char(char* c)		{ return get(c); }  
	int get_byte(byte* b)		{ return get(b); }
	int get_int16(int16_t* p)	{ return get_swap2(p); }
	int get_uint16(uint16_t* p)	{ return get_swap2(p); }
	int get_int32(int32_t* p)	{ return get_swap4(p); }
	int get_uint32(uint32_t* p)	{ return get_swap4(p); }

	#if !defined(CTRLR_FX_NO_FLOAT)
		int get_float(float* x)		{ return get(x); }
		int get_double(double* x)	{ return get(x); }
	#endif
};

// --------------------------------------------------------------------------

template <typename T>
int BinSwapDecoder::get(T* p)
{
	int nx = 0;

	if (good_) {
		buf_.align(sizeof(T));
		nx = buf_.get(reinterpret_cast<byte*>(p), sizeof(T));
		good_ = (nx == int(sizeof(T)));
	}
	return nx;
}

template <typename T> 
int BinSwapDecoder::get(T arr[], size_t n)
{
	int nx = 0;

	if (good_) {
		cnt_t nc;
		if (get(&nc) != sizeof(cnt_t))
			return 0;

		n = min<size_t>(n, nc);
		// TODO: Read the if n < nc, pop remaining bytes from buf?

		buf_.align(sizeof(T));
		nx = buf_.get(reinterpret_cast<byte*>(arr), n);
		good_ = (nx == int(n));
	}
	return nx;
}

inline int BinSwapDecoder::get_bool(bool* b)
{
	byte by;
	int ret = get(&by);
	*b = (by != 0);
	return ret;
}

/////////////////////////////////////////////////////////////////////////////

/// @todo This is a temporary fix until we have a base class heirarchy

typedef BinNativeEncoder BufEncoder;
typedef BinNativeDecoder BufDecoder;

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_BufCodec_h

