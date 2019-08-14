#include <iostream>
#include <bitset>
#include <memory>
#include <string>
#include "SFINAE.h"
#include "test_sfinae.hpp"


template<size_t N>
void PrintBitset(std::bitset<N> const& bs)
{
	std::cout << bs. template to_string<char, std::char_traits<char>, std::allocator<char>>() << std::endl;
}

void Reverse()
{
	char c;
	std::cin >> c;
	if (c != 'x') Reverse();

	std::cout << c;
}

template<class T, size_t N>
void PrintArray(T(&data)[N])
{
	std::cout << "-------------PrintArray Begin------------" << std::endl;
	for (auto v : data)
	{
		std::cout << v << std::endl;
	}
	std::cout << "-------------PrintArray End------------" << std::endl;

}

template <class T, size_t n>
void insertionSort(T(&data)[n]) {
	for (size_t i = 1; i < n; ++i) 
	{
		T tmp = data[i];
		size_t pos = i - 1;
		for (; pos >= 0 && data[pos] > tmp; pos--)
		{
			data[pos + 1] = data[pos];
		}
		data[pos+1] = tmp;

		PrintArray(data);
	}
}

template<class T>
void selectSort(T* data, size_t n)
{
	for (size_t i = 0; i < n - 1; ++i)
	{
		size_t j = 0;
		for (j = i + 1; j < n; ++j)
		{
			if (data[i] > data[j])
			{
				T tmp = data[i];
				data[i] = data[j];
				data[j] = tmp;
			}

			// debug
			{
				std::cout << " 内00000000: -----------------" << std::endl;
				for (size_t i = 0; i < n; ++i)
				{
					std::cout << data[i] << std::endl;
				}
				std::cout << " 内11111111: -----------------" << std::endl;
			}
		}

		// debug
		{
			for (size_t i = 0; i < n; ++i)
			{
				std::cout << data[i] << std::endl;
			}
			std::cout << "第 " << i << ": -----------------" << std::endl;
		}
	}
}

using namespace std::string_literals;
int main()
{
	std::bitset<100> a;

	PrintBitset(a);
#if 0
	int data[5] = { 5,3,2,1,4 };
	PrintArray(data);

	selectSort(data, 5);
	
	std::cout << "hello"s << std::endl;
	//insertionSort(data);
	//Reverse();
#endif
	// SFINEA
	{
		// 传右值
		f_test_sfinae(1);

		st_sfinae<int> st;
		f_test_sfinae<st_sfinae<int>>(st);


		SFINAE_TEST::test_sfinae();

	}

	getchar();
	getchar();
	return 0;
}