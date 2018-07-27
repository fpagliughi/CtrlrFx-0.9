// StackTest.cpp
// 
// CppUnit test for the CtrlrFx "Stack" class
//

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "CtrlrFx/CtrlrFx.h"
#include "CtrlrFx/Stack.h"

using namespace CppUnit;
using namespace CtrlrFx;

/////////////////////////////////////////////////////////////////////////////

class StackTest : public TestFixture
{
public:
	CPPUNIT_TEST_SUITE( StackTest );
	CPPUNIT_TEST( test_constructors );
	CPPUNIT_TEST( test_resize );
	CPPUNIT_TEST( test_size );
	CPPUNIT_TEST( test_pushpop );
	CPPUNIT_TEST( test_arr );
	CPPUNIT_TEST_SUITE_END();

private:
	Stack<int> st_;


public:
	void setUp() {
	}

	void tearDown() {
	}

	void test_constructors() {
		Stack<int> est;

		CPPUNIT_ASSERT_EQUAL(0U, est.size());
		CPPUNIT_ASSERT_EQUAL(0U, est.capacity());

		Stack<int> dst(4);

		CPPUNIT_ASSERT_EQUAL(0U, dst.size());
		CPPUNIT_ASSERT_EQUAL(4U, dst.capacity());

		int arr5[5];
		Stack<int> cst(arr5, 5);

		CPPUNIT_ASSERT_EQUAL(0U, cst.size());
		CPPUNIT_ASSERT_EQUAL(5U, cst.capacity());
	}

	void test_resize() {
		Stack<int> st(4);
		st.push(9);

		CPPUNIT_ASSERT_EQUAL(1U, st.size());
		CPPUNIT_ASSERT_EQUAL(4U, st.capacity());

		st.resize(10);
		CPPUNIT_ASSERT_EQUAL(0U,  st.size());
		CPPUNIT_ASSERT_EQUAL(10U, st.capacity());

		/*
		st.destroy();
		CPPUNIT_ASSERT_EQUAL(0U, st.size());
		CPPUNIT_ASSERT_EQUAL(0U, st.capacity());
		CPPUNIT_ASSERT(st.empty());
		CPPUNIT_ASSERT(st.full());
		*/
	}

	void test_size() {
		Stack<int> st(4);

		CPPUNIT_ASSERT_EQUAL(0U, st.size());
		CPPUNIT_ASSERT_EQUAL(4U, st.remaining());
		CPPUNIT_ASSERT(st.empty());
		CPPUNIT_ASSERT(!st.full());

		CPPUNIT_ASSERT(st.push(1));
		CPPUNIT_ASSERT_EQUAL(1U, st.size());
		CPPUNIT_ASSERT_EQUAL(3U, st.remaining());
		CPPUNIT_ASSERT(!st.empty());
		CPPUNIT_ASSERT(!st.full());

		CPPUNIT_ASSERT(st.push(2));
		CPPUNIT_ASSERT_EQUAL(2U, st.size());
		CPPUNIT_ASSERT_EQUAL(2U, st.remaining());

		CPPUNIT_ASSERT(st.push(3));
		CPPUNIT_ASSERT_EQUAL(3U, st.size());
		CPPUNIT_ASSERT_EQUAL(1U, st.remaining());

		CPPUNIT_ASSERT(st.push(4));
		CPPUNIT_ASSERT_EQUAL(4U, st.size());
		CPPUNIT_ASSERT_EQUAL(0U, st.remaining());
		CPPUNIT_ASSERT(st.full());

		CPPUNIT_ASSERT(!st.push(5));
		CPPUNIT_ASSERT_EQUAL(4U, st.size());
		CPPUNIT_ASSERT_EQUAL(0U, st.remaining());
		CPPUNIT_ASSERT(st.full());
	}

	void test_pushpop() {
		Stack<char> st(8);

		CPPUNIT_ASSERT(st.push('a'));
		CPPUNIT_ASSERT(st.push('b'));
		CPPUNIT_ASSERT(st.push('c'));

		CPPUNIT_ASSERT_EQUAL('c', st.pop());
		CPPUNIT_ASSERT_EQUAL('b', st.pop());

		char c;
		CPPUNIT_ASSERT(st.pop(&c));
		CPPUNIT_ASSERT_EQUAL('a', c);
	}

	void test_arr() {
		const int ARR_SZ = 3;
		Stack<short> st(2*ARR_SZ+2);

		short 	in_arr[ARR_SZ] = { 5, 7, 9 },
				out_arr[ARR_SZ] = { 0, 0, 0 };

		st.push(in_arr, ARR_SZ);

		CPPUNIT_ASSERT_EQUAL(3U, st.size());
		CPPUNIT_ASSERT_EQUAL(short(9), st.pop());
		CPPUNIT_ASSERT_EQUAL(short(7), st.pop());
		CPPUNIT_ASSERT_EQUAL(short(5), st.pop());

		st.push(in_arr, ARR_SZ);
		CPPUNIT_ASSERT_EQUAL(3U, st.size());

		st.pop(out_arr, ARR_SZ);
		CPPUNIT_ASSERT(st.empty());

		CPPUNIT_ASSERT_EQUAL(short(5), out_arr[0]);
		CPPUNIT_ASSERT_EQUAL(short(7), out_arr[1]);
		CPPUNIT_ASSERT_EQUAL(short(9), out_arr[2]);
	}
};


/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	CPPUNIT_TEST_SUITE_REGISTRATION( StackTest );

	TextUi::TestRunner runner;
	TestFactoryRegistry &registry = TestFactoryRegistry::getRegistry();

	runner.addTest(registry.makeTest());
	return (runner.run()) ? 0 : 1;
}
