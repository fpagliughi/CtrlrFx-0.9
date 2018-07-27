// Device.cpp

#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Device.h"

namespace CtrlrFx {

/////////////////////////////////////////////////////////////////////////////
//						"Global" I/O Routines
/////////////////////////////////////////////////////////////////////////////

// Best effort attempt to read the requested number of bytes.

int read_n(fd_t fd, void *buf, size_t n)
{
	size_t	nr = 0;
	int	nx = 0;

	byte *b = reinterpret_cast<byte*>(buf);

	while (nr < n) {
		if ((nx = ::read(fd, b+nr, n-nr)) <= 0)
			break;
		nr += nx;
	}

	return (nr == 0) ? nx : int(nr);
}

// --------------------------------------------------------------------------
// Best effort attempt to write the requested number of bytes.

int write_n(fd_t fd, const void *buf, size_t n)
{
	size_t	nw = 0;
	int	nx = 0;

	const byte *b = reinterpret_cast<const byte*>(buf);

	while (nw < n) {
		if ((nx = ::write(fd, b+nw, n-nw)) <= 0)
			break;
		nw += nx;
	}

	return (nw == 0) ? nx : int(nw);
}

// --------------------------------------------------------------------------
// Attempts to fill all the available space in the buffer with data from 
// the device. Short reads are possible and accepted.

int read(fd_t fd, ByteBuffer& buf)
{
	int n;
	
	if ((n = ::read(fd, buf.position_ptr(), buf.available())) > 0)
		 buf.incr_position(n);

	return n;
}

// --------------------------------------------------------------------------
// Best effort attempt at reading the whole buffer.

int read_n(fd_t fd, ByteBuffer& buf)
{
	int n;
	
	if ((n = read_n(fd, buf.position_ptr(), buf.available())) > 0)
		 buf.incr_position(n);

	return n;
}

// --------------------------------------------------------------------------
// Attempts to write all of the available data in the buffer to the device.

int write(fd_t fd, ByteBuffer& buf)
{
	int n = ::write(fd, buf.position_ptr(), buf.available());

	if (n > 0)
		 buf.incr_position(n);
		 
	return n;
}

// --------------------------------------------------------------------------
// Best effort attempts to write all of the available data in the buffer 
// to the device.

int write_n(fd_t fd, ByteBuffer& buf)
{
	int n = write_n(fd, buf.position_ptr(), buf.available());

	if (n > 0)
		 buf.incr_position(n);
		 
	return n;
}

/////////////////////////////////////////////////////////////////////////////
//							DeviceBase
/////////////////////////////////////////////////////////////////////////////

// Open the named device and claim ownership of the file descriptor.

int DeviceBase::open(const char *name, int flags)
{
	close();
	
	fd_ = ::open(name, flags);
	own_ = true;
	return fd_;
}

// --------------------------------------------------------------------------
// Close the device, but only if it's already open and we own it.

void DeviceBase::close()
{
	if (fd_ >= 0 && own_) {
		::close(fd_);
		fd_ = -1;
		own_ = false;
	}
}

/////////////////////////////////////////////////////////////////////////////
// End namespace CtrlrFx
}


