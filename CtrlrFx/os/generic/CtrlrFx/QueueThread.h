/// @file QueueThread.h
/// 

#ifndef __CtrlrFx_QueueThread_h
#define __CtrlrFx_QueueThread_h

#include "CtrlrFx/Thread.h"
#include "CtrlrFx/MsgQueue.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// A thread with a built-in message queue.

template <typename T>
class QueueThread : public Thread
{
protected:
	MsgQueue<T> que_;

public:
	QueueThread(int prio, size_t queCap);
	QueueThread(int prio, unsigned stackSize, size_t queCap);

	/// Places an item into the queue.
	/// This will block if the queue is full and wait for a slot to open
	/// up to insert the message. It will not return until the message
	/// is delivered to the queue
	/// @param v the item to place in the queue
	void put(const T& v) { que_.put(v); }

	/// Tries to place an item into the queue, and waits a bounded amount
	/// of time if the queue is currently full.
	/// This will attempt to place a message in the queue
	/// @param v The item to place in the queue.
	/// @param d The amount of time to wait.
	/// @return
	/// @li @em true if the item is successfully placed in the queue
	/// @li @em false if the buffer is full and a timeout occurs
	bool put(const T& v, const Duration& d) { return que_.put(v, d); }

	/// Attempts to place an item in the queue.
	/// This will try to place the item in the queue. If the queue is 
	/// currently full, the routine will fail immediately without blocking.
	/// @param v the value to place in the queue
	/// @return
	/// @li @em true if the item is successfully placed in the queue
	/// @li @em false if the buffer is full and the item was not inserted
	bool tryput(const T& v) { return que_.tryput(v); }

	/// Signals the thread to quit.
	/// This should work for most threads that wait on the queue, provided
	/// that they check the quit_ flag after retrieving a message from the
	/// queue.
	virtual void quit() {
		quit_ = true;
		que_.put(T());
	}
};

// --------------------------------------------------------------------------

template <typename T>
QueueThread<T>::QueueThread(int prio, size_t queCap)
					: Thread(prio), que_(queCap)
{
}

template <typename T>
QueueThread<T>::QueueThread(int prio, unsigned stackSize, size_t queCap)
					: Thread(prio, stackSize), que_(queCap)
{
}

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_QueueThread_h

