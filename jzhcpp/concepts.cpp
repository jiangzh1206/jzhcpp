#include <iostream>
#include <string>
#include <concepts>
#include <numeric>
#include <iterator>
#include <type_traits>

using namespace std::literals;

// 概念 "Hashable" 的声明，可被符合以下条件的任何类型 T 满足：
// 对于 T 类型的值 a，表达式 std::hash<T>{}(a) 可编译且其结果可转换为 std::size_t
template<typename T>
concept Hashable = requires(T a) {
	{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

struct meow {};

template<Hashable T>
void f(T) {}

// 语法
template<std::integral T>
void f(T) requires std::signed_integral<T> {}

template<typename T>
requires std::integral<T>&& std::signed_integral<T>
void f(T) {}


// msvc不支持 clang 支持
//std::integral auto add0(std::integral auto a, std::same_as<int> auto b)
//{
//	return a + b;
//}


// requires 表达式
template<typename T>
concept Addable = requires (T x) { x + x; };

// requires 子句，非 requires 表达式
template<typename T> requires Addable<T> 
T add(T a, T b) { return a + b; }

template<typename T>
requires requires (T x) { x + x; } // 随即的约束，注意关键字被使用两次
T add1(T a, T b) { return a + b; }


template <typename T>
requires std::integral<T> && requires { requires(sizeof(T) == 4); } && (sizeof(T) == 4)
T func(T x) { return x + 1; }


// 类型要求
template<typename T>
struct S {
	struct inner
	{

	};
};
template<typename T> using Ref = T&;
template<typename T> concept C =
requires {
	typename T::inner; // 要求的嵌套成员名
	typename S<T>;     // 要求的类模板特化
	typename Ref<T>;   // 要求的别名模板替换
};
template<C c>
void typeReq(c)
{
	std::cout << "类型要求" << std::endl;
}


int main()
{
	f("abc"s); // OK，std::string 满足 Hashable
	//f(meow{}); // 错误：meow 不满足 Hashable

	//std::cout << add0(1, 10) << std::endl;
	std::cout << add1(2, 10) << std::endl;

	std::cout << func(1) << std::endl;
	//std::cout << func(1.0) << std::endl;

	std::cout << add(1, 10) << std::endl;
	//std::cout << add("b", "aa") << std::endl;

	S<int> tr;
	typeReq(tr);

	return 0;
}

