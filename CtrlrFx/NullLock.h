/// @file NullLock.h
/// Definition of an empty locking construct.

#ifndef __CtrlrFx_NullLock_h
#define __CtrlrFx_NullLock_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
/// An empty locking structure that can be used in place of a Mutex.
/// This is used in data structures that are made for multi-threaded 
/// operation, but the user knows that a particular instance is going to be
/// used in a single thread, or in another place where actual locking is
/// not required.

class NullLock
{
public:
	void acquire() {}
	void release() {}
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_NullLock_h

