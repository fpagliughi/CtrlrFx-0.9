/// @file Event.h
/// Definition of the CtrlrFx Event class.

#ifndef __CtrlrFx_Event_h
#define __CtrlrFx_Event_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// The event signal type
typedef int16_t evt_sig_t;

/// Base for CtrlrFx events.
struct Event
{
	evt_sig_t sig;
};


/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Event_h

