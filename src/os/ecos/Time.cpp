// Time.cpp
// Time classes for eCos.

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Time.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------

void Clock::create(uint32_t dividend, uint32_t divisor)
{
	res_.dividend = dividend;
	res_.divisor  = divisor;

	::cyg_clock_create(res_, &handle_, &clk_);
	res_ = ::cyg_clock_get_resolution(handle_);
}


