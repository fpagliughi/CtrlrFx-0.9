/// @file Win32WaitHandle.h

#ifndef __CtrlrFx_Win32WaitHandle_h
#define __CtrlrFx_Win32WaitHandle_h

#include "CtrlrFx\StdWin32.h"
#include "CtrlrFx\Time.h"
#include <windows.h>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
///	Base class for handle-based sync objects

class Win32WaitHandle
{
	HANDLE	handle_;

public:
	Win32WaitHandle() : handle_(INVALID_HANDLE_VALUE) {}
	Win32WaitHandle(HANDLE h) : handle_(h) {}

	~Win32WaitHandle() { ::CloseHandle(handle_); }

	void	close() { ::CloseHandle(handle_); }
	void	attach(HANDLE h) { handle_ = h; }
	HANDLE	handle() const { return handle_; }

	bool	is_valid() const { return handle_ != INVALID_HANDLE_VALUE; }
	int		error() const { return -1; }

	operator void*() const { return (void*) is_valid(); }
	bool	operator!() const { return !is_valid(); }

	void	wait();
	bool	wait(const Duration& d);
	bool	trywait();

	void	acquire() 					{ wait(); }
	bool	acquire(const Duration& d)	{ return wait(d);  }
	bool	tryacquire()				{ return trywait(); }
};

// --------------------------------------------------------------------------

inline void Win32WaitHandle::wait()
{
	::WaitForSingleObject(handle_, INFINITE);
}

inline bool Win32WaitHandle::wait(const Duration& d)
{
	return ::WaitForSingleObject(handle_, d.to_msec()) == WAIT_OBJECT_0; 
}

inline bool Win32WaitHandle::trywait()
{
	return ::WaitForSingleObject(handle_,  0) == WAIT_OBJECT_0;
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};


#endif		// __CtrlrFx_Win32WaitHandle_h

