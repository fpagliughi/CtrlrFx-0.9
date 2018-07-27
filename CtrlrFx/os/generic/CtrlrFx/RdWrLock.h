/// @file RdWrLock.h
/// Implementation of Reader/Writer locks.
///
/// @author Frank Pagliughi
/// @date February 2005

// --------------------------------------------------------------------------
// This file is part of CtrlrFx, The Controller Framework.
// Copyright (C) 2001-2005 Frank Pagliughi
// Copyright (C) 2005-2007 SoRo Systems, Inc
//
// CtrlrFx is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later
// version.
//
// CtrlrFx is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with CtrlrFx; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// --------------------------------------------------------------------------

#ifndef __CtrlrFx_RdWrLock_h
#define __CtrlrFx_RdWrLock_h

#include "CtrlrFx/ConditionVar.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/**
 * Class for Reader/Writer locks.
 */
class RdWrLock
{
	bool			wrlk_,	///< If a write lock is present
					wrwt_;	///< If a writer is waiting for a lock
	int				nrd_;	///< The number of current read locks
	ConditionVar	cond_;	///< Gets signaled when rd or wr lock is freed

	// Non-copyable
	RdWrLock(const RdWrLock&);
	RdWrLock& operator=(const RdWrLock&);

public:
	RdWrLock() : wrlk_(false), wrwt_(false), nrd_(0) {}

	/// Acquire a read lock.
	/// This will block until any write locks are released.
	bool rd_lock();

	/// Release a read lock.
	bool rd_unlock();

	/// Acquire a write lock.
	/// This will block until any other write lock and all read locks are
	/// released.
	bool wr_lock();

	/// Release a write lock.
	bool wr_unlock();

	// Demotes a write lock to a read lock.
	// This should be used with some care, since the request will be granted
	// even if another writer is waiting. That other, pending, writer will
	// continue to lock out any other reader, and the calling thread would 
	// effectively be granted exclusive read access to the data, which may
	// not be the desired effect.
	bool wr_demote();
};

/////////////////////////////////////////////////////////////////////////////
/// Guard for a read lock on a Reader/Writer lock.

class RdGuard
{
	RdWrLock&	lock_;	///< The lockable object we're guarding
	bool 		own_;	///< Whether we currently own the lock

public:
	/// Constructs the guard and acquires the lock.
	RdGuard(RdWrLock& lock) : lock_(lock), own_(true) {
		lock_.rd_lock();
	}

	/// Constructs the guard and acquires the lock, if not already owned
	RdGuard(RdWrLock& lock, bool own) : lock_(lock), own_(own) {
		acquire();
	}

	/// Destroys the guard and releases the lock, if held.
	~RdGuard() {
		if (own_)
			lock_.rd_unlock();
	}

	/// Acquires the lock.
	/// This has no effect of the lock is already held.
	void acquire() {
		if (!own_) {
			lock_.rd_lock();
			own_ = true;
		}
	}

	/// Releases the lock.
	/// This has no effect of the lock is not currently held.
	void release() {
		if (own_) {
			lock_.rd_unlock();
			own_ = false;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
/// Guard for a write lock on a Reader/Writer lock.

class WrGuard
{
	RdWrLock&	lock_;	///< The lockable object we're guarding
	bool 		own_;	///< Whether we currently own the lock

public:
	/// Constructs the guard and acquires the lock.
	WrGuard(RdWrLock& lock) : lock_(lock), own_(true) {
		lock_.wr_lock();
	}

	/// Constructs the guard and acquires the lock, if not already owned
	WrGuard(RdWrLock& lock, bool own) : lock_(lock), own_(own) {
		acquire();
	}

	/// Destroys the guard and releases the lock, if held.
	~WrGuard() {
		if (own_)
			lock_.wr_unlock();
	}

	/// Acquires the lock.
	/// This has no effect of the lock is already held.
	void acquire() {
		if (!own_) {
			lock_.wr_lock();
			own_ = true;
		}
	}

	/// Releases the lock.
	/// This has no effect of the lock is not currently held.
	void release() {
		if (own_) {
			lock_.wr_unlock();
			own_ = false;
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_RdWrLock_h


