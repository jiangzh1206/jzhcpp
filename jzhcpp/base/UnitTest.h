// UnitTest.h
// Definition of the UnitTest.
// Copyleft 2014 - 2018, jzh just do it


#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include "Basic.h"

namespace yep {

namespace unittest {
	
	class UnitTest {
	public:
		// <test case body>
		typedef void(*TestCaseFun)();

		static void PrintInfo(const std::string&);

		static void PrintError(const std::string&);

		// <add testcase>
		static void PushTest(TestCaseFun test);

		// <run testcase>
		static void RunAllTest();
	};


#define TEST_ASSERT_NULL(p) do{ if (p) throw Error("ptr is not null");} while(0)

#define TEST_ASSERT(x) do{if(!(x)) throw Error("unit test assert");} while(0)

#define TEST_CASE(NAME)																\
	extern void TESTCASE_##NAME();													\
	namespace uint_test_executor{													\
	class TESTCASE_RUNNER_##NAME{													\
	public:																			\
		static void RunTest(){														\
			yep::unittest::UnitTest::PrintInfo(#NAME);								\
			TESTCASE_##NAME();														\
		}																			\
		TESTCASE_RUNNER_##NAME() {													\
			yep::unittest::UnitTest::PushTest(&TESTCASE_RUNNER_##NAME::RunTest);	\
		}																			\
	} TESTCASE_RUNNER_##NAME##INSTANCE;												\
	}																				\
	void TESTCASE_##NAME()
	
#define TEST_PRINT(x) yep::unittest::UnitTest::PrintInfo(x)
	
	} // namespace unittest
} //  yep



#endif // !UNIT_TEST_H
