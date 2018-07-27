/// @file StateMachine.h
/// Definition of the CtrlrFx @ref StateMachine class.
///
/// @author Frank Pagliughi
/// @date May 2005

#ifndef __CtrlrFx_StateMachine_h
#define __CtrlrFx_StateMachine_h

#include "CtrlrFx/Event.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// The state machine template.

template <typename T, typename EvtType=const Event*>
class StateMachine
{
public:
	/// The super-state return value from handlers.
	typedef void (T::*ReturnState)(EvtType evt);

	/// The state handler method type.
	typedef ReturnState (T::*State)(EvtType evt);

	/// Pre-defined signals.
	enum { SIG_ENTER = -2, SIG_EXIT };

private:
	State state_;	///< The current state handler

protected:
	/// Initializes the state handler.
	/// This sets the initial state handler without attempting to go through
	/// the EXIT/ENTER transition.
	void initialize(State st) { state_ = st; }

	/// Transition to the specified state.
	/// This sends an EXIT event to the current state, sets the state handler
	/// to the new state, then sends the ENTER event to the new state.
	void transition(State st);

public:
	/// Virtual destructor
	virtual ~StateMachine() {}

	/// Dispatch the event to the state machine.
	void dispatch(EvtType evt);
};

// --------------------------------------------------------------------------

template <typename T, typename EvtType>
void StateMachine<T,EvtType>::transition(State st)
{
	dispatch(EvtType(SIG_EXIT));
	state_ = st;
	dispatch(EvtType(SIG_ENTER));
}

template <typename T, typename EvtType>
void StateMachine<T,EvtType>::dispatch(EvtType evt)
{
	T* obj = static_cast<T*>(this);

	State st = state_;
	while (st != State(0))
		st = (State) (obj->*st)(evt);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_StateMachine_h



