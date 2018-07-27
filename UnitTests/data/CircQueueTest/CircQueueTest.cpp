// CircQueueTest.cpp
// 
// CppUnit test for the CtrlrFx "CircQueue" class
//

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/CircQueue.h"

using namespace CppUnit;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

class CircQueueTest : public TestFixture
{
public:
	CPPUNIT_TEST_SUITE( CircQueueTest );
	CPPUNIT_TEST( test_constructors );
	CPPUNIT_TEST( test_resize );
	CPPUNIT_TEST( test_size );
	CPPUNIT_TEST( test_putget );
	CPPUNIT_TEST( test_wrap );
	CPPUNIT_TEST( test_arr );
	CPPUNIT_TEST( test_arr_wrap );
	CPPUNIT_TEST_SUITE_END();

private:
	CircQueue<int> que_;


public:
	void setUp() {
	}

	void tearDown() {
	}

	void test_constructors() {
		CircQueue<int> eque;

		CPPUNIT_ASSERT_EQUAL(0U, eque.size());
		CPPUNIT_ASSERT_EQUAL(0U, eque.capacity());

		CircQueue<int> dque(4);

		CPPUNIT_ASSERT_EQUAL(0U, dque.size());
		CPPUNIT_ASSERT_EQUAL(4U, dque.capacity());

		int arr5[5];
		CircQueue<int> cque(arr5, 5);

		CPPUNIT_ASSERT_EQUAL(0U, cque.size());
		CPPUNIT_ASSERT_EQUAL(5U, cque.capacity());
	}

	void test_resize() {
		CircQueue<int> que(4);
		que.put(9);

		CPPUNIT_ASSERT_EQUAL(1U, que.size());
		CPPUNIT_ASSERT_EQUAL(4U, que.capacity());

		que.resize(10);
		CPPUNIT_ASSERT_EQUAL(0U,  que.size());
		CPPUNIT_ASSERT_EQUAL(10U, que.capacity());

		que.destroy();
		CPPUNIT_ASSERT_EQUAL(0U, que.size());
		CPPUNIT_ASSERT_EQUAL(0U, que.capacity());
		CPPUNIT_ASSERT(que.empty());
		CPPUNIT_ASSERT(que.full());
	}

	void test_size() {
		CircQueue<int> que(4);

		CPPUNIT_ASSERT_EQUAL(0U, que.size());
		CPPUNIT_ASSERT_EQUAL(4U, que.remaining());
		CPPUNIT_ASSERT(que.empty());
		CPPUNIT_ASSERT(!que.full());

		CPPUNIT_ASSERT(que.put(1));
		CPPUNIT_ASSERT_EQUAL(1U, que.size());
		CPPUNIT_ASSERT_EQUAL(3U, que.remaining());
		CPPUNIT_ASSERT(!que.empty());
		CPPUNIT_ASSERT(!que.full());

		CPPUNIT_ASSERT(que.put(2));
		CPPUNIT_ASSERT_EQUAL(2U, que.size());
		CPPUNIT_ASSERT_EQUAL(2U, que.remaining());

		CPPUNIT_ASSERT(que.put(3));
		CPPUNIT_ASSERT_EQUAL(3U, que.size());
		CPPUNIT_ASSERT_EQUAL(1U, que.remaining());

		CPPUNIT_ASSERT(que.put(4));
		CPPUNIT_ASSERT_EQUAL(4U, que.size());
		CPPUNIT_ASSERT_EQUAL(0U, que.remaining());
		CPPUNIT_ASSERT(que.full());

		CPPUNIT_ASSERT(!que.put(5));
		CPPUNIT_ASSERT_EQUAL(4U, que.size());
		CPPUNIT_ASSERT_EQUAL(0U, que.remaining());
		CPPUNIT_ASSERT(que.full());
	}

	void test_putget() {
		CircQueue<char> que(8);

		CPPUNIT_ASSERT(que.put('a'));
		CPPUNIT_ASSERT(que.put('b'));
		CPPUNIT_ASSERT(que.put('c'));

		CPPUNIT_ASSERT_EQUAL('a', que.get());
		CPPUNIT_ASSERT_EQUAL('b', que.get());

		char c;
		CPPUNIT_ASSERT(que.get(&c));
		CPPUNIT_ASSERT_EQUAL('c', c);
	}

	void test_wrap() {
		CircQueue<char> que(4);

		que.put('a');
		que.put('b');
		que.get();
		que.get();

		que.put('c');
		que.put('d');
		que.put('e');

		CPPUNIT_ASSERT_EQUAL(3U, que.size());
		CPPUNIT_ASSERT_EQUAL('c', que.get());
		CPPUNIT_ASSERT_EQUAL('d', que.get());
		CPPUNIT_ASSERT_EQUAL('e', que.get());
	}

	void test_arr() {
		const int ARR_SZ = 3;
		CircQueue<short> que(2*ARR_SZ+2);

		short 	in_arr[ARR_SZ] = { 5, 7, 9 },
				out_arr[ARR_SZ] = { 0, 0, 0 };

		que.put(in_arr, ARR_SZ);

		CPPUNIT_ASSERT_EQUAL(3U, que.size());
		CPPUNIT_ASSERT_EQUAL(short(5), que.get());
		CPPUNIT_ASSERT_EQUAL(short(7), que.get());
		CPPUNIT_ASSERT_EQUAL(short(9), que.get());

		que.put(in_arr, ARR_SZ);
		CPPUNIT_ASSERT_EQUAL(3U, que.size());

		que.get(out_arr, ARR_SZ);
		CPPUNIT_ASSERT(que.empty());

		CPPUNIT_ASSERT_EQUAL(short(5), out_arr[0]);
		CPPUNIT_ASSERT_EQUAL(short(7), out_arr[1]);
		CPPUNIT_ASSERT_EQUAL(short(9), out_arr[2]);
	}

	void test_arr_wrap() {
		const int ARR_SZ = 3;
		CircQueue<char> que(4);

		que.put('a');
		que.put('b');
		que.get();
		que.get();

		char	in_arr[ARR_SZ] = { 'c', 'd', 'e' },
				out_arr[ARR_SZ] = { ' ', ' ', ' ' };

		que.put(in_arr, ARR_SZ);

		CPPUNIT_ASSERT_EQUAL(3U, que.size());
		CPPUNIT_ASSERT_EQUAL('c', que.get());
		CPPUNIT_ASSERT_EQUAL('d', que.get());
		CPPUNIT_ASSERT_EQUAL('e', que.get());

		que.put('f');
		que.get();

		que.put(in_arr, ARR_SZ);
		CPPUNIT_ASSERT_EQUAL(3U, que.size());

		que.get(out_arr, ARR_SZ);
		CPPUNIT_ASSERT(que.empty());

		CPPUNIT_ASSERT_EQUAL('c', out_arr[0]);
		CPPUNIT_ASSERT_EQUAL('d', out_arr[1]);
		CPPUNIT_ASSERT_EQUAL('e', out_arr[2]);
	}
};


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	CPPUNIT_TEST_SUITE_REGISTRATION( CircQueueTest );

	TextUi::TestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	runner.addTest(registry.makeTest());
	return (runner.run()) ? 0 : 1;
}

