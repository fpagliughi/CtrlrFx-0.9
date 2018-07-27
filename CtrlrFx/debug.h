/// @file debug.h
/// Various debug macros and helper classes.

#ifndef __CtrlrFx_debug_h
#define __CtrlrFx_debug_h

#if defined(__cplusplus)
	extern "C" {
		namespace CtrlrFx {
#endif

void _debug_init(int nPort);
void _debug_open(const char *s);
int  _dprintf(const char *fmt, ...);
void _debug_close(void);
void _debug_flush(void);

void _assert(int bCond);

#if defined(__cplusplus)
		}
	}
#endif

// ----- Determine if debugging turned on through any macro -----

#if defined(_DEBUG) || defined(CFX_DEBUG_LEVEL) || defined(CFX_DEBUG_STDOUT)
	#undef CFX_DEBUG_OUT
	#define CFX_DEBUG_OUT
#endif

// ----- Some compiler issues -----

#if defined(__GNUC__)
	inline int nop() { return 0; }
	#define NUL_FUNC nop()
	#define FUNCNAME __PRETTY_FUNCTION__
#elif defined(WIN32)
	#define NUL_FUNC ((void)0)
	#define FUNCNAME __FUNCSIG__
#else
	#define NUL_FUNC 0
	#define FUNCNAME "function"
#endif

// ----- Debug levels that trigger various verbose features -----

#if !defined(CFX_DUMP_LEVEL)
	#define CFX_DUMP_LEVEL 4
#endif

#if !defined(CFX_TRACE_LEVEL)
	#define CFX_TRACE_LEVEL 5
#endif


// ----- Primary Debug Macros -----

#if defined(CFX_DEBUG_OUT)
	#if !defined(CFX_DEBUG_LEVEL)
		#define CFX_DEBUG_LEVEL 1
	#endif

	#if defined(CFX_DEBUG_STDOUT)
		#include <stdio.h>
		#define DEBUG_INIT(n)
		#define DEBUG_CLOSE()
		#define DPRINTF_FUNC	printf
		#define DPUTS_FUNC(msg)	fputs(msg, stdout)
		#define DFLUSH_FUNC()	fflush(stdout)
	#else
		// TODO: else define debug macros for local calls
		#include <stdio.h>
		#define DEBUG_INIT(n)
		#define DEBUG_CLOSE()
		#define DPRINTF_FUNC	printf
		#define DPUTS_FUNC(msg)	fputs(msg, stdout)
		#define DFLUSH_FUNC()	fflush(stdout)
	#endif

	#define DPRINTF	 	DPRINTF_FUNC
	#define DPUTS(msg)	DPUTS_FUNC(msg)
	#define DFLUSH()	DFLUSH_FUNC()

	#if CFX_DEBUG_LEVEL >= 2
		#define DPRINTF2	DPRINTF_FUNC
		#define DPUTS2(msg)	DPUTS_FUNC(msg)
	#else
		#define DPRINTF2	(1) ? NUL_FUNC : DPRINTF_FUNC
		#define DPUTS2(msg)
	#endif

	#if CFX_DEBUG_LEVEL >= 3
		#define DPRINTF3	DPRINTF_FUNC
		#define DPUTS3(msg)	DPUTS_FUNC(msg)
	#else
		#define DPRINTF3	(1) ? NUL_FUNC : DPRINTF_FUNC
		#define DPUTS3(msg)
	#endif

	#if CFX_DEBUG_LEVEL >= 4
		#define DPRINTF4	DPRINTF_FUNC
		#define DPUTS4(msg)	DPUTS_FUNC(msg)
	#else
		#define DPRINTF4	(1) ? NUL_FUNC : DPRINTF_FUNC
		#define DPUTS4(msg)
	#endif

	#if CFX_DEBUG_LEVEL >= 5
		#define DPRINTF5	DPRINTF_FUNC
		#define DPUTS5(msg)	DPUTS_FUNC(msg)
	#else
		#define DPRINTF5	(1) ? NUL_FUNC : DPRINTF_FUNC
		#define DPUTS5(msg)
	#endif

#else
	// TODO: We don't need to drag in all of stdio.h for null DPRINTF
	#include <stdio.h>
	#define DEBUG_INIT(n)
	#define DEBUG_CLOSE()
	#define DPRINTF (1) ? NUL_FUNC : printf
	#define DPUTS(msg)
	#define DFLUSH()

	#define DPRINTF2	(1) ? NUL_FUNC : printf
	#define DPUTS2(msg)
	#define DPRINTF3	(1) ? NUL_FUNC : printf
	#define DPUTS3(msg)
	#define DPRINTF4	(1) ? NUL_FUNC : printf
	#define DPUTS4(msg)
	#define DPRINTF5	(1) ? NUL_FUNC : printf
	#define DPUTS5(msg)
#endif


#if defined(CFX_TRACE_LEVEL) && defined(CFX_DEBUG_LEVEL) && \
		(CFX_DEBUG_LEVEL >= CFX_DUMP_LEVEL)
	void _dump_buf(const void *buf, size_t n);
	#define DUMP_BUF _dump_buf
#else
	#define DUMP_BUF(p,n)
#endif


// ----- Trace Class -----

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

#if defined(CFX_TRACE_LEVEL) && (CFX_DEBUG_LEVEL >= CFX_TRACE_LEVEL)
	class DebugTrace {
		const char *msg_;
	public:
		DebugTrace(const char *msg) : msg_(msg) {
			DPRINTF_FUNC("Entering: %s\n", msg_);
		}
		~DebugTrace() {
			DPRINTF_FUNC("Leaving: %s\n", msg_);
		}
	};

	#define DTRACE(msg)		DebugTrace ____dtrace(msg)
	#define DTRACE_FUNC()	DebugTrace _____dtrace(FUNCNAME)
#else
	#define DTRACE(msg)
	#define DTRACE_FUNC()
#endif

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
};


#endif		// __CtrlrFx_debug_h

