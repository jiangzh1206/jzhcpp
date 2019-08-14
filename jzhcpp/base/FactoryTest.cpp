#include "Factory.h"
#include <iostream>
using namespace yep;

#if 0
struct Message1 : public Message {
	void foo() {
		std::cout << "test" << std::endl;
	}
};

struct Message2 : public Message {
	Message2(int i) {}
	void foo() {
		std::cout << "test2" << std::endl;
	}
};

struct Message3 : public Message {
	Message3(int i, double j) {}
	void foo() {
		std::cout << "test3" << std::endl;
	}
};

struct Message4 : public Message {
	Message4(int i) {}
	void foo() {
		std::cout << "test4" << std::endl;
	}
};

enum
{
	MSG1 = 1,
	MSG2 = 2,
	MSG3 = 3,
	MSG4 = 4,
};

REGISTER_MESSAGE(Message1, MSG1);
REGISTER_MESSAGE1(Message2, MSG2, int);
REGISTER_MESSAGE1(Message3, MSG3, int, double);
REGISTER_MESSAGE2(Message4, MSG4, [](int i) {return new Message4(i);}, int);



int main() {
	auto p = produce_msg(MSG1);
	p->foo();

	auto p2 = produce_msg(MSG2, 1);
	p2->foo();

	auto p3 = produce_msg(MSG3, 1, 2.5); //注意，这里的参数类型必须和注册时候的参数类型严格对应
	p3->foo();

	auto p4 = produce_msg(MSG4, 1);
	p4->foo();

	return 0;
}
#endif