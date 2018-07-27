/// @file Mutex.h
/// Definitions of a mutual exclusion synchronization object
/// for the Analog Devices VisualDSP++ Kernel (vdk)
///
/// @note The VDK port for a mutex is just a semaphore with a max count of
/// one. This differs from a traditional mutex in that there is no "owner"
/// thread - any thread could free a locked mutex.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// @date July 2006
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved

#ifndef __CtrlrFx_Mutex_h
#define __CtrlrFx_Mutex_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Mutex
{
	VDK::SemaphoreID id_;

public:
	Mutex() : id_(VDK::CreateSemaphore(1, 1, 0, 0)) {}
	~Mutex() { VDK::DestroySemaphore(id_); }

	VDK::SemaphoreID& handle() { return id_; }

	bool	is_valid() const { return id_ != UINT_MAX; }
	int		error() const { return is_valid() ? 0 : -1; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	lock() 		{ VDK::PendSemaphore(id_, 0); }
	bool	trylock()	{ return VDK::PendSemaphore(id_, 1 | VDK::kNoTimeoutError); }
	void	unlock()	{ VDK::PostSemaphore(id_); }

	void	acquire()		{ lock(); }
	bool	tryacquire()	{ return trylock(); }
	void	release()		{ unlock(); }
};

/////////////////////////////////////////////////////////////////////////////

typedef Mutex FastMutex;

/////////////////////////////////////////////////////////////////////////////
// endnamespace CtrlrFx
};

#endif		// __CtrlrFx_Mutex_h

