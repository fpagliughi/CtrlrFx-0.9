/// @file IDevice.h

#ifndef __CtrlrFx_IDevice_h
#define __CtrlrFx_IDevice_h

#include "CtrlrFx/Buffer.h"
#include "CtrlrFx/Time.h"
#include <stddef.h>

namespace CtrlrFx {
	
/////////////////////////////////////////////////////////////////////////////
// A read-only (input) communications port.

interface IInDevice
{
	/// Reads from the port
	virtual int read(void *buf, size_t n) =0;

	/// Attempts to read the specified number of bytes
	virtual int read_n(void *buf, size_t n) =0;
	
	/// Reads data into a buffer object
	virtual int read(ByteBuffer& buf) =0;

	/// Best-effort attempt to read the whole buffer from the device.
	virtual int read_n(ByteBuffer& buf) =0;
	/**
     * Set a timeout for read operations.
     * Sets the timout that the device uses for read operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool read_timeout(const Duration& d) =0;
	/**
	 * Virtual destructor
	 */
	virtual ~IInDevice() {}
};
	
/////////////////////////////////////////////////////////////////////////////
// A write-only (output) communications port

interface IOutDevice
{
	/// Writes the buffer to the port
	virtual int write(const void *buf, size_t n) =0;

	/// Attempts to write the whole buffer.
	virtual int write_n(const void *buf, size_t n) =0;

	/// Writes the buffer to the port
	virtual int write(ByteBuffer& buf) =0;

	/// Best-effort attempt to write the whole buffer to the device
	virtual int write_n(ByteBuffer& buf) =0;
	/**
     * Set a timeout for write operations.
     * Sets the timout that the device uses for write operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool write_timeout(const Duration& d) =0;
	/**
     * Virtual destructor.
	 */
	virtual ~IOutDevice() {}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * A bi-directional (I/O) communications port.
 */
class IDevice : public virtual IInDevice, public virtual IOutDevice
{
};

/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_IDevice_h

