/// @file FixedBuffer.h
///
/// 


#ifndef __CtrlrFx_FixedBuffer_h
#define __CtrlrFx_FixedBuffer_h

#include "Buffer.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// Encapsultes a buffer object and the underlying memory that it requires.
/// This is a convenient way to use a buffer without allocating dynamic
/// memory off the heap. All memory is local to the block where the buffer
/// is declared.

template <typename T, size_t N>
class FixedBuffer : public Buffer<T>
{
	T arr_[N];

	/// Don't allow re-assignment of underlying memory
	Buffer<T>& resize(size_t cap);
	Buffer<T>& set(T* arr, size_t cap, bool own=false);
	Buffer<T>& assign(T* arr, size_t cap, size_t sz, bool own=false);

public:
	FixedBuffer() : Buffer<T>(arr_, N, false) {}

	FixedBuffer(const Buffer<T>& rhs);
	FixedBuffer(const FixedBuffer& rhs);

	FixedBuffer& operator=(const Buffer<T>& rhs);
	FixedBuffer& operator=(const FixedBuffer& rhs);
};


// --------------------------------------------------------------------------

template <typename T, size_t N>
FixedBuffer<T,N>::FixedBuffer(const Buffer<T>& rhs) : Buffer<T>(arr_, N)
{
	deep_copy(rhs);
}

template <typename T, size_t N>
FixedBuffer<T,N>::FixedBuffer(const FixedBuffer& rhs) : Buffer<T>(arr_, N)
{
	deep_copy(rhs);
}

template <typename T, size_t N>
FixedBuffer<T,N>& FixedBuffer<T,N>::operator=(const Buffer<T>& rhs)
{
	deep_copy(rhs);
	return *this;
}

template <typename T, size_t N>
FixedBuffer<T,N>& FixedBuffer<T,N>::operator=(const FixedBuffer& rhs)
{
	deep_copy(rhs);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_FixedBuffer_h

