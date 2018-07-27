// Time.cpp
// Implementation of time wrappers for Posix CtrlrFx
//
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
//

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Time.h"

using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

void normalize_timespec(timespec* tm)
{
	const long NS_PER_SEC = 1000000000L;

	// Adjust for large nsec (positive)

	while (tm->tv_nsec > NS_PER_SEC) {
		++tm->tv_sec;
		tm->tv_nsec -= NS_PER_SEC;
	}

	// Adjust for large nsec (negative)

	while (tm->tv_nsec < -NS_PER_SEC) {
		--tm->tv_sec;
		tm->tv_nsec += NS_PER_SEC;
	}

	// Adjust if sign of sec & nsec different

	if (tm->tv_sec > 0 && tm->tv_nsec < 0) {
		--tm->tv_sec;
		tm->tv_nsec += NS_PER_SEC;
	}
	else if (tm->tv_sec < 0 && tm->tv_nsec > 0) {
		++tm->tv_sec;
		tm->tv_nsec -= NS_PER_SEC;
	}
}

