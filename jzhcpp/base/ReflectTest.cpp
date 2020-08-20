#include "Reflect.h"
#include <iostream>

#include <type_traits>

#define GET_HEAD(FIRST,...) FIRST
#define GETARG_00(...)        BOOST_PP_CAT(GET_HEAD(__VA_ARGS__),)
#define GETARG_01(_0,...)     BOOST_PP_CAT(GET_HEAD(__VA_ARGS__),)
#define GETARG_02(_0,_1,...)  BOOST_PP_CAT(GET_HEAD(__VA_ARGS__),)
#define GETARG_03(_0,_1,_2,...)  BOOST_PP_CAT(GET_HEAD(__VA_ARGS__),)

// !msvc
#ifndef _MSC_VER
    #define GET_NTH_ARG(_1,  _2,  _3,  _4,  _5,  _6,  _7, n, ...) n
    #define GET_ARG_COUNT(...) GET_NTH_ARG(__VA_ARGS__, 7,  6,  5,  4,  3,  2,  1)
#endif // !

#define BOOST_PP_CAT_I(a, b) a ## b
#define BOOST_PP_CAT_OO(par) BOOST_PP_CAT_I ## par
#define BOOST_PP_CAT(a, b) BOOST_PP_CAT_OO((a, b))
#define BOOST_PP_VARIADIC_SIZE_I(e0, e1, e2, e3, e4, e5, e6, e7, size, ...) size
#define BOOST_PP_VARIADIC_SIZE(...) BOOST_PP_CAT(BOOST_PP_VARIADIC_SIZE_I(__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1), )


struct AnyType {
    template <typename T>
    operator T();
};
template <typename T, typename = void, typename ...Ts>
struct CountMember {
    constexpr static size_t value = sizeof...(Ts) - 1;
};

template <typename T, typename ...Ts>
struct CountMember < T, std::void_t<decltype(T{ Ts{}... }) >, Ts... > {
    constexpr static size_t value = CountMember<T, void, Ts..., AnyType>::value;
};

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
	
	{
		int first = GET_HEAD(4, 3, 2);
		GETARG_00(4, 3, 2);
		GETARG_01(4, 3, 2);
		int f2 = BOOST_PP_VARIADIC_SIZE(4, 3, 2, 7, 8, 9,5,4);
		f2 = GETARG_03(4, 3, 2,5);

		//int n = GET_ARG_COUNT(a,b,c);
		//n = metamacro_argcount(a, b, c);
		struct Test { int a; int b; int c; int d; };
		int a = CountMember<Test>::value;
	}
	
	Reflect(Base)["Class1"]->create();
	return 0;
}
#endif
