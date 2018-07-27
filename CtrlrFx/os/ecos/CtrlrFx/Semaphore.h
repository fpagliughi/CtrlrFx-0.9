// Semaphore.h
// Semaphore definitions for eCos

#ifndef __CtrlrFx_Semaphore_h
#define __CtrlrFx_Semaphore_h

#include <cyg/kernel/sema.hxx>		// Binary Semaphores not exposed in C API
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Semaphore
{
	cyg_sem_t sem_;

public:
	Semaphore();
	Semaphore(unsigned cnt);

	~Semaphore();
	
	void	create(unsigned cnt);

	bool	is_valid() const { return true; }
	int		error() const { return 0; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	wait() 		{ ::cyg_semaphore_wait(&sem_); }
	bool	trywait()	{ return ::cyg_semaphore_trywait(&sem_); }
	void	post()		{ ::cyg_semaphore_post(&sem_); }

	bool	wait(const Duration& d);
	bool	wait_until(const Time& t);

	void	acquire()		{ wait(); }
	bool	tryacquire()	{ return trywait(); }
	void	release()		{ post(); }

	bool	acquire(const Duration& d)	{ return wait(d); }

	int		value();
	bool	available();
};

// --------------------------------------------------------------------------

inline Semaphore::Semaphore()
{
	::cyg_semaphore_init(&sem_, 0);
}

inline Semaphore::Semaphore(unsigned cnt)
{
	::cyg_semaphore_init(&sem_, cyg_ucount32(cnt));
}

inline Semaphore::~Semaphore()
{
	::cyg_semaphore_destroy(&sem_);
}

inline void Semaphore::create(unsigned cnt)
{
	::cyg_semaphore_destroy(&sem_);
	::cyg_semaphore_init(&sem_, cyg_ucount32(cnt));
}

inline int Semaphore::value()
{
	cyg_count32 val;
	::cyg_semaphore_peek(&sem_, &val);
	return int(val);
}

inline bool Semaphore::available()
{
	cyg_count32 val;
	::cyg_semaphore_peek(&sem_, &val);
	return val != 0;
}

/////////////////////////////////////////////////////////////////////////////
//							Binary Semaphore
/////////////////////////////////////////////////////////////////////////////

class BinarySemaphore
{
	Cyg_Binary_Semaphore sem_;

public:
	BinarySemaphore();
	BinarySemaphore(bool init);

	~BinarySemaphore() {}

	bool	is_valid() const { return true; }
	int		error() const { return 0; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	wait() { sem_.wait(); }
	bool	trywait() { return sem_.trywait(); }
	void	post() { sem_.post(); }

	bool	wait(const Duration& d);
	bool	wait_until(const Time& t);

	void	acquire()		{ wait(); }
	bool	tryacquire()	{ return trywait(); }
	void	release()		{ post(); }

	bool	acquire(const Duration& d)	{ return wait(d); }

	unsigned value() { return sem_.posted() ? 1 : 0; }
	bool	available() { return sem_.posted(); }
};

// --------------------------------------------------------------------------

inline BinarySemaphore::BinarySemaphore() : sem_(false)
{
}

inline BinarySemaphore::BinarySemaphore(bool init) : sem_(init)
{
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Semaphore_h

