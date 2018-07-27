// ConditionVar.cpp
// Win32 implementation of ConditionVars
//

#include "CtrlrFx\CtrlrFx.h"
#include "CtrlrFx\ConditionVar.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------
// Waits on the condition variable until it is signaled or a timeout occurs.

bool ConditionVar::wait(const Duration& d)
{
	HANDLE evt[N_EVT];

	evt[SIGNAL] = signalEvt_.handle();
	evt[BROADCAST] = broadcastEvt_.handle();

	++nthread_;
	mutex_.release();

	int  res = ::WaitForMultipleObjects(N_EVT, evt, FALSE, d.to_msec());
	bool ok = (res != WAIT_TIMEOUT);

	if (ok) {
		--nthread_;

		// If we're the last thread successfully leaving a 
		// broadcast, reset the broadcast event
		if (res == WAIT_OBJECT_0+BROADCAST && nthread_ == 0)
			broadcastEvt_.reset();
	}

	mutex_.acquire();
	return ok;
}


