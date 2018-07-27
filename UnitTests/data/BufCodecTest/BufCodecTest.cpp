// BufCodecTest.cpp
// 
// CppUnit test for the CtrlrFx Buffer Codec classes
//

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/BufCodec.h"

using namespace CppUnit;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

class BufCodecTest : public TestFixture
{
public:
	CPPUNIT_TEST_SUITE( BufCodecTest );
    CPPUNIT_TEST( test_constructors );
    CPPUNIT_TEST( test_put );
    CPPUNIT_TEST( test_putget );
    CPPUNIT_TEST( test_full );
    CPPUNIT_TEST( test_arr );
	CPPUNIT_TEST( test_insert );
	CPPUNIT_TEST_SUITE_END();

private:
	static const int BUF_SZ = 256;
	ByteBuffer buf_;

public:
	void setUp() {
		buf_.resize(BUF_SZ);
	}

	void tearDown() {
	}

	void test_constructors() {
		// TODO: Add constructor tests?
	}

	void test_put() {
		byte by = '\x1';
		int16_t n16 = 2;
		int32_t n32 = 4;
		float f = 4.0;
		double d = 8.0;

		BinNativeEncoder enc(buf_.clear());

		CPPUNIT_ASSERT_EQUAL(1, enc.put(by));
		CPPUNIT_ASSERT_EQUAL(2, enc.put(n16));
		CPPUNIT_ASSERT_EQUAL(4, enc.put(n32));
		CPPUNIT_ASSERT_EQUAL(4, enc.put(f));
		CPPUNIT_ASSERT_EQUAL(8, enc.put(d));

		// With padding this should be 24 bytes
		CPPUNIT_ASSERT_EQUAL(24U, buf_.position());
	}

	void test_putget() {
		byte by = '\x1';
		int16_t n16 = 2;
		int32_t n32 = 4;
		float f = 4.0F;
		double d = 8.0;

		BinNativeEncoder enc(buf_.clear());

		CPPUNIT_ASSERT_EQUAL(1, enc.put(by));
		CPPUNIT_ASSERT_EQUAL(2, enc.put(n16));
		CPPUNIT_ASSERT_EQUAL(4, enc.put(n32));
		CPPUNIT_ASSERT_EQUAL(4, enc.put(f));
		CPPUNIT_ASSERT_EQUAL(8, enc.put(d));

		// With padding this should be 24 bytes
		CPPUNIT_ASSERT_EQUAL(24U, buf_.position());

		BinNativeDecoder dec(buf_.flip());

		by = '\x0';
		n16 = 0;
		n32 = 0;
		f = 0.0F;
		d = 0.0;

		CPPUNIT_ASSERT_EQUAL(1, dec.get(&by));
		CPPUNIT_ASSERT_EQUAL(2, dec.get(&n16));
		CPPUNIT_ASSERT_EQUAL(4, dec.get(&n32));
		CPPUNIT_ASSERT_EQUAL(4, dec.get(&f));
		CPPUNIT_ASSERT_EQUAL(8, dec.get(&d));

		CPPUNIT_ASSERT_EQUAL('\x1', (char) by);
		CPPUNIT_ASSERT_EQUAL(2, (int) n16);
		CPPUNIT_ASSERT_EQUAL(4, (int) n32);
		CPPUNIT_ASSERT_EQUAL(4.0F, f);
		CPPUNIT_ASSERT_EQUAL(8.0, d);
	}

	// Fill the buffer and then empty it.

	void test_full() {
		int32_t n32 = 42;
		size_t i, n = BUF_SZ/sizeof(int32_t);

		BinNativeEncoder enc(buf_.clear());

		for (i=0; i<n; ++i) {
			CPPUNIT_ASSERT_EQUAL(4, enc.put(n32));
		}
		// This should fail and return zero
		CPPUNIT_ASSERT_EQUAL(0, enc.put(n32));

		BinNativeDecoder dec(buf_.flip());
		i = 0;
		n32 = 0;

		while (dec.get(&n32) == sizeof(int32_t)) {
			CPPUNIT_ASSERT_EQUAL(42, (int) n32);
			++i;
			n32 = 0;
		}

		CPPUNIT_ASSERT_EQUAL(n, i);
	}

	void test_arr() {
		// TODO: Add an array and read it back out
	}

	void test_insert() {
		int32_t n32 = 42;
		size_t i, n = BUF_SZ/sizeof(int32_t);

		BinNativeEncoder enc(buf_.clear());

		i = 0;
		while (enc << n32)
			++i;

		CPPUNIT_ASSERT_EQUAL(n, i);

		BinNativeDecoder dec(buf_.rewind());
		n32 = 0;
		dec >> n32;

		CPPUNIT_ASSERT(!(!dec));
		CPPUNIT_ASSERT_EQUAL(42, n32);
	}
};


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	CPPUNIT_TEST_SUITE_REGISTRATION( BufCodecTest );

	TextUi::TestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	runner.addTest(registry.makeTest());
	return (runner.run()) ? 0 : 1;
}

