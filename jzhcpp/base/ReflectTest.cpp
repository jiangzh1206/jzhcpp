#include "Reflect.h"
#include <iostream>

#define Test(xx) what##xx

char* Test(XX)()
{
	return nullptr;
}

class Base {
public:
	Base() {
		std::cout << "Base" << std::endl;
	}
};

STRING_WRAPPER(Class1)

static yep::reflect::RegistClassCaller<
	Base, class Class1, string_wrapper_Class1> registcaller;

class Class1 : public Base {
public:
	Class1() {
		std::cout << "Class1" << std::endl;
	}
};

//RE_class(Class1, Base)
//{
//public:
//	Class1() {
//		std::cout << "Class1" << std::endl;
//	}
//};

//RE_class(Class2, Base)
//{
//public:
//	Class2() {
//		std::cout << "Class2" << std::endl;
//	}
//};

#if 0
int main() 
{
	
	Reflect(Base)["Class1"]->create();
	return 0;
}
#endif