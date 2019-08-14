

#include "Pointer.h"
#include "UnitTest.h"
#include "Basic.h"

#include <iostream>
#include <memory>

namespace {
	class TestObject {
		std::string data_;
		static int count_;
	public:
		typedef int I;

		TestObject() { ++count_; }
		TestObject(const std::string& data) : data_(data) {
			++count_;
		}
		virtual ~TestObject() {
			--count_;
		}
		const std::string& data() { return data_; }
		static int count() { return count_; }
	};
	int TestObject::count_ = 0;

	class Derived : public TestObject {
		int number_;
	public:
		Derived(const std::string& data, int i) : TestObject(data), number_(i) {}
		int number() { return number_; }
	};
}

using yep::Ptr;
using yep::ReleaseArrayPtr;
using yep::Error;

void Del(TestObject* p) {
	delete[] p;
	p = nullptr;
}

void test_std_shared_ptr()
{
	TestObject::I iii = 0;
	void* p = nullptr;
	delete p;
	std::shared_ptr<TestObject> p1;
	int c = p1.use_count();
	p1 = std::make_shared<TestObject>("ppp");
	c = p1.use_count();
	p1.reset();
	c = p1.use_count();
	{
		std::shared_ptr<TestObject> p = std::make_shared<TestObject>("ttt");
		int count = p.use_count();
		p = 0;
		count = p.use_count();
	}
	{
		// error
		//std::shared_ptr<TestObject> ptr1 = std::make_shared<Derived>("test", 666);
		//std::shared_ptr<Derived> ptr2 = static_cast<Derived*>(ptr1);

		TestObject* parray = new TestObject[10];
		std::shared_ptr<TestObject> sp (parray, Del);
	}

	int a = 10;
}

TEST_CASE(PointerBase) {
	Ptr<TestObject> ptr1;
	TEST_ASSERT_NULL(ptr1.ptr());

	TestObject* pto1 = new TestObject("one");
	TestObject* pto2 = new TestObject("two");
	if (pto2 < pto1) {
		TestObject* temp = pto1;
		pto1 = pto2;
		pto2 = temp;
	}
	TEST_ASSERT(pto1 < pto2);

	ptr1 = pto1;
	TEST_ASSERT(ptr1.refcount() == 1);
	Ptr<TestObject> ptr2 = pto2;
	Ptr<TestObject> ptr3 = ptr1;
	TEST_ASSERT(ptr1.refcount() == 2);

	TEST_ASSERT(ptr1.ptr() == pto1);
	TEST_ASSERT(ptr1 == pto1);

	TEST_ASSERT(ptr1 == pto1);
	TEST_ASSERT(ptr1 != pto2);
	TEST_ASSERT(ptr1 < pto2);
	TEST_ASSERT(ptr1 <= pto2);
	TEST_ASSERT(ptr2 > pto1);
	TEST_ASSERT(ptr2 >= pto1);

	ptr1.swap(ptr2);
	TEST_ASSERT(ptr2 < ptr1);
	ptr2.swap(ptr1);

	TEST_ASSERT((ptr1->data() == "one" && ptr2->data() == "two") || (ptr1->data() == "two" && ptr2->data() == "one"));

	TEST_ASSERT(TestObject::count() == 2);
	void * ppp = nullptr;
	delete ppp;

	ptr1 = 0;
	ptr2 = 0;
	ptr3 = 0;

	TEST_ASSERT(TestObject::count() == 0);

	{
		Ptr<TestObject> ptr = new TestObject("");
		TEST_ASSERT(TestObject::count() == 1);
	}
	TEST_ASSERT(TestObject::count() == 0);
}

TEST_CASE(PointerCast) {
	{
		Ptr<Derived> p1 = new Derived("test", 100);
		TEST_ASSERT(TestObject::count() == 1);
		Ptr<TestObject> p2 = p1;
		TEST_ASSERT(TestObject::count() == 1);
	}

	{
		TEST_ASSERT(TestObject::count() == 0);

		Ptr<Derived> p1(new Derived("test", 666));
		Ptr<TestObject> p2 = p1;
		TEST_ASSERT(TestObject::count() == 1);
		p1 = 0;
		TEST_ASSERT(TestObject::count() == 1);
	}

	{
		Ptr<TestObject> p1 = new Derived("test", 666);
		p1 = 0;
		TEST_ASSERT(TestObject::count() == 0);
	}

	{
		Ptr<TestObject> ptr1 = new Derived("test", 666);
		Ptr<Derived> ptr2 = ptr1.Cast<Derived>();
		TEST_ASSERT(ptr2.ptr() != 0);
		TEST_ASSERT(666 == ptr2->number());
		
		ptr2 = 0;
		int count1 = ptr1.refcount();
		TEST_ASSERT( 0 == ptr2.refcount());
	}

	TEST_ASSERT(TestObject::count() == 0);
	
	{
		TestObject* parray = new TestObject[10];
		Ptr<TestObject, ReleaseArrayPtr<TestObject>> sp(parray);
	}
	TEST_ASSERT(1 == 1);
}

TEST_CASE(PointerReset)
{
	{
		Ptr<TestObject> ptr1;
		TEST_ASSERT_NULL(ptr1.ptr());

		ptr1 = new TestObject("one");
		TEST_ASSERT(ptr1.refcount() == 1);
		ptr1.reset(0);
		TEST_ASSERT(ptr1.ptr() == nullptr);
		TEST_ASSERT(ptr1.refcount() == 0);

		auto c = TestObject::count();

		TEST_ASSERT(TestObject::count() == 0);


		Ptr<TestObject> ptr2 = new TestObject("two");
		ptr1.reset(ptr2);

		TEST_ASSERT(ptr1.refcount() == 2);
		TEST_ASSERT(ptr2.refcount() == 2);
		TEST_ASSERT(TestObject::count() == 1);


		TestObject* ptr3 = new TestObject("two");
		ptr2.reset(ptr3);
		TEST_ASSERT(ptr1.refcount() == 1);

		Ptr<Derived> ptr4 = new Derived("son", 110);
		ptr2.reset(ptr4);
		TEST_ASSERT(ptr2.refcount() == 2);
	}

	TEST_ASSERT(TestObject::count() == 0);
}

class nocopy NONECOPYABLE {
public:
	nocopy() {};
};

void testException()
{
	try
	{
		throw yep::Error("xxxx");
	}
	catch (const yep::Error& e)
	{
		std::string desc = e.Description();
		std::cout << desc << std::endl;
	}
}

//int main(int argc, char* argv) {
//
//	nocopy a;
//	//nocopy b = a;
//
//
//	testException();
//
//	test_std_shared_ptr();
//	yep::unittest::UnitTest::RunAllTest();
//	return 0;
//}