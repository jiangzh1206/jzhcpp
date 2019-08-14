#include "UnitTest.h"

namespace yep {
	namespace unittest {


	void UnitTest::PrintInfo(const std::string& name)
	{
		//std::cout << name << std::endl;
	}

	void UnitTest::PrintError(const std::string&)
	{
	}

	typedef struct UnitTestLinkNode
	{
		UnitTest::TestCaseFun fun = nullptr;
		UnitTestLinkNode* next = nullptr;
	}UTLink;

	UTLink* Head = nullptr;
	UTLink** Tail = &Head;

	void UnitTest::PushTest(TestCaseFun test)
	{
		auto node = new UTLink;
		node->fun = test;
		// <add to tail>
		*Tail = node;
		Tail = &(node->next);
	}

	void UnitTest::RunAllTest()
	{
		auto current = Head;
		Head = nullptr;
		Tail = nullptr;
		while (current) {
			current->fun();
			auto temp = current;
			current = current->next;
			delete temp;
		}
	}



	} // namespace unittest
} // yep

