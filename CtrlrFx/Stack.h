/// Stack.h
/// A stack (LIFO) class.
///
/// @author	Frank Pagliughi
/// @author SoRo Systems, Inc.
///	@author	www.sorosys.com
///
/// @date   March 2004

//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved

#ifndef __Stack_h
#define __Stack_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
///	This is a simple and efficient stack/LIFO collection class. The buffer
///	used to hold the data items is static in nature. It will not grow or
///	shrink in response to push or pop commands. The buffer can be 
///	automatically allocated off the heap by the object, or the caller
///	can provide it.
///
///	Thus, it doesn't thrash the heap during its operation, and might be
///	preferable to an STL stack in an embedded system.
///	It uses the same interface as the CircQueue (FIFO) class.
///
///	@note Objects are not thread safe

template <typename T> class Stack
{
protected:
	size_t	sz_,		// The current # elements (& index)
			cap_;		// The max # elements

	T		*base_;
	bool	own_;		// Whether we own (should delete) the buffer

	void	alloc_buffer(size_t n);
	void	dealloc();

public:
	Stack();
	explicit Stack(size_t cap);
	Stack(T *buf, size_t cap, bool own=false);

	~Stack();

	void	resize(size_t cap);
	void	set(T* buf, size_t cap, bool own=false);

	size_t	size() const		{ return sz_; }
	size_t	capacity() const	{ return cap_; }

	bool	full() const		{ return sz_ == cap_; }
	bool	empty() const		{ return sz_ == 0; }

	size_t	remaining() const	{ return cap_ - sz_; }
	size_t	available() const	{ return sz_; }

	bool	push(const T& v);
	size_t	push(const T* buf, size_t n);

	T		pop();
	bool	pop(T* p);
	size_t	pop(T* buf, size_t n);
};

// --------------------------------------------------------------------------

template <typename T>
Stack<T>::Stack() : sz_(0), cap_(0), base_(0), own_(false)
{
}

template<typename T>
inline Stack<T>::Stack(size_t cap)
			: sz_(0), cap_(cap), base_(new T[cap]), own_(true)
{
}

template<typename T>
inline Stack<T>::Stack(T* buf, size_t cap, bool own /*=false*/)
						: sz_(0), cap_(cap), base_(buf), own_(false)
{
}

template <typename T>
inline Stack<T>::~Stack()
{
	dealloc();
}

// --------------------------------------------------------------------------
// Frees the buffer and sets the pointer to NULL if we own it.

template <typename T>
void Stack<T>::dealloc()
{
	if (own_) {
		delete[] base_;
		base_ = 0;
		own_ = false;
	}
}

// --------------------------------------------------------------------------
// "Resizes" the stack's buffer by allocating new memory for it. It 
// completely destroys the current contents of the stack.
//

template <typename T>
inline void Stack<T>::resize(size_t cap)
{
	set(new T[cap], cap, true);
}

// --------------------------------------------------------------------------

template <typename T>
void Stack<T>::set(T *buf, size_t cap, bool own /*=false*/)
{
	dealloc();

	sz_ = 0;
	base_ = buf;
	cap_ = cap;
	own_ = own;
}

// --------------------------------------------------------------------------
// Pushes a single element onto the top of the stack.

template<typename T>
bool Stack<T>::push(const T& v)
{
	if (sz_ == cap_)
		return false;

	base_[sz_++] = v;
	return true;
}

// --------------------------------------------------------------------------
// Pushes an array of elements onto the stack. The last element of the array
// will be left at the top of the stack.

template <typename T>
size_t Stack<T>::push(const T* buf, size_t n)
{
	n = min(n, remaining());

	for (size_t i=0; i<n; i++)
		base_[sz_++] = buf[i];

	return n;
}

// --------------------------------------------------------------------------
// Pops the element off the top of the stack and returns it. If the stack is
// empty it returns a zero.
//
template<typename T> 
inline T Stack<T>::pop()
{
	return (sz_ > 0) ? base_[--sz_] : T(0);
}

// --------------------------------------------------------------------------
// Pops the element off the top of the stack and places it into the memory
// that 'p' points to.
//
// RETURNS:
//		true	- if pop successful
//		false	- if the stack is empty and there wqas nothing to pop
//

template <typename T> 
bool Stack<T>::pop(T *p)
{
	if (sz_ == 0)
		return false;

	*p = base_[--sz_];
	return true;
}

// --------------------------------------------------------------------------
// Pops the top 'n' elements off the stack and places them into the array in
// reverse order. When you push an array, then pop it, you get the same array
// back (in the same order).
//

template <typename T> 
size_t Stack<T>::pop(T* buf, size_t n)
{
	size_t nx = n = min(n, available());

	buf += n - 1;

	while (nx--)
		*buf-- = base_[--sz_];

	return n;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __Stack_h

