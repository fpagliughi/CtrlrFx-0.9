/// @file OSEvent.h
/// CtrlrFx wrapper for VDK OS Events.
///
/// @author Frank Pagliughi
/// @author SoRo Systems, Inc.
///
/// @date July 2006
///
/// COPYRIGHT NOTICE:
///		Copyright (c) 2002-2007, SoRo Systems, Inc.
///		All Rights Reserved

#ifndef __CtrlrFx_OSEvent_h
#define __CtrlrFx_OSEvent_h

#include "CtrlrFx/Semaphore.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/**
 * The OSEvent class for the VDK port is empty.
 * There is no commonality for the
 */
class OSEvent
{
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Synchronization object for simple inter-thread signaling that an event
 * occurred, which automatically gets reset when received.
 */
class AutoResetEvent : public OSEvent, public BinarySemaphore
{
public:
	/**
     * Constructor.
	 */
	AutoResetEvent() {}
	/**
     * Signal that the event occurred.
	 */
	void signal() { post(); }
	/**
     * Remove the signal that the thread occurred.
     * This will execute without blocking, whether or not the event is
     * currently signalled.
	 */
	void reset() { trywait(); }
};

/////////////////////////////////////////////////////////////////////////////

/**
 * An event that enables itself periodically.
 * VDK has semaphores that can post themselves periodically. This class
 * simply wraps those objects.
 *
 * @warning
 * This class is specific to the VDK port of @em CtrlrFx.
 */
class PeriodicEvent : public OSEvent
{
public:
	typedef VDK::Ticks tick_t;

private:	
	PeriodicSemaphore sem_;

public:
	//PeriodicEvent() : sem_(0, 1, 0, 0) {}
	explicit PeriodicEvent(const Duration& period) : sem_(0, 1, period, period) {}
	PeriodicEvent(const Duration& delay, const Duration& period)
			: sem_(0, 1, delay, period) {}

	void signal() { sem_.post(); }
	void reset()  { sem_.trywait(); }
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_OSEvent_h

