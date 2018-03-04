#include <iostream>

template<class T>
void Print(T val)
{
	std::cout << val << std::endl;
}

template<class T, class... Args>
void Print(T val, Args... args)
{
	std::cout << val << std::endl;
	Print(args...);
}

int main()
{
	Print("what", "is", "this");

	getchar();
	return 0;
}