/// @file MsgSlot.h
/// Definition of the message slot class
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// @date March 2004
///
/// COPYRIGHT:
///		Copyright (c) 2004-2007, SoRo Systems, Inc.
///		All Rights Reserved.
///

#ifndef __CtrlrFx_MsgSlot_h
#define __CtrlrFx_MsgSlot_h

#include "CtrlrFx/os.h"
#include "CtrlrFx/Guard.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// A synchronization object for passing a single message between two threads.
/// This is a synchronization mechanism to pass a single data object between
/// threads. This can be used to rendevous between the two threads.

template <typename T> class MsgSlot
{
	typedef Guard<Mutex> MyGuard;

	mutable Mutex	lock_;			///< The object's lock

	T				val_;			///< The current value in the slot.
	BinarySemaphore	semEmpty_,		///< Signaled if the slot is empty
					semFull_;		///< Signaled if the slot is full

	bool do_put(const T& val);
	bool do_get(T* p);

	// Non-copyable
	MsgSlot(const MsgSlot&);
	MsgSlot& operator=(const MsgSlot&);

public:
	MsgSlot() : semEmpty_(true) {}

	/// Places the message into the slot.
	/// Blocks if the slot is currently occupied.
	/// @param msg The message to pass
	void put(const T& msg);

	/// Attempts to place a message in the slot, with a bounded timeout.
	/// This will block if the slot is full. 
	bool put(const T& val, const Duration& d);

	/// Attempts to place a message in the slot.
	/// This will fail immediately, without blocking, if the slot is full.
	/// @param msg The message to pass
	bool tryput(const T& msg); 

	/// Places the message into the slot, overwriting any existing message.
	/// @param msg The message to pass
	void signal(const T& msg);

	/// Remove and discard any message in the slot.
	void reset();

	/// Gets and returns the item in the slot.
	/// This blocks until an item appears in the slot.
	/// @return The item in the slot.
	T get();

	/// Gets the item from the slot.
	/// This blocks until an item appears in the slot.
	/// @param p A pointer to the memory that will receive the item
	void get(T *p);

	/// Attemps to get the item from the slot, with bounded time.
	/// @param p A pointer to the memory that will receive the item
	/// @param d The time to wait for an item if the slot is empty
	///
	/// @return
	/// @li @em true if the item is successfully retrieved
	/// @li @em false if the slot is empty and a timeout occured
	bool get(T *p, const Duration& d);

	/// Attempts to get the value from the slot without blocking.
	/// @param p Pointer to the 
	bool tryget(T *p);
};

// --------------------------------------------------------------------------

template <typename T>
bool MsgSlot<T>::do_put(const T& val)
{
	MyGuard g(lock_);
	val_ = val;
	g.release();

	semFull_.release();
	return true;
}

template <typename T>
bool MsgSlot<T>::do_get(T *val)
{
	MyGuard g(lock_);
	*val = val_;
	g.release();

	semEmpty_.release();
	return true;
}

// --------------------------------------------------------------------------

template <typename T>
void MsgSlot<T>::put(const T& val)
{
	semEmpty_.acquire();
	do_put(val);
}

template <typename T>
bool MsgSlot<T>::put(const T& val, const Duration& d)
{
	if (!semEmpty_.acquire(d))
		return false;
	return do_put(val);
}

template <typename T>
bool MsgSlot<T>::tryput(const T& val)
{
	if (!semEmpty_.tryacquire())
		return false;
	return do_put(val);
}

template <typename T>
void MsgSlot<T>::signal(const T& val)
{
	semEmpty_.tryacquire();
	do_put(val);
}

template <typename T>
void MsgSlot<T>::reset()
{
	semFull_.tryacquire();
}

template <typename T>
T MsgSlot<T>::get()
{
	semFull_.acquire();

	T val;
	do_get(&val);
	return val;
}

template <typename T>
void MsgSlot<T>::get(T *p)
{
	semFull_.acquire();
	do_get(p);
}

template <typename T>
bool MsgSlot<T>::get(T *p, const Duration& d)
{
	if (!semFull_.acquire(d))
		return false;
	return do_get(p);
}

template <typename T>
bool MsgSlot<T>::tryget(T *p)
{
	if (!semFull_.tryacquire())
		return false;
	return do_get(p);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_MsgSlot_h

