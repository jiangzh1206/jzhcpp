#include <iostream>
#include "YepTuple.h"
#include <string>
#include <stdexcept>
#include <cassert>
#include <tuple>

//using namespace std;
using namespace yep;

constexpr void test_Get()
{
	int i = 1;
	auto t = MakeTuple(i, 0.5);
	i = Get<0>(t);
	auto d = Get<1>(t);
	assert(i == 1);
	assert(d == 0.5);
}


constexpr void test_tuple_size()
{
	auto t = MakeTuple(8, 0.5);
	auto int_size = TupleSize<YepTuple<int>>::value;
	auto size = TupleSize<decltype(t)>::value;

	assert(TupleSize<YepTuple<int>>::value == 1);
	assert(TupleSize<decltype(t)>::value == 2);
}

constexpr void test_Get_assign()
{
	auto t = MakeTuple(8, 0.5);
	Get<int>(t) = 5;
	Get<double>(t) = 4.4;
	assert(Get<0>(t) == 5);
	assert(Get<1>(t) == 4.4);
	assert(Get<double>(t) == 4.4);
}

constexpr void test_tie()
{
	int li = 5;
	double ld = 3.2;

	Tie(li, ld) = MakeTuple(4, 2.2);
	assert(li == 4);
	assert(ld == 2.2);

	Tie(ignore, ld) = MakeTuple(5, 6);
	assert(li == 4);
	assert(ld == 6);

	Tie(li, ignore) = MakeTuple(7, 8);
	assert(li == 7);
	assert(ld == 6);

	auto copyTuple = MakeTuple(2, 3.3);
	Tie(li, ld) = copyTuple;
	assert(li == 2);
	assert(ld == 3.3);

	auto moveTuple = MakeTuple(5, 4.4);
	Tie(li, ld) = std::move(moveTuple);
	assert(li == 5);
	assert(ld == 4.4);
}


constexpr void test_copy_assign()
{
	auto copyTuple = MakeTuple(2, 3.3);
	auto t2 = MakeTuple(1, 0.3);
	copyTuple = t2;
	assert(Get<0>(copyTuple) == 1);
	assert(Get<1>(copyTuple) == 0.3);
}

constexpr void test_initialization_list()
{
	YepTuple<int, double> tupInitList = { 1, 2.4 };
	assert(Get<1>(tupInitList) == 2.4);
}

constexpr void test_tuple_element()
{
	auto te = std::is_same_v<TupleElem<0, YepTuple<int, float>>::Type, int>;
	assert(te);
	te = std::is_same_v<TupleElem<1, YepTuple<int, float>>::Type, float>;
	assert(te);
}


template <typename Callable>
class Proxy {
	Callable c;
public:
	Proxy(Callable c) : c(c) {}
	template <class... Args>
	decltype(auto) operator()(Args&&... args) {
		return std::invoke(c, std::forward<Args>(args)...);
	}
};

template<typename Array, std::size_t... I>
decltype(auto) a2t_impl(const Array& a, std::integer_sequence<std::size_t, I...>) {
	return std::make_tuple(a[I]...);
}

template<typename T, std::size_t N, typename Indices = std::make_index_sequence<N>>
decltype(auto) a2t(const std::array<T, N>& a) {
	return a2t_impl(a, Indices());
}

#if 0
int main()
{
	auto add = [](int x, int y) {
		return x + y;
	};

	Proxy<decltype(add)> p{ add };
	assert(3 == p(1, 2)); // == 3

	// Invoke a Callable object with a tuple of arguments.

	std::apply(add, std::make_tuple(1, 2)); // == 3

	test_Get();
	test_tuple_size();
	test_Get_assign();
	test_tie();
	test_copy_assign();
	test_initialization_list();
	test_tuple_element();
	//test_tuple_cat();

	return 0;
}
#endif