// RdWrLock.cpp
// Implementation of the generic Reader/Writer lock, created with a 
// condition variable.

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

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/os.h"
#include "CtrlrFx/RdWrLock.h"

namespace CtrlrFx {

// --------------------------------------------------------------------------
// Acquire a read lock. This waits/blocks while a writer has an exclusive
// locks, or a writer is waiting for a lock.

bool RdWrLock::rd_lock()
{
	Guard<ConditionVar> g(cond_);
	while (wrlk_ || wrwt_)
		cond_.wait();
	nrd_++;

	return true;
}

// --------------------------------------------------------------------------

bool RdWrLock::rd_unlock()
{
	Guard<ConditionVar> g(cond_);
	if (nrd_ == 0)
		return false;

	// Broadcast, since there could be writers at the "wait" stage and one
	// at the exclusive lock stage. The latter one must get the message.
	if (--nrd_ == 0)
		cond_.broadcast();

	return true;
}

// --------------------------------------------------------------------------

bool RdWrLock::wr_lock()
{
	Guard<ConditionVar> g(cond_);
	while (wrwt_)
		cond_.wait();

	wrwt_ = true;
	while (wrlk_ || nrd_ > 0)
		cond_.wait();

	wrlk_ = true;

	// Release the wait flag and signal, in case another
	// writer is pending on it.
	wrwt_ = false;
	cond_.signal();

	return true;
}

// --------------------------------------------------------------------------
// Acquire an exclusive write lock.

bool RdWrLock::wr_unlock()
{
	Guard<ConditionVar> g(cond_);
	if (!wrlk_)
		 return false;

	 wrlk_ = false;
	 cond_.broadcast();
	 return true;
}

// --------------------------------------------------------------------------
// Demotes a write lock to a read lock.

bool RdWrLock::wr_demote()
{
	Guard<ConditionVar> g(cond_);
	if (!wrlk_)
		 return false;

	 wrlk_ = false;
	 nrd_++;

	 cond_.broadcast();
	 return true;
}


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

