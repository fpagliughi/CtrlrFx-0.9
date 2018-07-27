/// @file Thread.h
/// Thread definitions for the eCos platform.
//
// PLATFORM:
//		eCos
//
// COMPILER:
//		GNU g++ (i386-elf-g++ v. 2.95)
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		CtrlrFramework
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		March 2003
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2003, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#ifndef __CtrlrFx_Thread_h
#define __CtrlrFx_Thread_h

#include "CtrlrFx/OSEvent.h"
#include "CtrlrFx/Time.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
//								Event
/////////////////////////////////////////////////////////////////////////////
/*
class Event
{
protected:
	cyg_flag_t	m_flag;

	static const cyg_flag_mode_t	WAITMODE = CYG_FLAG_WAITMODE_AND
												| CYG_FLAG_WAITMODE_CLR;
	static const cyg_flag_value_t	FLAG = 1;

public:
	Event();
	~Event();

	void	Wait();
	bool	Wait(uint32 ms);
	bool	TryWait();
	void	Signal();
};

// --------------------------------------------------------------------------

inline Event::Event()
{
	::cyg_flag_init(&m_flag);
}

inline Event::~Event()
{
	::cyg_flag_destroy(&m_flag);
}

inline void Event::Wait()
{
	::cyg_flag_wait(&m_flag, FLAG, WAITMODE);
}

inline bool Event::TryWait()
{
	return (bool) ::cyg_flag_poll(&m_flag, FLAG, WAITMODE);
}

inline void Event::Signal()
{
	::cyg_flag_setbits(&m_flag, FLAG);
}

/////////////////////////////////////////////////////////////////////////////
//								Event
/////////////////////////////////////////////////////////////////////////////

class EventGroup
{
protected:
	cyg_flag_t			m_flag;
	cyg_flag_value_t	m_dfltPattern;

	static const cyg_flag_mode_t	ALL_WAITMODE = CYG_FLAG_WAITMODE_AND
													| CYG_FLAG_WAITMODE_CLR,
									ANY_WAITMODE = CYG_FLAG_WAITMODE_OR
													| CYG_FLAG_WAITMODE_CLR;

public:
	EventGroup(int nEvt);
	~EventGroup();

	// ----- AND'd Waits -----

	uint32	WaitForAll();
	uint32	WaitForAll(uint32 pattern);

	uint32	TryWaitForAll();
	uint32	TryWaitForAll(uint32 pattern);

	uint32	TimedWaitForAll(uint32 ms);
	uint32	TimedWaitForAll(uint32 pattern, uint32 ms);

	// ----- OR'd waits -----

	uint32	WaitForAny();
	uint32	WaitForAny(uint32 pattern);

	uint32	TryWaitForAny();
	uint32	TryWaitForAny(uint32 pattern);

	uint32	TimedWaitForAny(uint32 ms);
	uint32	TimedWaitForAny(uint32 pattern, uint32 ms);

	// ----- Signal -----

	void	SignalOne(int nEvt);
	void	Signal(uint32 pattern);
	void	SignalAll();
};

// --------------------------------------------------------------------------

inline EventGroup::EventGroup(int nEvt) : m_dfltPattern(0)
{
	::cyg_flag_init(&m_flag);

	for (int i=0; i<nEvt; i++)
		m_dfltPattern = (m_dfltPattern << 1) | 1;
}

inline EventGroup::~EventGroup()
{
	::cyg_flag_destroy(&m_flag);
}

inline uint32 EventGroup::WaitForAll()
{
	return WaitForAll(m_dfltPattern);
}

inline uint32 EventGroup::WaitForAll(uint32 pattern)
{
	return (uint32) ::cyg_flag_wait(&m_flag, pattern, ALL_WAITMODE);
}

inline uint32 EventGroup::TryWaitForAll()
{
	return TryWaitForAll(m_dfltPattern);
}

inline uint32 EventGroup::TryWaitForAll(uint32 pattern)
{
	return (uint32) ::cyg_flag_poll(&m_flag, pattern, ALL_WAITMODE);
}

inline uint32 EventGroup::TimedWaitForAll(uint32 ms)
{
	return TimedWaitForAll(m_dfltPattern, ms);
}

inline uint32 EventGroup::WaitForAny()
{
	return WaitForAny(m_dfltPattern);
}

inline uint32 EventGroup::WaitForAny(uint32 pattern)
{
	return (uint32) ::cyg_flag_wait(&m_flag, pattern, ANY_WAITMODE);
}

inline uint32 EventGroup::TryWaitForAny()
{
	return TryWaitForAny(m_dfltPattern);
}

inline uint32 EventGroup::TryWaitForAny(uint32 pattern)
{
	return (uint32) ::cyg_flag_poll(&m_flag, pattern, ANY_WAITMODE);
}

inline uint32 EventGroup::TimedWaitForAny(uint32 ms)
{
	return TimedWaitForAny(m_dfltPattern, ms);
}

inline void	EventGroup::Signal(uint32 pattern)
{
	::cyg_flag_setbits(&m_flag, pattern);
}

inline void	EventGroup::SignalAll()
{
	Signal(m_dfltPattern);
}

inline void EventGroup::SignalOne(int nEvt)
{
	Signal(1 << nEvt);
}

/////////////////////////////////////////////////////////////////////////////
// Message Queue

class MsgQueue
{
	cyg_handle_t	m_hBox;
	cyg_mbox		m_box;

public:
	MsgQueue(int nMsg);
	~MsgQueue();

	unsigned	Size() const;
	unsigned	Capacity() const;

	bool		Empty() const;
	bool		Full() const;

	void		Put(void *p);
	bool		Put(void *p, uint32 ms);
	bool		TryPut(void *p);

	void*		Get();
	void*		Get(uint32 ms);
	void*		TryGet();

	void		Release();
};	

// --------------------------------------------------------------------------

inline MsgQueue::MsgQueue(int)
{
	::cyg_mbox_create(&m_hBox, &m_box);
}

inline MsgQueue::~MsgQueue()
{
	::cyg_mbox_delete(m_hBox);
}

inline unsigned MsgQueue::Size() const
{
	return (unsigned) ::cyg_mbox_peek(m_hBox);
}

inline uint32 MsgQueue::Capacity() const
{
	return (unsigned) CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE;
}

inline bool MsgQueue::Empty() const
{
	return ::cyg_mbox_peek(m_hBox) == 0;
}

inline bool MsgQueue::Full() const
{
	return ::cyg_mbox_peek(m_hBox) == CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE;
}

inline void MsgQueue::Put(void *p)
{
	::cyg_mbox_put(m_hBox, p);
}

inline bool MsgQueue::TryPut(void *p)
{
	return (bool) ::cyg_mbox_tryput(m_hBox, p);
}

inline void* MsgQueue::Get()
{
	return ::cyg_mbox_get(m_hBox);
}

inline void* MsgQueue::TryGet()
{
	return ::cyg_mbox_tryget(m_hBox);
}

inline void MsgQueue::Release()
{
	::cyg_mbox_tryput(m_hBox, 0);
}
*/

/////////////////////////////////////////////////////////////////////////////

typedef cyg_handle_t thread_t;

/////////////////////////////////////////////////////////////////////////////
// 								Thread
/////////////////////////////////////////////////////////////////////////////

class Thread
{
public:
	static const int	DFLT_STACK_SIZE			= 2048;

	static const int	PRIORITY_HIGHEST		=  1,
						PRIORITY_ABOVE_NORMAL	=  8,
						PRIORITY_NORMAL			= 16,
						PRIORITY_BELOW_NORMAL	= 24,
						PRIORITY_LOWEST			= 30,
						PRIORITY_IDLE			= 31;

	static const int	PRIORITY_INCREASE		= -1,
						PRIORITY_DECREASE		= +1;


	static const uint32	INFINITE				= uint32(-1),
						WAIT_FOREVER			= INFINITE;

protected:
	cyg_handle_t		handle_;
	cyg_thread			thread_;

	volatile bool		quit_;
	ManualResetEvent	evtDone_;
	char				*stack_;

protected:
	Thread(int prio);
	Thread(int prio, unsigned stackSize);
	Thread(int prio, unsigned stackSize, const char *name);
	virtual ~Thread();

	void create(int prio, unsigned stackSize, const char *name);

	static void startup(CYG_ADDRESS p);

	virtual int run() =0;
	virtual void close() {}

public:
	// ----- Static Functions: Apply to Current Thread -----

	static void		sleep(const Duration& d);

	static bool		curr_priority(int prio);
	static int		curr_priority();

	static thread_t	curr_thread() { return ::cyg_thread_self(); }

	// ----- Public Interface -----

	bool	activate();

	void 	wait() { evtDone_.wait(); }
	bool	wait(const Duration& d) { return evtDone_.wait(d); }

	bool	priority(int prio);
	int		priority() const;

	virtual void quit() { quit_ = true; }
};

// --------------------------------------------------------------------------

inline bool Thread::activate()
{
	::cyg_thread_resume(handle_);
	return true;
}

// --------------------------------------------------------------------------
// Get/Set the thread's  Priority

inline int Thread::priority() const
{
	return (int) ::cyg_thread_get_priority(handle_);
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif	//  __CtrlrFx_Thread_h

