// StreamTest.cpp
// Low-level test of CtrlrFx binary streams

#include <iostream>
#include <iomanip>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/BinStream.h"
#include "CtrlrFx/MemBinStream.h"
#include "CtrlrFx/BufCodec.h"

using namespace std;
using namespace CtrlrFx;


const int BUF_SIZE = 1024;

void dump(const byte arr[], size_t n)
{
	for (size_t i=0; i<n; i++)
		cout << "[0x" << setfill('0') << setw(2) << hex << unsigned(arr[i]) << "]";
	cout << endl;
}

void dump(ByteBuffer& buf)
{
	dump(buf.c_array(), buf.size());
}

// --------------------------------------------------------------------------

int main()
{
	cout << "Big Endian Binary Streams" << endl;

	byte arr[BUF_SIZE];
	OutMemBinStream os(arr, BUF_SIZE);

	os.align(true);
	os.byte_order(BinStreamBase::BYTE_ORDER_BIG_ENDIAN);

	uint16_t	ou16 = 16;
	byte		oby = 8;
	int32_t		oi32 = 32;


	os << ou16 << oby << oi32;

	dump(arr, 10);

	uint16_t	iu16;
	byte		iby;
	int32_t		ii32;

	InMemBinStream is(arr, BUF_SIZE);
	is.align(true);
	is.byte_order(BinStreamBase::BYTE_ORDER_BIG_ENDIAN);

	is >> iu16 >> iby >> ii32;
	cout << dec << iu16 << '\n' << unsigned(iby) << '\n' << ii32 << endl;

	iu16 = 0;
	iby  = 0;
	ii32 = 0;

	is.reset();
	is >> iu16 >> iby >> ii32;
	cout << '\n' << dec << iu16 << '\n' << unsigned(iby) << '\n' << ii32 << endl;

	// ----- Encoder/Decoder -----

	ByteBuffer			buf(arr, BUF_SIZE);
	BinNativeEncoder	encoder(buf);

	buf.fill(0).rewind();
	encoder.put(ou16);
	encoder.put(oby);
	encoder.put(oi32);

	buf.flip();
	dump(buf);

	iu16 = 0;
	iby  = 0;
	ii32 = 0;

	buf.rewind();
	dump(buf);
	buf.rewind();
	BinNativeDecoder decoder(buf);

	cout << decoder.get(&iu16) << endl;
	cout << decoder.get(&iby)  << endl;
	cout << decoder.get(&ii32) << endl;

	dump(buf);

	cout << '\n' << dec << iu16 << '\n' << unsigned(iby) << '\n' << ii32 << endl;

	return 0;
}
