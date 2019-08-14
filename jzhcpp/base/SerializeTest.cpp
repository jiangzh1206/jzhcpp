#include "Serialization.h"
#include <iostream>

using namespace serialization;


class serial : public Serializable<serial>
{
public:
	int a;
	unsigned int ua;
	double b;
	std::string str;

	static std::string serialize(const serial& inc)
	{
		SerialOutput x;
		x << inc.a << inc.ua << inc.b << inc.str;
		return x.str();
	}
	static int deserialize(std::string inc, serial& box)
	{
		SerialInput x(inc);
		x >> box.a >> box.ua >> box.b >> box.str;
		return x.donesize();
	}
};

//#include <memory>
//int main()
//{
//	int a = 0;
//	std::shared_ptr<int> p = std::make_shared<int>(5);
//	std::cout << std::boolalpha;
//	std::cout << is_smart_pointer<int>::value << std::endl;
//	std::cout << is_smart_pointer<std::shared_ptr<int>>::value << std::endl;
//
//	return 0;
//}

#if 0
int main()
{
	serial s1;
	s1.a = 1;
	s1.ua = 2;
	s1.b = 10.10;
	s1.str = "xxxx";

	serial s2;
	s2.a = 3;
	s2.ua = 4;
	s2.b = 20.20;
	s2.str = "wwww";

	SerialOutput output;
	output << s1 << s2;

	std::string b = output.str();
	std::cout << b << std::endl;

	serial s3;
	serial s4;
	SerialInput input(b);
	input >> s3 >> s4;

	std::cout << s3.a << "\t"<< s3.ua << "\t" << s3.b << "\t" << s3.str << std::endl;

	std::cout << s4.a << "\t" << s4.ua << "\t" << s4.b << "\t" << s4.str << std::endl;

	return 0;
}
#endif

