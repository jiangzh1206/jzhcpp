#include "Tuple.h"
#include <sstream>
#include <iostream>
#include <string>
#include <tuple>
#include <thread>
//#include <charconv>
#include <system_error>
//#include <string_view>
#include <array>
#include "YepTuple.h"

template<size_t N, typename T>
void Test(T t) {

	std::cout << typeid(N).name() << "\t" << t << std::endl;
}

#if 0
void test_tuple()
{
	using namespace yep;
	Tuple<> tp1;
	//auto h = TupleGet<0>(tp1);

	Tuple<int, int, const char*> tp = MakeTuple(1,4,"hello");
	auto v = TupleGet<0>(tp);
	auto v1 = TupleGet<1>(tp);
	auto v2 = TupleGet<2>(tp);

	TupleSet<0>(tp, 100);
	TupleSet<1>(tp, 200);
	TupleSet<2>(tp, "world");

	v = TupleGet<0>(tp);
	v1 = TupleGet<1>(tp);
	v2 = TupleGet<2>(tp);


	Tuple<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int,
		int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> tp2 
	{
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48 
	};
	auto v_tup2 = TupleGet<20>(tp2);

	auto size1 = tuple_size<decltype(tp1)>::value;
	auto size = tuple_size<decltype(tp)>::value;
	auto size2 = tuple_size<decltype(tp2)>::value;

	std::tuple<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int,
		int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> stdtp
	{
		1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48
	};

	int inta = 10;
	auto fun = [inta]() {return inta + 10; };
	std::tuple<int> inttp = fun();//inta;

	auto stdtp_v = std::get<40>(stdtp);
}
#endif

class TestDefault {
public:
	//constexpr TestDefault(int td) : td_(td) {}
	constexpr TestDefault() noexcept = default;


	int td_;
};

class TestDefault1 {
public:
	int td_;
};

struct st;
struct st1{};

#if 0
int main()
{
#if 0
	std::thread t([](){
		while (true)
		{

		}
	});
	std::thread t1([]() {
		while (true)
		{

		}
	});
	t.join();
	t1.join();
#endif
	//st st12;
	//st1 st11;

	int tt = 1, tt2 = 2;
	TestDefault td1{ tt }, td2{ tt2 };
	td1 = td2;

	TestDefault1 td3{tt}, td4{/*tt*/};
	td3 = td4;

	const char* str = R"-(Embedded---- )" characters)-";
	std::cout << str << std::endl;
	
	// Ê®¸ö¿Õ¸ñ
	std::string out(10, ' ');
	//auto[ptr, ec] = std::to_chars(out.data(), out.data() + out.size(), 12345);
	//if (ec == std::errc()) { /* Conversion successful. */ }

	//test_tuple();


	return 0;
}
#endif