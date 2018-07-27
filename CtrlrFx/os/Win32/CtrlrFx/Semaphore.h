// Semaphore.h
// Semaphore definitions for Win32

#ifndef __CtrlrFx_Semaphore_h
#define __CtrlrFx_Semaphore_h

#if !defined(WIN32)
	#error "This is a Win32 implementation file: requires WIN32 defined"
#endif

#include <climits>

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//								Semaphore
/////////////////////////////////////////////////////////////////////////////

class Semaphore : public Win32WaitHandle
{
public:
	Semaphore();
	Semaphore(int cnt);
	Semaphore(int cnt, int maxCnt);

	void	create(int cnt);
	void	create(int cnt, int maxCnt);

	void	release();
	void	post();
};

// --------------------------------------------------------------------------

inline Semaphore::Semaphore()
{
	attach(::CreateSemaphore(NULL, 0, LONG_MAX, NULL));
}

inline Semaphore::Semaphore(int cnt)
{
	attach(::CreateSemaphore(NULL, cnt, LONG_MAX, NULL));
}

inline Semaphore::Semaphore(int cnt, int maxCnt)
{
	attach(::CreateSemaphore(NULL, cnt, maxCnt, NULL));
}

inline void Semaphore::create(int cnt)
{
	close();
	attach(::CreateSemaphore(NULL, cnt, LONG_MAX, NULL));
}

inline void Semaphore::create(int cnt, int maxCnt)
{
	close();
	attach(::CreateSemaphore(NULL, cnt, maxCnt, NULL));
}

inline void Semaphore::release()
{
	::ReleaseSemaphore(handle(), 1, NULL);
}

inline void Semaphore::post()
{ 
	release();
}

/////////////////////////////////////////////////////////////////////////////
//							Binary Semaphore
/////////////////////////////////////////////////////////////////////////////

class BinarySemaphore : public Semaphore
{
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
// end namespace CtrlrFx
};


#endif 		// __CtrlrFx_Semaphore_h

