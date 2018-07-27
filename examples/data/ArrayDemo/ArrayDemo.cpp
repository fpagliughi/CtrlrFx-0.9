// ArrayDemo.cpp
//
// CtrlrFx example application.  This demonstrates the basic use of the 
// CtrlrFx Array class.
//
// The Array class acts much like a c-style array, except that it carries its
// size and max capacity along with it. By setting the size, the application
// can easily communicate how much data the array contains for output functions,
// or how many items an input funciton should fill.

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Array.h"
#include "CtrlrFx/debug.h"

using namespace CtrlrFx;

// --------------------------------------------------------------------------

void fill_array(Array<int>& arr)
{
	size_t sz = arr.size();

	for (size_t i=0; i<sz; ++i)
		arr[i] = (i+1) * sz;
}

// --------------------------------------------------------------------------

void dump_array(const Array<int>& arr)
{
	DPRINTF("%3u items: ", (unsigned) arr.size());

	for (size_t i=0; i<arr.size(); ++i)
		DPRINTF(" %3d", arr[i]);
	DPRINTF("\n");
}

// --------------------------------------------------------------------------

int main()
{
	DPRINTF("ArrayDemo\n");

	Array<int> arr(20);

	arr.size(5);
	fill_array(arr);
	dump_array(arr);

	arr.size(10);
	fill_array(arr);
	dump_array(arr);

	FixedArray<int,20> farr;
	farr.size(5);
	fill_array(farr);
	dump_array(farr);


	return 0;
}

