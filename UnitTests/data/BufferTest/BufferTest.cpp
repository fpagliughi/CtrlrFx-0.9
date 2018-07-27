// BufferTest.cpp
// 
// CppUnit test for the CtrlrFx "Buffer" class
//

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Buffer.h"

using namespace CppUnit;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

class BufferTest : public TestFixture
{
public:
	CPPUNIT_TEST_SUITE( BufferTest );
    CPPUNIT_TEST( test_constructors );
    CPPUNIT_TEST( test_resize );
    CPPUNIT_TEST( test_putget );
    CPPUNIT_TEST( test_arr );
    CPPUNIT_TEST( test_compact );
	CPPUNIT_TEST_SUITE_END();

private:

public:
	void setUp() {
	}

	void tearDown() {
	}

	void test_constructors() {
		Buffer<int> ebuf;

		CPPUNIT_ASSERT(ebuf.position() == 0);
		CPPUNIT_ASSERT(ebuf.size() == 0);
		CPPUNIT_ASSERT(ebuf.capacity() == 0);

		int arr5[5];
		Buffer<int> cbuf(arr5, 5);

		CPPUNIT_ASSERT_EQUAL(0U, cbuf.position());
		CPPUNIT_ASSERT_EQUAL(5U, cbuf.size());
		CPPUNIT_ASSERT_EQUAL(5U, cbuf.capacity());

		int arr10[10];
		Buffer<int> fbuf(arr10, 4U, 10U);

		CPPUNIT_ASSERT_EQUAL(4U,  fbuf.position());
		CPPUNIT_ASSERT_EQUAL(6U,  fbuf.available());
		CPPUNIT_ASSERT_EQUAL(10U, fbuf.size());
		CPPUNIT_ASSERT_EQUAL(10U, fbuf.capacity());
	}

	void test_resize()
	{
		Buffer<char> buf(10);

		buf.put('a');
		CPPUNIT_ASSERT_EQUAL(1U,  buf.position());
		CPPUNIT_ASSERT_EQUAL(10U, buf.capacity());

		buf.resize(20);
		CPPUNIT_ASSERT_EQUAL(0U,  buf.position());
		CPPUNIT_ASSERT_EQUAL(20U, buf.size());
		CPPUNIT_ASSERT_EQUAL(20U, buf.capacity());

		buf.put('a');
		buf.clear(10);
		CPPUNIT_ASSERT_EQUAL(0U,  buf.position());
		CPPUNIT_ASSERT_EQUAL(10U, buf.size());
		CPPUNIT_ASSERT_EQUAL(20U, buf.capacity());

		buf.destroy();
		CPPUNIT_ASSERT_EQUAL(0U, buf.position());
		CPPUNIT_ASSERT_EQUAL(0U, buf.size());
		CPPUNIT_ASSERT_EQUAL(0U, buf.capacity());
	}

	void test_putget() {
		const size_t CAPACITY = 4;
		Buffer<char> buf(CAPACITY);

		buf.clear();
		CPPUNIT_ASSERT_EQUAL(0U,  buf.position());
		CPPUNIT_ASSERT_EQUAL(buf.capacity(), buf.size());

		size_t n = 0;
		char ch = 'a';

		while (!buf.full()) {
			buf.put(ch++);
			CPPUNIT_ASSERT_EQUAL(++n,  buf.position());
		}

		CPPUNIT_ASSERT_EQUAL(buf.capacity(), n);
		CPPUNIT_ASSERT_EQUAL(buf.capacity(), buf.position());

		buf.flip();
		CPPUNIT_ASSERT_EQUAL(0U,  buf.position());
		CPPUNIT_ASSERT_EQUAL(buf.capacity(), buf.available());

		n = 0;
		ch = ' ';

		while (!buf.empty()) {
			CPPUNIT_ASSERT(buf.get(&ch));
			CPPUNIT_ASSERT_EQUAL(char('a' + n), ch);
			n++;
		}
		CPPUNIT_ASSERT_EQUAL(0U, buf.available());

		n = 0;
		ch = ' ';

		buf.rewind();
		CPPUNIT_ASSERT_EQUAL(0U, buf.position());

		while (!buf.empty()) {
			CPPUNIT_ASSERT_EQUAL(char('a' + n), buf.get());
			n++;
		}
	}

	void test_arr()
	{
		const size_t SZ = 3;

		Buffer<long> buf(SZ);
		long	in_arr[SZ],
				out_arr[SZ];

		for (size_t i=0; i<SZ; ++i) {
			in_arr[i] = long((i+1)*2);
			out_arr[i] = 0;
		}

		buf.put(in_arr, SZ);
		CPPUNIT_ASSERT_EQUAL(0U, buf.available());

		buf.flip();
		CPPUNIT_ASSERT_EQUAL(SZ, buf.available());

		buf.get(out_arr, SZ);
		CPPUNIT_ASSERT_EQUAL(0U, buf.available());

		for (size_t i=0; i<SZ; ++i)
			CPPUNIT_ASSERT_EQUAL(long((i+1)*2), out_arr[i]);
	}

	void test_compact()
	{
		Buffer<int> buf(5);


		for (int i=1; i<=5; ++i)
			buf.put(i*2);

		buf.flip();
		buf.get();
		buf.get();

		CPPUNIT_ASSERT_EQUAL(2U, buf.position());
		CPPUNIT_ASSERT_EQUAL(3U, buf.available());

		buf.compact();
		CPPUNIT_ASSERT_EQUAL(0U, buf.position());
		CPPUNIT_ASSERT_EQUAL(3U, buf.available());

		CPPUNIT_ASSERT_EQUAL(6,  buf.get());
		CPPUNIT_ASSERT_EQUAL(8,  buf.get());
		CPPUNIT_ASSERT_EQUAL(10, buf.get());
	}
};


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	CPPUNIT_TEST_SUITE_REGISTRATION( BufferTest );

	TextUi::TestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	runner.addTest(registry.makeTest());
	return (runner.run()) ? 0 : 1;
}

