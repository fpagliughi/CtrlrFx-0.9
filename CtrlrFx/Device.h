/// @file Device.h
/// Classes and functions for devices that perform I/O using POSIX-style
/// unbuffered operations with a file handle, like open(), close(), read(),
/// write(), and lseek().

#ifndef __CtrlrFx_Device_h
#define __CtrlrFx_Device_h

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/FileIO.h"
#include "CtrlrFx/IDevice.h"

namespace CtrlrFx {
	
/////////////////////////////////////////////////////////////////////////////

#ifndef _FD_T_DEFINED
	typedef int fd_t;		///< The OS socket handle
	#define _FD_T_DEFINED
#endif

/////////////////////////////////////////////////////////////////////////////

/**
 * Best effort to read the requested number of bytes from the device.
 * @param fd File handle for the device.
 * @param buf Buffer where the data will go.
 * @param n The number of bytes to read
 * @return The actual number of bytes read. This will always be the
 *         number of bytes requested unless and error or end-of-file is
 *         detected.
 */
int read_n(fd_t fd, void *buf, size_t n);

/**
 * Best effort to write the requested number of bytes to the device.
 * @param fd File handle for the device.
 * @param buf Buffer of data
 * @param n The number of bytes to write.
 * @return The actual number of bytes written. This will always be
 *         the number of bytes requested unless and error is detected.
 */
int write_n(fd_t fd, const void *buf, size_t n);

int read(fd_t fd, ByteBuffer& buf);

int read_n(fd_t fd, ByteBuffer& buf);

int write(fd_t fd, ByteBuffer& buf);

int write_n(fd_t fd, ByteBuffer& buf);

/////////////////////////////////////////////////////////////////////////////
/// The base class for device comm ports.
/// This class manipulates the file descriptor.

class DeviceBase
{
public:
	static const fd_t INVALID_HANDLE = -1;

	// Non-copyable
	DeviceBase(const DeviceBase&);
	DeviceBase& operator=(const DeviceBase&);

protected:
	fd_t	fd_;	///< The OS file descriptor
	bool	own_;	///< Whether we own the file descriptor
	
public:
	/// Creates an unopened device port.
	DeviceBase() : fd_(INVALID_HANDLE) {}
	
	/// Creates a device port from an existing file descriptor.
	/// @param fd Descriptor for an existing device
	explicit DeviceBase(fd_t fd, bool own=false);
	
	/// Creates and opens a device
	DeviceBase(const char *name, int flags);
	
	/// Closes the device
	virtual ~DeviceBase() { close(); }
	
	/// Opens the named device
	int open(const char *name, int flags);
	
	/// Assigns the file descriptor to this device port.
	/// @param fd	the file descriptor
	/// @param own	whether the object should take ownership of the file 
	///				descriptor. If so it will close the device on destruction.
	void assign(fd_t fd, bool own=false);

	/// Claim ownership of the currently-held file descriptor.
	/// The object will then automatically close the descriptor on destruction.
	void claim() { own_ = true; }

	/// Gets the file descriptor handle.
	fd_t handle() const { return fd_; }

	/// Determines if the device is open.
	bool is_open() const { return fd_ >= 0; }
	
	/// Closes the device.
	/// This will only have an effect if the object owns the file descriptor.
	virtual void close();

	/// Repositions the file pointer to the offset relative to the start of the file.
	/// This only works on devices that support random access.
	off_t seek_beg(off_t off) {
		return ::lseek(fd_, off, SEEK_SET);
	}

	/// Repositions the file pointer to the offset relative to the current posiiton.
	/// This only works on devices that support random access.
	off_t seek_cur(off_t off) {
		return ::lseek(fd_, off, SEEK_CUR);
	}

	/// Repositions the file pointer to the offset relative to the end of the file.
	/// This only works on devices that support random access.
	off_t seek_end(off_t off) {
		return ::lseek(fd_, off, SEEK_END);
	}

	/// Repositions the file pointer to the offset relative to the start of the file.
	/// This only works on devices that support random access.
	off_t seek(off_t off) {
		return seek_beg(off);
	}
};

// --------------------------------------------------------------------------

inline DeviceBase::DeviceBase(fd_t fd, bool own /*=false*/)
								: fd_(fd), own_(own)
{
}

inline DeviceBase::DeviceBase(const char *name, int flags) : fd_(INVALID_HANDLE)
{
	open(name, flags);
}

inline void DeviceBase::assign(fd_t fd, bool own /*=false*/)
{
	close();
	fd_ = fd;
	own_ = own;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * An read-only (input) communications port
 */
class InDevice : virtual public DeviceBase,
					virtual public IInDevice
{
public:
	/**
     * Constructs an unconnected input device object.
	 */
	InDevice() {}
	/**
     * Constructs an input device connected to the specified device handle.
     * @param fd The device handle.
     * @param own Whether the object owns the handle and should close it
     *            upon destruction.
	 */
	explicit InDevice(fd_t fd, bool own=false) : DeviceBase(fd, own) {
	}
	/**
     * Constructs and opens the specified input device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
	 */
	InDevice(const char *name, int flags=0) {
		open(name, flags);
	}
	/**
     * Opens the input device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
     * @return 0 on success, <0 on error.
	 */
	int open(const char *name, int flags=0) {
		flags &= ~(O_WRONLY | O_RDWR);
		return DeviceBase::open(name, O_RDONLY | flags);
	}

	/// Reads from the port
	virtual int read(void *buf, size_t n) {
		return ::read(fd_, buf, n);
	}

	/// Attempts to read the specified number of bytes
	virtual int read_n(void *buf, size_t n) {
		return CtrlrFx::read_n(fd_, buf, n);
	}

	/// Attempts to fill the buffer
	virtual int read(ByteBuffer& buf) {
		return CtrlrFx::read(fd_, buf);
	}

	/// Best-effort attempt to read the whole buffer from the device.
	virtual int read_n(ByteBuffer& buf) {
		return CtrlrFx::read_n(fd_, buf);
	}
	/**
     * Set a timeout for read operations.
     * Sets the timout that the device uses for read operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool read_timeout(const Duration& d) {
		return false;
	}
};


/////////////////////////////////////////////////////////////////////////////

/**
 * A write-only (output) communications port.
 */
class OutDevice : virtual public DeviceBase, 
					virtual public IOutDevice
{
public:
	/**
     * Constructs an unconnected output device object.
	 */
	OutDevice() {}
	/**
     * Constructs an output device connected to the specified device handle.
     * @param fd The device handle.
     * @param own Whether the object owns the handle and should close it
     *            upon destruction.
	 */
	explicit OutDevice(fd_t fd, bool own=false) : DeviceBase(fd, own) {
	}
	/**
     * Constructs and opens an output device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
	 */
	OutDevice(const char *name, int flags=0) {
		open(name, flags);
	}
	/**
     * Opens the output device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
     * @return 0 on success, <0 on error.
	 */
	int open(const char *name, int flags=0) {
		flags &= ~(O_RDONLY | O_RDWR);
		return DeviceBase::open(name, O_WRONLY | flags);
	}
 	
	/// Writes the buffer to the socket
	virtual int write(const void *buf, size_t n) {
		return ::write(fd_, buf, n);
	}

	/// Attempts to write the whole buffer.
	virtual int write_n(const void *buf, size_t n) {
		return CtrlrFx::write_n(fd_, buf, n);
	}

	/// Attempts to write the entire available contents of the buffer.
	virtual int write(ByteBuffer& buf) {
		return CtrlrFx::write(fd_, buf);
	}

	/// Best-effort attempt to write the whole buffer to the device
	virtual int write_n(ByteBuffer& buf) {
		return CtrlrFx::write_n(fd_, buf);
	}
	/**
     * Set a timeout for write operations.
     * Sets the timout that the device uses for write operations. Not all
     * devices support timouts, so the caller should prepare for failure.
     * @param d The amount of time to wait for the operation to complete.
     * @return @em true on success, @em false on failure.
	 */
	virtual bool write_timeout(const Duration& d) {
		return false;
	}
};


/////////////////////////////////////////////////////////////////////////////

/**
 * An I/O device
 */
class Device : virtual public IDevice, 
					virtual public InDevice, 
					virtual public OutDevice
{
public:
	/**
     * Constructs an unconnected I/O device.
	 */
	Device() {}
	/**
     * Constructs a device connected to the specified device handle.
     * @param fd The device handle.
     * @param own Whether the object owns the handle and should close it
     *            upon destruction.
	 */
	explicit Device(fd_t fd, bool own=false) : InDevice(fd, own), OutDevice(fd, own) {
	}
	/**
     * Constructs and opens an I/O device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
	 */	
	Device(const char *name, int flags=0) {
		open(name, flags);
	}
	/**
     * Opens the I/O device.
     * @param name The name of the device to open.
     * @param flags Open flags to pass to the os.
     * @return 0 on success, <0 on error.
	 */
	int open(const char *name, int flags=0) {
		flags &= ~(O_RDONLY | O_WRONLY);
		return DeviceBase::open(name, O_RDWR | flags);
	}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper class for the standard input device.
 */
class StdInDevice : public InDevice
{
	static const fd_t STD_IN_FD = 0;
public:
	StdInDevice() { assign(STD_IN_FD); }
	~StdInDevice()	{ fd_ = -1; }
	void close()	{ fd_ = -1; }
};
	
/////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper class for the standard output device
 */
class StdOutDevice : public OutDevice
{
	static const fd_t STD_OUT_FD = 1;

public:
	StdOutDevice() { assign(STD_OUT_FD); }
	~StdOutDevice()	{ fd_ = -1; }
	void close()	{ fd_ = -1; }
};
	
/////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper class for the standard error device.
 */
class StdErrDevice : public OutDevice
{
	static const fd_t STD_ERR_FD = 2;
public:
	StdErrDevice()	{ assign(STD_ERR_FD); }
	~StdErrDevice()	{ fd_ = -1; }
	void close()	{ fd_ = -1; }
};
	
/////////////////////////////////////////////////////////////////////////////
// end namespace CtrlrFx
};

#endif		// __CtrlrFx_Device_h

