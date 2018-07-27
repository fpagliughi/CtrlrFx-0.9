/// @file Sempahore.h
/// CtrlrFx wrapper class for VDK Semaphores
///
/// @note The VDK operating system contains a fairly standard implementation
/// of counting semaphores, but does not have a call for a non-blocking 
/// pend/acquire operation.
///
/// The VDK, does, however contain an implementation of a "periodic" 
/// semaphore. This is one that gets spontaneously posted (signaled) at a 
/// fixed time rate. It's good for makeing periodic threads.
///

#ifndef __CtrlrFx_Semaphore_h
#define __CtrlrFx_Semaphore_h

#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Semaphore
{
	static const int				MAX_CNT = INT_MAX-1;
	static const VDK::SemaphoreID	INVALID_HANDLE = VDK::SemaphoreID(UINT_MAX);

protected:
	static const unsigned DFLT_MAX = UINT_MAX-1;

	VDK::SemaphoreID id_;

public:
	Semaphore();
	explicit Semaphore(int cnt);
	explicit Semaphore(VDK::SemaphoreID semId);
	Semaphore(int cnt, int maxCnt);

	~Semaphore();

	VDK::SemaphoreID& handle() { return id_; }

	void	create(int cnt, int maxCnt=int(DFLT_MAX));
	void	destroy();

	bool	is_valid() const { return id_ != INVALID_HANDLE; }
	int		error() const { return is_valid() ? 0 : -1; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	wait();
	bool	wait(const Duration& d);
	bool	trywait();
	void	post();

	void	acquire()					{ wait(); }
	bool	acquire(const Duration& d)	{ return wait(d); }
	bool	tryacquire()				{ return trywait(); }
	void	release()					{ post(); }
};

// --------------------------------------------------------------------------

inline Semaphore::Semaphore()
			: id_(VDK::CreateSemaphore(0, DFLT_MAX, 0, 0))
{
}

inline Semaphore::Semaphore(int cnt)
			: id_(VDK::CreateSemaphore(cnt, DFLT_MAX, 0, 0))
{
}

inline Semaphore::Semaphore(VDK::SemaphoreID semId) : id_(semId)
{
}

inline Semaphore::Semaphore(int cnt, int maxCnt)
			: id_(VDK::CreateSemaphore(cnt, maxCnt, 0, 0))
{
}

inline Semaphore::~Semaphore()
{
	destroy();
}

inline void Semaphore::create(int cnt, int maxCnt /*=int(DFLT_MAX)*/)
{
	destroy();
	id_ = VDK::CreateSemaphore(cnt, maxCnt, 0, 0);
}

inline void Semaphore::destroy()
{
	// TODO: Critical region here? Is DestroySemaphore safe then?
	if (id_ != INVALID_HANDLE) {
		VDK::DestroySemaphore(id_);
		id_ = INVALID_HANDLE;
	}
}

inline void Semaphore::wait()
{
	VDK::PendSemaphore(id_, 0);
}

inline bool Semaphore::wait(const Duration& d)
{
	tick_t ticks = d.ticks();
	if (ticks == 0)
		++ticks;

	return VDK::PendSemaphore(id_, ticks | VDK::kNoTimeoutError);
}

// Note that the VDK semaphore doesn't have a non-blocking wait,
// so we just do the best we can.

inline bool Semaphore::trywait()
{
	return (VDK::GetSemaphoreValue(id_) != 0)
			? VDK::PendSemaphore(id_, 1 | VDK::kNoTimeoutError)
			: false;
}

inline void Semaphore::post()
{
	VDK::PostSemaphore(id_);
}

/////////////////////////////////////////////////////////////////////////////
// Binary Semaphore

class BinarySemaphore : public Semaphore
{
	/// Hide the semaphore creation
	void create(int cnt, int maxCnt);

public:
	BinarySemaphore();
	BinarySemaphore(bool init);
};

// --------------------------------------------------------------------------

inline BinarySemaphore::BinarySemaphore() : Semaphore(0, 1)
{
}

inline BinarySemaphore::BinarySemaphore(bool init) : Semaphore(init ? 1 : 0, 1)
{
}

/////////////////////////////////////////////////////////////////////////////
/// This is a semaphore that is automatically posted at fixed time 
/// intervals by the operating system.

class PeriodicSemaphore : public Semaphore
{
private:
	tick_t	delay_,
			period_;

	void create(int cnt, int maxCnt=int(DFLT_MAX));

public:
	PeriodicSemaphore();
	explicit PeriodicSemaphore(const Duration& period);
	PeriodicSemaphore(const Duration& delay, const Duration& period);
	PeriodicSemaphore(int cnt, int maxCnt, const Duration& delay, const Duration& period);

	void enable();
	void enable(const Duration& period);
	void enable(const Duration& delay, const Duration& period);
	void disable();
};

// --------------------------------------------------------------------------

inline PeriodicSemaphore::PeriodicSemaphore()
			: Semaphore(VDK::CreateSemaphore(0, DFLT_MAX, 0, 0)), 
					delay_(0), period_(0)
{
}

inline PeriodicSemaphore::PeriodicSemaphore(const Duration& period)
			: Semaphore(VDK::CreateSemaphore(0, DFLT_MAX, period.ticks(), period.ticks())),
					delay_(period.ticks()), period_(period.ticks())
{
}

inline PeriodicSemaphore::PeriodicSemaphore(const Duration& delay, const Duration& period)
			: Semaphore(VDK::CreateSemaphore(0, DFLT_MAX, delay.ticks(), period.ticks())),
					delay_(period.ticks()), period_(period.ticks())
//					delay_(delay), period_(period)
{
}

inline PeriodicSemaphore::PeriodicSemaphore(int cnt, int maxCnt, const Duration& delay, const Duration& period)
			: Semaphore(VDK::CreateSemaphore(cnt, maxCnt, delay.ticks(), period.ticks())),
					delay_(period.ticks()), period_(period.ticks())
//					delay_(delay), period_(period)
{
}


inline void PeriodicSemaphore::enable()
{
	if (delay_ != 0 && period_ != 0)
		VDK::MakePeriodic(id_, delay_, period_);
}

inline void PeriodicSemaphore::enable(const Duration& delay, const Duration& period)
{
	VDK::MakePeriodic(id_, delay_ = delay.ticks(), period_ = period.ticks());
}

inline void PeriodicSemaphore::enable(const Duration& period)
{
	enable(period, period);
}

inline void PeriodicSemaphore::disable()
{
	VDK::RemovePeriodic(id_);
}

/////////////////////////////////////////////////////////////////////////////
// endnamespace CtrlrFx
};

#endif		// __CtrlrFx_Semaphore_h

