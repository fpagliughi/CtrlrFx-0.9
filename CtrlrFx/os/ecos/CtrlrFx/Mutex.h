// Mutex.h
// Mutex definitions for eCos

#ifndef __CtrlrFx_Mutex_h
#define __CtrlrFx_Mutex_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

class Mutex
{
	cyg_mutex_t	handle_;
	bool own_;

public:
	Mutex();
	Mutex(cyg_mutex_t& handle, bool own=false);
	~Mutex();

	cyg_mutex_t& handle() { return handle_; }

	bool	is_valid() const { return true; }
	int		error() const { return 0; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void lock()		{ ::cyg_mutex_lock(&handle_); }
	bool trylock()	{ return ::cyg_mutex_trylock(&handle_); }
	void unlock()	{ ::cyg_mutex_unlock(&handle_); }

	void acquire()		{ lock(); }
	bool tryacquire()	{ return trylock(); }
	void release()		{ unlock(); }
};

// --------------------------------------------------------------------------

inline Mutex::Mutex() : own_(true)
{ 
	::cyg_mutex_init(&handle_);
}

inline Mutex::Mutex(cyg_mutex_t& handle, bool own /*=false*/)
						: handle_(handle), own_(own)
{
}

inline Mutex::~Mutex()
{
	if (own_)
		::cyg_mutex_destroy(&handle_);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Mutex_h

