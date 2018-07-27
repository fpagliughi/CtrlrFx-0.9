// os.h
//
//
//
// CUSTOMER:
//		SoRo Systems, Inc.
//
// PROJECT:
//		
//
// AUTHOR:
//		Frank Pagliughi
//
//		SoRo Systems, Inc.
//		www.sorosys.com
//
// DATE:
//		January 2004
//
// COPYRIGHT NOTICE:
//		Copyright (c) 2004, SoRo Systems, Inc.
//		All Rights Reserved
//
// SOURCE SAFE:
//		$Header:$
//

#ifndef __CtrlrFx_os_h
#define __CtrlrFx_os_h

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Mutex.h"
#include "CtrlrFx/RecursiveMutex.h"
#include "CtrlrFx/OSEvent.h"
#include "CtrlrFx/Semaphore.h"
#include "CtrlrFx/ConditionVar.h"
#include "CtrlrFx/Guard.h"
#include "CtrlrFx/Thread.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
//								App
/////////////////////////////////////////////////////////////////////////////

class App
{
public:
	static int main(int argc, char *argv[]);
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_os_h

