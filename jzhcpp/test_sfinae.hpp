#pragma once
#include "SFINAE.h"

namespace SFINAE_TEST {

	struct A {};
	inline std::string to_string(const A&) {
		return "I am is empty A";
	}

	struct B {
		std::string serialize()const {
			return "I am is B.serialize";
		}
	};

	struct C
	{
		std::string serialize;
	};
	inline std::string to_string(const C&) {
		return "I am is wrong serialize C";
	}


	struct D : A
	{
		std::string serialize() const {
			return "I am is D";
		}
	};

	template <class T>
	bool testHasSerialize(const T& t) {
		return SFINAE_98::hasSerialize<T>::value;
	}

	inline void test_sfinae() {
		{
			std::cout << SFINAE_98::hasSerialize<A>::value << std::endl;
			std::cout << SFINAE_98::hasSerialize<B>::value << std::endl;
			std::cout << SFINAE_98::hasSerialize<C>::value << std::endl;

			D d;
			A& a = d;
			std::cout << testHasSerialize(d) << std::endl;
			std::cout << testHasSerialize(a) << std::endl;
		}

		{
			A a;
			B b;
			C c;
			std::cout << SFINAE_98::serialize(a) << std::endl;
			std::cout << SFINAE_98::serialize(b) << std::endl;
			std::cout << SFINAE_98::serialize(c) << std::endl;

			//auto hasSerialize = SFINAE_14::is_valid([](auto&& x) -> decltype(x.serialize()) {});
			//auto ret = hasSerialize(b);
			//std::cout << ret.value << std::endl;
		}
		
	}
}