// Example application demonstrating the use of the CtrlfFx buffers
//

#include <iostream>
#include <string>
#include <io.h>
#include <fcntl.h>
#include "CtrlrFx/xtypes.h"
#include "CtrlrFx/Buffer.h"
#include "CtrlrFx/FixedBuffer.h"

using namespace std;
using namespace CtrlrFx;

// --------------------------------------------------------------------------
// This demonstrates how to read from a file using a buffer.
// The read is done directly to the buffer's memory with no additional
// copying.

int read(int h, Buffer<char>& buf)
{
	int n, nt = 0;

	do {
		n = (int) buf.available();

		if (n > 0) {
			n = read(h, buf.position_ptr(), n);

			if (n > 0) {
				buf.incr_position(n);
				nt += n;
			}
		}
	}
	while (n > 0);

	return nt;
}

// --------------------------------------------------------------------------

int main()
{
	ByteBuffer buf(256);

	buf.fill(0).rewind();
	buf.put((const byte*) "Hello", 5);

	// same thing, but this time manually
	buf.clear();

	char *arr = "Who goes";
	size_t n = strlen(arr);

	for (size_t i=0; i<n; ++i)
		buf[i] = (byte) arr[i];
	buf.position(n);

	// Copy the buffer to another

	ByteBuffer buf2(buf);

	buf.put((const byte*) " there", 8);
	cout << buf.data() << endl;

	buf.flip();

	byte by;
	by = buf.get();
	by = buf.get();
	by = buf.get();
	by = buf.get();
	buf.compact();

	cout << buf.data() << endl;



	FixedBuffer<byte, 256> fbuf;

	fbuf.put((const byte*) "Hello", 6);
	cout << fbuf.data() << endl;

	buf2 = fbuf;
	cout << buf2.data() << endl;

	FixedBuffer<byte, 256> fbuf2(buf2);
	cout << fbuf2.data() << endl;

	// you can't do this
	//fbuf.resize(512);

	// ----- Read in a file -----

	FixedBuffer<char, 512> cbuf;

	int h = open("buf_example.cpp", _O_RDONLY);
	if (h < 0)
		return -1;

	cbuf.put("File:\r\n", 7);

	while (read(h, cbuf) > 0) {
		cbuf.flip();
		cout.write(cbuf.data(), (streamsize) cbuf.size());

		cbuf.clear();
	}
	cout << endl;
	close(h);

	return 0;
}
