/// @file DigIO.h
/// Remote interfaces for Digital I/O.

#ifndef __CtrlrFx_DigIO_h
#define __CtrlrFx_DigIO_h

namespace CtrlrFx {
	
/////////////////////////////////////////////////////////////////////////////
/// A digital input port.
/// Each bit has a high/low (true/false) value that can be queried.

interface DigIn
{
	enum { WIDTH_CMD, VALUE_CMD, N_CMD };

	/// Gets the width (# of bits) in the port
	virtual int width() =0;
	
	/// Gets the current value of the bit (true=high)
	virtual bool value(int bit) =0;
 
	/// Virtual base destructor
	virtual ~DigIn() {}
};

/////////////////////////////////////////////////////////////////////////////
/// A digital output port.
/// Each bit has a high/low (true/false) value that can be set.

interface DigOut
{
	enum { WIDTH_CMD, SET_CMD, CLEAR_CMD, TOGGLE_CMD, N_CMD };

	/// Gets the width (# of bits) in the port
	virtual int width() =0;
	
	/// Sets the output bit high.
	virtual void set(int bit) =0;
	
	/// Sets the output bit to the specified value (true=high).
	virtual void set(int bit, bool b) =0;
	
	/// Sets the output bit low.
	virtual void clear(int bit) =0;
	
	/// Toggles the output bit.
	virtual void toggle(int bit) =0;
	
	/// Virtual base destructor
	virtual ~DigOut() {}
};

/////////////////////////////////////////////////////////////////////////////

interface DigIO : virtual public DigIn, virtual public DigOut
{
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_DigIO_h
