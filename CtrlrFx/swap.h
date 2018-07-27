/// @file swap.h
/// Funcion templates for byte swapping.
// 

#ifndef __CtrlrFx_swap_h
#define __CtrlrFx_swap_h

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////

/// Swap bytes in place
template <typename T> void swap_bytes(T* pval)
{
	size_t	n = sizeof(T);

	if (n > 1) {
		uint8_t	tmp, *pv = (uint8_t *) pval;

		for (std::size_t i=0; i<n/2; ++i) {
			tmp = pv[i];
			pv[i] = pv[n-1-i];
			pv[n-1-i] = tmp;
		}
	}
}

template <> inline void swap_bytes<uint16_t>(uint16_t* pval)
{
	uint8_t	tmp, *pv = (uint8_t *) pval;
		
	tmp = pv[0];
	pv[0] = pv[1];
	pv[1] = tmp;
}	

template <> inline void swap_bytes<int16_t>(int16_t* pval)
{
	swap_bytes<uint16_t>((uint16_t*) pval);
}

/// Return a swapped variable without touching the original
template <typename T> inline T swap_bytes(T val)
{
	swap_bytes(&val);
	return val;
}

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
};

#endif	// __CtrlrFx_swap_h

