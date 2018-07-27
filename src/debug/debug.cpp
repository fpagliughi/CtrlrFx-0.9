// debug.cpp
// CtrlrFx debug functionality

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/debug.h"

// --------------------------------------------------------------------------

#if defined(CFX_DEBUG_LEVEL) && defined(CFX_DUMP_LEVEL) && \
		(CFX_DEBUG_LEVEL >= CFX_DUMP_LEVEL)

void _dump_buf(const void *buf, size_t n)
{
	const byte* b = reinterpret_cast<const byte*>(buf);

	while (n--) {
		DPRINTF("[x%02X]", (unsigned) *b++);
	}
	DPUTS("\n");
}

#endif

